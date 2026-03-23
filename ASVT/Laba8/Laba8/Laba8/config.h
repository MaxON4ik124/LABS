#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#define UART_BAUD               38400UL


#define UART_RX_BUF_SIZE        128u
#define UART_TX_BUF_SIZE        128u


#define MAX_KEYS                8u
#define DEFAULT_BLOCK_SIZE      16u
#define MIN_BLOCK_SIZE          1u
#define MAX_BLOCK_SIZE          64u


#define PROTO_SOF1              0x55u
#define PROTO_SOF2              0xAAu
#define PROTO_MAX_PAYLOAD       (2u + (2u * MAX_BLOCK_SIZE))


#define EEPROM_MAGIC            0xE3A3u
#define EEPROM_VERSION          0x0001u


typedef enum
{
    CMD_PING            = 0x01,
    CMD_GET_BLOCK_SIZE  = 0x02,
    CMD_SET_BLOCK_SIZE  = 0x03,

    CMD_KEY_ADD         = 0x10,
    CMD_KEY_DELETE      = 0x11,
    CMD_KEY_LIST        = 0x12,

    CMD_ENC_BEGIN       = 0x20,
    CMD_ENC_DATA        = 0x21,
    CMD_ENC_END         = 0x22,

    CMD_DEC_BEGIN       = 0x23,
    CMD_DEC_DATA        = 0x24,
    CMD_DEC_END         = 0x25,

    CMD_ERROR           = 0x7F
} command_t;

typedef enum
{
    ERR_NONE            = 0x00,
    ERR_BAD_FRAME       = 0x01,
    ERR_BAD_CRC         = 0x02,
    ERR_UNKNOWN_CMD     = 0x03,
    ERR_BAD_LENGTH      = 0x04,
    ERR_INVALID_ARG     = 0x05,
    ERR_NOT_FOUND       = 0x06,
    ERR_STORAGE_FULL    = 0x07,
    ERR_INVALID_STATE   = 0x08,
    ERR_DECRYPT_RANGE   = 0x09
} error_code_t;

#endif /* CONFIG_H */
