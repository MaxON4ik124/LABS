#include "protocol.h"

#include "config.h"
#include "uart.h"
#include "eeprom_store.h"
#include "crypto_elgamal.h"

#include <string.h>

typedef enum
{
    RX_WAIT_SOF1 = 0,
    RX_WAIT_SOF2,
    RX_CMD,
    RX_FLAGS,
    RX_LEN_LO,
    RX_LEN_HI,
    RX_PAYLOAD,
    RX_CRC
} rx_state_t;

typedef union
{
    struct
    {
        uint8_t active  : 1;
        uint8_t decrypt : 1;
        uint8_t reserved: 6;
    } bits;
    uint8_t raw;
} session_flags_t;

typedef struct
{
    session_flags_t flags;
    uint16_t a;
    uint16_t p;
    uint16_t x;
    uint16_t ay;
    uint16_t shared_secret;
    uint16_t block_size;
} crypto_session_t;

typedef struct
{
    rx_state_t state;
    uint8_t cmd;
    uint8_t flags;
    uint16_t length;
    uint16_t index;
    uint8_t payload[PROTO_MAX_PAYLOAD];
} rx_parser_t;

static rx_parser_t g_rx;
static crypto_session_t g_session;
static uint8_t g_tx_payload[PROTO_MAX_PAYLOAD];

static uint16_t rd_u16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static void wr_u16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)(value & 0xFFu);
    p[1] = (uint8_t)(value >> 8);
}

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

static void reset_parser(void)
{
    g_rx.state = RX_WAIT_SOF1;
    g_rx.cmd = 0u;
    g_rx.flags = 0u;
    g_rx.length = 0u;
    g_rx.index = 0u;
}

static void send_frame(uint8_t cmd, uint8_t flags, const uint8_t *payload, uint16_t len)
{
    uint8_t header[6];
    uint8_t crc;

    header[0] = PROTO_SOF1;
    header[1] = PROTO_SOF2;
    header[2] = cmd;
    header[3] = flags;
    header[4] = (uint8_t)(len & 0xFFu);
    header[5] = (uint8_t)(len >> 8);

    crc = frame_crc(cmd, flags, len, payload);

    uart_write_block(header, sizeof(header));
    if (len != 0u)
    {
        uart_write_block(payload, len);
    }
    uart_write_byte(crc);
}

static void send_ok(uint8_t cmd, const uint8_t *payload, uint16_t len)
{
    send_frame((uint8_t)(cmd | 0x80u), 0u, payload, len);
}

static void send_error(uint8_t failed_cmd, error_code_t code)
{
    uint8_t payload[2];
    payload[0] = failed_cmd;
    payload[1] = (uint8_t)code;
    send_frame(CMD_ERROR, 0u, payload, 2u);
}

static void clear_session(void)
{
    memset(&g_session, 0, sizeof(g_session));
}

static void handle_ping(void)
{
    static const uint8_t k_sig[4] = { 'E', 'G', 'A', '3' };
    send_ok(CMD_PING, k_sig, sizeof(k_sig));
}

static void handle_get_block_size(void)
{
    const uint16_t size = store_get_block_size();
    wr_u16(g_tx_payload, size);
    send_ok(CMD_GET_BLOCK_SIZE, g_tx_payload, 2u);
}

static void handle_set_block_size(const uint8_t *payload, uint16_t len)
{
    uint16_t size;

    if (len != 2u)
    {
        send_error(CMD_SET_BLOCK_SIZE, ERR_BAD_LENGTH);
        return;
    }

    size = rd_u16(payload);
    if ((size < MIN_BLOCK_SIZE) || (size > MAX_BLOCK_SIZE))
    {
        send_error(CMD_SET_BLOCK_SIZE, ERR_INVALID_ARG);
        return;
    }

    store_set_block_size(size);
    wr_u16(g_tx_payload, size);
    send_ok(CMD_SET_BLOCK_SIZE, g_tx_payload, 2u);
}

static void handle_key_add(const uint8_t *payload, uint16_t len)
{
    int8_t index;
    uint16_t a;
    uint16_t p;
    uint16_t x;

    if (len != 6u)
    {
        send_error(CMD_KEY_ADD, ERR_BAD_LENGTH);
        return;
    }

    a = rd_u16(payload + 0u);
    p = rd_u16(payload + 2u);
    x = rd_u16(payload + 4u);

    if ((a == 0u) || (p == 0u) || (x == 0u))
    {
        send_error(CMD_KEY_ADD, ERR_INVALID_ARG);
        return;
    }

    index = store_add_key(a, p, x);
    if (index < 0)
    {
        send_error(CMD_KEY_ADD, ERR_STORAGE_FULL);
        return;
    }

    g_tx_payload[0] = (uint8_t)index;
    send_ok(CMD_KEY_ADD, g_tx_payload, 1u);
}

static void handle_key_delete(const uint8_t *payload, uint16_t len)
{
    if (len != 1u)
    {
        send_error(CMD_KEY_DELETE, ERR_BAD_LENGTH);
        return;
    }

    if (!store_delete_key(payload[0]))
    {
        send_error(CMD_KEY_DELETE, ERR_INVALID_ARG);
        return;
    }

    send_ok(CMD_KEY_DELETE, NULL, 0u);
}

static void handle_key_list(void)
{
    key_record_t records[MAX_KEYS];
    uint8_t indexes[MAX_KEYS];
    uint8_t count;
    uint8_t i;
    uint16_t pos = 0u;

    count = store_list_keys(records, MAX_KEYS, indexes);
    g_tx_payload[pos++] = count;

    for (i = 0u; i < count; ++i)
    {
        g_tx_payload[pos++] = indexes[i];
        g_tx_payload[pos++] = records[i].flags_raw;
        wr_u16(g_tx_payload + pos, records[i].a); pos += 2u;
        wr_u16(g_tx_payload + pos, records[i].p); pos += 2u;
        wr_u16(g_tx_payload + pos, records[i].x); pos += 2u;
    }

    send_ok(CMD_KEY_LIST, g_tx_payload, pos);
}

static void handle_enc_begin(const uint8_t *payload, uint16_t len)
{
    key_record_t rec;
    int8_t key_index;
    uint8_t y;

    if (len != 4u)
    {
        send_error(CMD_ENC_BEGIN, ERR_BAD_LENGTH);
        return;
    }

    clear_session();
    g_session.a = rd_u16(payload + 0u);
    g_session.p = rd_u16(payload + 2u);
    g_session.block_size = store_get_block_size();

    key_index = store_find_key(g_session.a, g_session.p, &rec);
    if (key_index < 0)
    {
        send_error(CMD_ENC_BEGIN, ERR_NOT_FOUND);
        return;
    }

    (void)key_index;
    g_session.x = rec.x;
    y = store_get_next_y();
    g_session.ay = crypto_mod_pow_u16(g_session.a, y, g_session.p);
    g_session.shared_secret = crypto_mod_pow_u16(g_session.ay, g_session.x, g_session.p);
    g_session.flags.raw = 0u;
    g_session.flags.bits.active = 1u;
    g_session.flags.bits.decrypt = 0u;

    wr_u16(g_tx_payload + 0u, g_session.ay);
    wr_u16(g_tx_payload + 2u, g_session.block_size);
    send_ok(CMD_ENC_BEGIN, g_tx_payload, 4u);
}

static void handle_enc_data(const uint8_t *payload, uint16_t len)
{
    uint16_t plain_len;
    uint16_t i;
    uint16_t pos = 0u;

    if ((g_session.flags.bits.active == 0u) || (g_session.flags.bits.decrypt != 0u))
    {
        send_error(CMD_ENC_DATA, ERR_INVALID_STATE);
        return;
    }

    if (len < 2u)
    {
        send_error(CMD_ENC_DATA, ERR_BAD_LENGTH);
        return;
    }

    plain_len = rd_u16(payload);
    if ((plain_len > g_session.block_size) || (plain_len > MAX_BLOCK_SIZE) || (len != (uint16_t)(2u + plain_len)))
    {
        send_error(CMD_ENC_DATA, ERR_BAD_LENGTH);
        return;
    }

    wr_u16(g_tx_payload + pos, plain_len); pos += 2u;
    for (i = 0u; i < plain_len; ++i)
    {
        uint16_t c = crypto_encrypt_byte(payload[2u + i], g_session.shared_secret, g_session.p);
        wr_u16(g_tx_payload + pos, c);
        pos += 2u;
    }

    send_ok(CMD_ENC_DATA, g_tx_payload, pos);
}

static void handle_enc_end(uint16_t len)
{
    if (len != 0u)
    {
        send_error(CMD_ENC_END, ERR_BAD_LENGTH);
        return;
    }

    clear_session();
    send_ok(CMD_ENC_END, NULL, 0u);
}

static void handle_dec_begin(const uint8_t *payload, uint16_t len)
{
    key_record_t rec;
    int8_t key_index;

    if (len != 6u)
    {
        send_error(CMD_DEC_BEGIN, ERR_BAD_LENGTH);
        return;
    }

    clear_session();
    g_session.a = rd_u16(payload + 0u);
    g_session.p = rd_u16(payload + 2u);
    g_session.ay = rd_u16(payload + 4u);
    g_session.block_size = store_get_block_size();

    key_index = store_find_key(g_session.a, g_session.p, &rec);
    if (key_index < 0)
    {
        send_error(CMD_DEC_BEGIN, ERR_NOT_FOUND);
        return;
    }

    (void)key_index;
    g_session.x = rec.x;
    g_session.shared_secret = crypto_mod_pow_u16(g_session.ay, g_session.x, g_session.p);
    g_session.flags.raw = 0u;
    g_session.flags.bits.active = 1u;
    g_session.flags.bits.decrypt = 1u;

    wr_u16(g_tx_payload, g_session.block_size);
    send_ok(CMD_DEC_BEGIN, g_tx_payload, 2u);
}

static void handle_dec_data(const uint8_t *payload, uint16_t len)
{
    uint16_t plain_len;
    uint16_t i;
    uint16_t pos = 0u;

    if ((g_session.flags.bits.active == 0u) || (g_session.flags.bits.decrypt == 0u))
    {
        send_error(CMD_DEC_DATA, ERR_INVALID_STATE);
        return;
    }

    if (len < 2u)
    {
        send_error(CMD_DEC_DATA, ERR_BAD_LENGTH);
        return;
    }

    plain_len = rd_u16(payload);
    if ((plain_len > g_session.block_size) || (plain_len > MAX_BLOCK_SIZE) || (len != (uint16_t)(2u + (2u * plain_len))))
    {
        send_error(CMD_DEC_DATA, ERR_BAD_LENGTH);
        return;
    }

    wr_u16(g_tx_payload + pos, plain_len); pos += 2u;
    for (i = 0u; i < plain_len; ++i)
    {
        const uint16_t cipher = rd_u16(payload + 2u + (2u * i));
        uint8_t plain;

        if (!crypto_decrypt_word(cipher, g_session.shared_secret, g_session.p, &plain))
        {
            send_error(CMD_DEC_DATA, ERR_DECRYPT_RANGE);
            return;
        }
        g_tx_payload[pos++] = plain;
    }

    send_ok(CMD_DEC_DATA, g_tx_payload, pos);
}

static void handle_dec_end(uint16_t len)
{
    if (len != 0u)
    {
        send_error(CMD_DEC_END, ERR_BAD_LENGTH);
        return;
    }

    clear_session();
    send_ok(CMD_DEC_END, NULL, 0u);
}

static void handle_command(uint8_t cmd, uint8_t flags, const uint8_t *payload, uint16_t len)
{
    (void)flags;

    switch (cmd)
    {
        case CMD_PING:           handle_ping(); break;
        case CMD_GET_BLOCK_SIZE: handle_get_block_size(); break;
        case CMD_SET_BLOCK_SIZE: handle_set_block_size(payload, len); break;
        case CMD_KEY_ADD:        handle_key_add(payload, len); break;
        case CMD_KEY_DELETE:     handle_key_delete(payload, len); break;
        case CMD_KEY_LIST:       handle_key_list(); break;
        case CMD_ENC_BEGIN:      handle_enc_begin(payload, len); break;
        case CMD_ENC_DATA:       handle_enc_data(payload, len); break;
        case CMD_ENC_END:        handle_enc_end(len); break;
        case CMD_DEC_BEGIN:      handle_dec_begin(payload, len); break;
        case CMD_DEC_DATA:       handle_dec_data(payload, len); break;
        case CMD_DEC_END:        handle_dec_end(len); break;
        default:
            send_error(cmd, ERR_UNKNOWN_CMD);
            break;
    }
}

void protocol_init(void)
{
    reset_parser();
    clear_session();
}

void protocol_poll(void)
{
    uint8_t byte;

    while (uart_read_byte(&byte))
    {
        switch (g_rx.state)
        {
            case RX_WAIT_SOF1:
                if (byte == PROTO_SOF1)
                {
                    g_rx.state = RX_WAIT_SOF2;
                }
                break;

            case RX_WAIT_SOF2:
                if (byte == PROTO_SOF2)
                {
                    g_rx.state = RX_CMD;
                }
                else
                {
                    reset_parser();
                }
                break;

            case RX_CMD:
                g_rx.cmd = byte;
                g_rx.state = RX_FLAGS;
                break;

            case RX_FLAGS:
                g_rx.flags = byte;
                g_rx.state = RX_LEN_LO;
                break;

            case RX_LEN_LO:
                g_rx.length = byte;
                g_rx.state = RX_LEN_HI;
                break;

            case RX_LEN_HI:
                g_rx.length |= (uint16_t)((uint16_t)byte << 8);
                g_rx.index = 0u;
                if (g_rx.length > PROTO_MAX_PAYLOAD)
                {
                    send_error(g_rx.cmd, ERR_BAD_LENGTH);
                    reset_parser();
                }
                else if (g_rx.length == 0u)
                {
                    g_rx.state = RX_CRC;
                }
                else
                {
                    g_rx.state = RX_PAYLOAD;
                }
                break;

            case RX_PAYLOAD:
                g_rx.payload[g_rx.index++] = byte;
                if (g_rx.index >= g_rx.length)
                {
                    g_rx.state = RX_CRC;
                }
                break;

            case RX_CRC:
            {
                const uint8_t expected = frame_crc(g_rx.cmd, g_rx.flags, g_rx.length, g_rx.payload);
                if (byte == expected)
                {
                    handle_command(g_rx.cmd, g_rx.flags, g_rx.payload, g_rx.length);
                }
                else
                {
                    send_error(g_rx.cmd, ERR_BAD_CRC);
                }
                reset_parser();
                break;
            }

            default:
                reset_parser();
                break;
        }
    }
}
