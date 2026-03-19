#ifndef PROTOCOL_HOST_H
#define PROTOCOL_HOST_H

#include <stdint.h>
#include <stddef.h>

#include "serial_port.h"

#define HOST_SOF1              0x55u
#define HOST_SOF2              0xAAu
#define HOST_MAX_PAYLOAD       512u
#define HOST_TIMEOUT_MS        3000u
#define HOST_UART_BAUD         38400u

typedef enum
{
    HOST_CMD_PING            = 0x01,
    HOST_CMD_GET_BLOCK_SIZE  = 0x02,
    HOST_CMD_SET_BLOCK_SIZE  = 0x03,
    HOST_CMD_KEY_ADD         = 0x10,
    HOST_CMD_KEY_DELETE      = 0x11,
    HOST_CMD_KEY_LIST        = 0x12,
    HOST_CMD_ENC_BEGIN       = 0x20,
    HOST_CMD_ENC_DATA        = 0x21,
    HOST_CMD_ENC_END         = 0x22,
    HOST_CMD_DEC_BEGIN       = 0x23,
    HOST_CMD_DEC_DATA        = 0x24,
    HOST_CMD_DEC_END         = 0x25,
    HOST_CMD_ERROR           = 0x7F
} host_command_t;

typedef struct
{
    uint8_t cmd;
    uint8_t flags;
    uint16_t len;
    uint8_t payload[HOST_MAX_PAYLOAD];
} host_packet_t;

const char *proto_error_to_string(uint8_t error_code);
void proto_wr_u16(uint8_t *p, uint16_t value);
uint16_t proto_rd_u16(const uint8_t *p);
void proto_wr_u32(uint8_t *p, uint32_t value);
uint32_t proto_rd_u32(const uint8_t *p);

int proto_send_packet(serial_port_t *sp, uint8_t cmd, uint8_t flags, const uint8_t *payload, uint16_t len);
int proto_recv_packet(serial_port_t *sp, host_packet_t *pkt, uint32_t timeout_ms);
int proto_exchange(serial_port_t *sp,
                   uint8_t cmd,
                   const uint8_t *payload,
                   uint16_t len,
                   host_packet_t *response,
                   uint8_t *mcu_error_code);

#endif /* PROTOCOL_HOST_H */
