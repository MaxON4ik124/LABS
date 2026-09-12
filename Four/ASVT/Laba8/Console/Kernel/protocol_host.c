#include "protocol_host.h"

#include <string.h>

static uint8_t crc8_update(uint8_t crc, uint8_t data)
{
    uint8_t i;
    crc ^= data;
    for (i = 0u; i < 8u; ++i)
    {
        if ((crc & 0x80u) != 0u)
        {
            crc = (uint8_t)((crc << 1u) ^ 0x07u);
        }
        else
        {
            crc <<= 1u;
        }
    }
    return crc;
}

static uint8_t frame_crc(uint8_t cmd, uint8_t flags, uint16_t len, const uint8_t *payload)
{
    uint8_t crc = 0u;
    uint16_t i;
    crc = crc8_update(crc, cmd);
    crc = crc8_update(crc, flags);
    crc = crc8_update(crc, (uint8_t)(len & 0xFFu));
    crc = crc8_update(crc, (uint8_t)(len >> 8));
    for (i = 0u; i < len; ++i)
    {
        crc = crc8_update(crc, payload[i]);
    }
    return crc;
}

const char *proto_error_to_string(uint8_t error_code)
{
    switch (error_code)
    {
        case 0x00: return "OK";
        case 0x01: return "bad frame";
        case 0x02: return "bad CRC";
        case 0x03: return "unknown command";
        case 0x04: return "bad length";
        case 0x05: return "invalid argument";
        case 0x06: return "key not found";
        case 0x07: return "storage full";
        case 0x08: return "invalid state";
        case 0x09: return "decryption range error";
        default:   return "unknown MCU error";
    }
}

void proto_wr_u16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)(value & 0xFFu);
    p[1] = (uint8_t)(value >> 8);
}

uint16_t proto_rd_u16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

void proto_wr_u32(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)(value & 0xFFu);
    p[1] = (uint8_t)((value >> 8) & 0xFFu);
    p[2] = (uint8_t)((value >> 16) & 0xFFu);
    p[3] = (uint8_t)((value >> 24) & 0xFFu);
}

uint32_t proto_rd_u32(const uint8_t *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int proto_send_packet(serial_port_t *sp, uint8_t cmd, uint8_t flags, const uint8_t *payload, uint16_t len)
{
    uint8_t hdr[6];
    uint8_t crc;

    if (len > HOST_MAX_PAYLOAD)
    {
        return -1;
    }

    hdr[0] = HOST_SOF1;
    hdr[1] = HOST_SOF2;
    hdr[2] = cmd;
    hdr[3] = flags;
    hdr[4] = (uint8_t)(len & 0xFFu);
    hdr[5] = (uint8_t)(len >> 8);
    crc = frame_crc(cmd, flags, len, payload);

    if (serial_write_all(sp, hdr, sizeof(hdr)) != 0)
    {
        return -2;
    }
    if ((len > 0u) && (serial_write_all(sp, payload, len) != 0))
    {
        return -3;
    }
    if (serial_write_all(sp, &crc, 1u) != 0)
    {
        return -4;
    }
    return 0;
}

int proto_recv_packet(serial_port_t *sp, host_packet_t *pkt, uint32_t timeout_ms)
{
    uint8_t byte;
    uint8_t crc_rx;
    uint8_t crc_calc;

    for (;;)
    {
        if (serial_read_exact(sp, &byte, 1u, timeout_ms) != 0)
        {
            return -1;
        }
        if (byte != HOST_SOF1)
        {
            continue;
        }
        if (serial_read_exact(sp, &byte, 1u, timeout_ms) != 0)
        {
            return -2;
        }
        if (byte == HOST_SOF2)
        {
            break;
        }
    }

    if (serial_read_exact(sp, &pkt->cmd, 1u, timeout_ms) != 0) return -3;
    if (serial_read_exact(sp, &pkt->flags, 1u, timeout_ms) != 0) return -4;
    if (serial_read_exact(sp, &byte, 1u, timeout_ms) != 0) return -5;
    pkt->len = byte;
    if (serial_read_exact(sp, &byte, 1u, timeout_ms) != 0) return -6;
    pkt->len |= (uint16_t)((uint16_t)byte << 8);

    if (pkt->len > HOST_MAX_PAYLOAD)
    {
        return -7;
    }
    if (pkt->len > 0u)
    {
        if (serial_read_exact(sp, pkt->payload, pkt->len, timeout_ms) != 0)
        {
            return -8;
        }
    }
    if (serial_read_exact(sp, &crc_rx, 1u, timeout_ms) != 0)
    {
        return -9;
    }

    crc_calc = frame_crc(pkt->cmd, pkt->flags, pkt->len, pkt->payload);
    if (crc_calc != crc_rx)
    {
        return -10;
    }
    return 0;
}

int proto_exchange(serial_port_t *sp,
                   uint8_t cmd,
                   const uint8_t *payload,
                   uint16_t len,
                   host_packet_t *response,
                   uint8_t *mcu_error_code)
{
    int rc;

    if (mcu_error_code != NULL)
    {
        *mcu_error_code = 0u;
    }

    rc = proto_send_packet(sp, cmd, 0u, payload, len);
    if (rc != 0)
    {
        return rc;
    }

    rc = proto_recv_packet(sp, response, HOST_TIMEOUT_MS);
    if (rc != 0)
    {
        return rc;
    }

    if (response->cmd == (uint8_t)(cmd | 0x80u))
    {
        return 0;
    }

    if ((response->cmd == HOST_CMD_ERROR) && (response->len >= 2u) && (response->payload[0] == cmd))
    {
        if (mcu_error_code != NULL)
        {
            *mcu_error_code = response->payload[1];
        }
        return 1;
    }

    return -20;
}
