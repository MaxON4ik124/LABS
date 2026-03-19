#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "serial_port.h"
#include "protocol_host.h"

#define ENC_MAGIC "EGA3"
#define FILE_HDR_SIZE 16u

static int parse_u16_arg(const char *s, uint16_t *out)
{
    char *end = NULL;
    unsigned long v = strtoul(s, &end, 0);
    if ((s == end) || (*end != '\0') || (v > 65535ul))
    {
        return -1;
    }
    *out = (uint16_t)v;
    return 0;
}

static void print_usage(const char *exe)
{
    printf("Usage:\n");
    printf("  %s <PORT> ping\n", exe);
    printf("  %s <PORT> block get\n", exe);
    printf("  %s <PORT> block set <size>\n", exe);
    printf("  %s <PORT> keys list\n", exe);
    printf("  %s <PORT> keys add <a> <p> <x>\n", exe);
    printf("  %s <PORT> keys del <index>\n", exe);
    printf("  %s <PORT> encrypt <a> <p> <input.bin> <output.ega>\n", exe);
    printf("  %s <PORT> decrypt <input.ega> <output.bin>\n", exe);
    printf("\nExample:\n");
    printf("  %s COM3 keys add 6397 8963 7499\n", exe);
    printf("  %s COM3 block set 32\n", exe);
    printf("  %s COM3 encrypt 6397 8963 in.bin out.ega\n", exe);
    printf("  %s COM3 decrypt out.ega restored.bin\n", exe);
}

static int exchange_or_print(serial_port_t *sp,
                             uint8_t cmd,
                             const uint8_t *payload,
                             uint16_t len,
                             host_packet_t *resp)
{
    uint8_t mcu_err = 0u;
    int rc = proto_exchange(sp, cmd, payload, len, resp, &mcu_err);
    if (rc == 1)
    {
        fprintf(stderr, "MCU error: %s (0x%02X)\n", proto_error_to_string(mcu_err), mcu_err);
        return -1;
    }
    if (rc != 0)
    {
        fprintf(stderr, "Serial/protocol exchange failed: %d\n", rc);
        return -2;
    }
    return 0;
}

static int cmd_ping(serial_port_t *sp)
{
    host_packet_t resp;
    if (exchange_or_print(sp, HOST_CMD_PING, NULL, 0u, &resp) != 0)
    {
        return 1;
    }
    printf("MCU replied: %.*s\n", (int)resp.len, (const char *)resp.payload);
    return 0;
}

static int cmd_block_get(serial_port_t *sp)
{
    host_packet_t resp;
    if (exchange_or_print(sp, HOST_CMD_GET_BLOCK_SIZE, NULL, 0u, &resp) != 0)
    {
        return 1;
    }
    if (resp.len != 2u)
    {
        fprintf(stderr, "Unexpected response length\n");
        return 1;
    }
    printf("Current block size: %u bytes\n", (unsigned)proto_rd_u16(resp.payload));
    return 0;
}

static int cmd_block_set(serial_port_t *sp, const char *size_s)
{
    host_packet_t resp;
    uint8_t payload[2];
    uint16_t size;

    if (parse_u16_arg(size_s, &size) != 0)
    {
        fprintf(stderr, "Invalid block size\n");
        return 1;
    }

    proto_wr_u16(payload, size);
    if (exchange_or_print(sp, HOST_CMD_SET_BLOCK_SIZE, payload, 2u, &resp) != 0)
    {
        return 1;
    }
    printf("New block size: %u bytes\n", (unsigned)proto_rd_u16(resp.payload));
    return 0;
}

static int cmd_keys_add(serial_port_t *sp, const char *a_s, const char *p_s, const char *x_s)
{
    host_packet_t resp;
    uint8_t payload[6];
    uint16_t a, p, x;

    if ((parse_u16_arg(a_s, &a) != 0) || (parse_u16_arg(p_s, &p) != 0) || (parse_u16_arg(x_s, &x) != 0))
    {
        fprintf(stderr, "Invalid key parameters\n");
        return 1;
    }

    proto_wr_u16(payload + 0u, a);
    proto_wr_u16(payload + 2u, p);
    proto_wr_u16(payload + 4u, x);

    if (exchange_or_print(sp, HOST_CMD_KEY_ADD, payload, 6u, &resp) != 0)
    {
        return 1;
    }
    printf("Key stored at slot %u\n", (unsigned)resp.payload[0]);
    return 0;
}

static int cmd_keys_del(serial_port_t *sp, const char *idx_s)
{
    host_packet_t resp;
    uint16_t index;
    uint8_t payload[1];

    if (parse_u16_arg(idx_s, &index) != 0 || index > 255u)
    {
        fprintf(stderr, "Invalid index\n");
        return 1;
    }
    payload[0] = (uint8_t)index;

    if (exchange_or_print(sp, HOST_CMD_KEY_DELETE, payload, 1u, &resp) != 0)
    {
        return 1;
    }
    printf("Key slot %u deleted\n", (unsigned)index);
    return 0;
}

static int cmd_keys_list(serial_port_t *sp)
{
    host_packet_t resp;
    uint8_t count;
    uint8_t i;
    uint16_t pos;

    if (exchange_or_print(sp, HOST_CMD_KEY_LIST, NULL, 0u, &resp) != 0)
    {
        return 1;
    }
    if (resp.len < 1u)
    {
        fprintf(stderr, "Bad key list response\n");
        return 1;
    }

    count = resp.payload[0];
    pos = 1u;
    printf("Stored keys: %u\n", (unsigned)count);
    for (i = 0u; i < count; ++i)
    {
        uint8_t idx;
        uint8_t flags;
        uint16_t a, p, x;
        if ((pos + 8u) > resp.len)
        {
            fprintf(stderr, "Truncated key list\n");
            return 1;
        }
        idx = resp.payload[pos++];
        flags = resp.payload[pos++];
        a = proto_rd_u16(resp.payload + pos); pos += 2u;
        p = proto_rd_u16(resp.payload + pos); pos += 2u;
        x = proto_rd_u16(resp.payload + pos); pos += 2u;
        printf("  [%u] flags=0x%02X a=%u p=%u x=%u\n",
               (unsigned)idx, (unsigned)flags, (unsigned)a, (unsigned)p, (unsigned)x);
    }
    return 0;
}

static int write_enc_header(FILE *fout, uint16_t a, uint16_t p, uint16_t ay, uint16_t block_size, uint32_t plain_size)
{
    uint8_t hdr[FILE_HDR_SIZE];
    memcpy(hdr + 0u, ENC_MAGIC, 4u);
    proto_wr_u16(hdr + 4u, a);
    proto_wr_u16(hdr + 6u, p);
    proto_wr_u16(hdr + 8u, ay);
    proto_wr_u16(hdr + 10u, block_size);
    proto_wr_u32(hdr + 12u, plain_size);
    return (fwrite(hdr, 1u, sizeof(hdr), fout) == sizeof(hdr)) ? 0 : -1;
}

static int read_enc_header(FILE *fin, uint16_t *a, uint16_t *p, uint16_t *ay, uint16_t *block_size, uint32_t *plain_size)
{
    uint8_t hdr[FILE_HDR_SIZE];
    if (fread(hdr, 1u, sizeof(hdr), fin) != sizeof(hdr))
    {
        return -1;
    }
    if (memcmp(hdr, ENC_MAGIC, 4u) != 0)
    {
        return -2;
    }
    *a = proto_rd_u16(hdr + 4u);
    *p = proto_rd_u16(hdr + 6u);
    *ay = proto_rd_u16(hdr + 8u);
    *block_size = proto_rd_u16(hdr + 10u);
    *plain_size = proto_rd_u32(hdr + 12u);
    return 0;
}

static uint32_t file_size_u32(FILE *f)
{
    long cur = ftell(f);
    long end;
    if (cur < 0)
    {
        return 0u;
    }
    if (fseek(f, 0L, SEEK_END) != 0)
    {
        return 0u;
    }
    end = ftell(f);
    if (end < 0)
    {
        return 0u;
    }
    if (fseek(f, 0L, SEEK_SET) != 0)
    {
        return 0u;
    }
    return (uint32_t)end;
}

static int cmd_encrypt(serial_port_t *sp,
                       const char *a_s,
                       const char *p_s,
                       const char *input_path,
                       const char *output_path)
{
    FILE *fin = NULL;
    FILE *fout = NULL;
    host_packet_t resp;
    uint8_t txbuf[2u + 64u];
    uint16_t a, p, ay, block_size;
    uint32_t total_plain_size;
    size_t rd;
    int rc = 1;

    if ((parse_u16_arg(a_s, &a) != 0) || (parse_u16_arg(p_s, &p) != 0))
    {
        fprintf(stderr, "Invalid a/p values\n");
        return 1;
    }

    fin = fopen(input_path, "rb");
    if (fin == NULL)
    {
        fprintf(stderr, "Cannot open input file '%s': %s\n", input_path, strerror(errno));
        return 1;
    }
    total_plain_size = file_size_u32(fin);

    fout = fopen(output_path, "wb");
    if (fout == NULL)
    {
        fprintf(stderr, "Cannot open output file '%s': %s\n", output_path, strerror(errno));
        fclose(fin);
        return 1;
    }

    proto_wr_u16(txbuf + 0u, a);
    proto_wr_u16(txbuf + 2u, p);
    if (exchange_or_print(sp, HOST_CMD_ENC_BEGIN, txbuf, 4u, &resp) != 0)
    {
        goto cleanup;
    }
    if (resp.len != 4u)
    {
        fprintf(stderr, "Bad ENC_BEGIN response\n");
        goto cleanup;
    }

    ay = proto_rd_u16(resp.payload + 0u);
    block_size = proto_rd_u16(resp.payload + 2u);
    if (block_size > 64u)
    {
        fprintf(stderr, "MCU block size is too large for this host build (%u)\n", (unsigned)block_size);
        goto cleanup;
    }

    if (write_enc_header(fout, a, p, ay, block_size, total_plain_size) != 0)
    {
        fprintf(stderr, "Failed to write encrypted header\n");
        goto cleanup;
    }

    while ((rd = fread(txbuf + 2u, 1u, block_size, fin)) > 0u)
    {
        proto_wr_u16(txbuf + 0u, (uint16_t)rd);
        if (exchange_or_print(sp, HOST_CMD_ENC_DATA, txbuf, (uint16_t)(2u + rd), &resp) != 0)
        {
            goto cleanup_with_end;
        }
        if ((resp.len != (uint16_t)(2u + (2u * rd))) || (proto_rd_u16(resp.payload) != (uint16_t)rd))
        {
            fprintf(stderr, "Bad ENC_DATA response\n");
            goto cleanup_with_end;
        }
        if (fwrite(resp.payload + 2u, 1u, (size_t)(2u * rd), fout) != (size_t)(2u * rd))
        {
            fprintf(stderr, "Write error while saving ciphertext\n");
            goto cleanup_with_end;
        }
    }

    if (ferror(fin))
    {
        fprintf(stderr, "Read error while processing input file\n");
        goto cleanup_with_end;
    }

    if (exchange_or_print(sp, HOST_CMD_ENC_END, NULL, 0u, &resp) != 0)
    {
        goto cleanup;
    }

    printf("Encryption completed. ay=%u, block_size=%u, plain_size=%u bytes\n",
           (unsigned)ay, (unsigned)block_size, (unsigned)total_plain_size);
    rc = 0;
    goto cleanup;

cleanup_with_end:
    (void)exchange_or_print(sp, HOST_CMD_ENC_END, NULL, 0u, &resp);
cleanup:
    if (fin != NULL) fclose(fin);
    if (fout != NULL) fclose(fout);
    return rc;
}

static int cmd_decrypt(serial_port_t *sp, const char *input_path, const char *output_path)
{
    FILE *fin = NULL;
    FILE *fout = NULL;
    host_packet_t resp;
    uint8_t txbuf[2u + (2u * 64u)];
    uint16_t a, p, ay, hdr_block_size, device_block_size;
    uint32_t plain_size, remaining;
    int rc = 1;

    fin = fopen(input_path, "rb");
    if (fin == NULL)
    {
        fprintf(stderr, "Cannot open encrypted input '%s': %s\n", input_path, strerror(errno));
        return 1;
    }
    if (read_enc_header(fin, &a, &p, &ay, &hdr_block_size, &plain_size) != 0)
    {
        fprintf(stderr, "Bad encrypted file header\n");
        fclose(fin);
        return 1;
    }

    fout = fopen(output_path, "wb");
    if (fout == NULL)
    {
        fprintf(stderr, "Cannot open output '%s': %s\n", output_path, strerror(errno));
        fclose(fin);
        return 1;
    }

    proto_wr_u16(txbuf + 0u, a);
    proto_wr_u16(txbuf + 2u, p);
    proto_wr_u16(txbuf + 4u, ay);
    if (exchange_or_print(sp, HOST_CMD_DEC_BEGIN, txbuf, 6u, &resp) != 0)
    {
        goto cleanup;
    }
    if (resp.len != 2u)
    {
        fprintf(stderr, "Bad DEC_BEGIN response\n");
        goto cleanup;
    }
    device_block_size = proto_rd_u16(resp.payload);
    if (device_block_size > 64u)
    {
        fprintf(stderr, "MCU block size is too large for this host build (%u)\n", (unsigned)device_block_size);
        goto cleanup;
    }

    remaining = plain_size;
    while (remaining > 0u)
    {
        uint16_t chunk_plain = (remaining > device_block_size) ? device_block_size : (uint16_t)remaining;
        size_t need = (size_t)(2u * chunk_plain);

        proto_wr_u16(txbuf + 0u, chunk_plain);
        if (fread(txbuf + 2u, 1u, need, fin) != need)
        {
            fprintf(stderr, "Unexpected end of encrypted file\n");
            goto cleanup_with_end;
        }
        if (exchange_or_print(sp, HOST_CMD_DEC_DATA, txbuf, (uint16_t)(2u + need), &resp) != 0)
        {
            goto cleanup_with_end;
        }
        if ((resp.len != (uint16_t)(2u + chunk_plain)) || (proto_rd_u16(resp.payload) != chunk_plain))
        {
            fprintf(stderr, "Bad DEC_DATA response\n");
            goto cleanup_with_end;
        }
        if (fwrite(resp.payload + 2u, 1u, chunk_plain, fout) != chunk_plain)
        {
            fprintf(stderr, "Write error while saving plaintext\n");
            goto cleanup_with_end;
        }
        remaining -= chunk_plain;
    }

    if (exchange_or_print(sp, HOST_CMD_DEC_END, NULL, 0u, &resp) != 0)
    {
        goto cleanup;
    }

    printf("Decryption completed. a=%u p=%u ay=%u file_block=%u device_block=%u plain_size=%u bytes\n",
           (unsigned)a, (unsigned)p, (unsigned)ay,
           (unsigned)hdr_block_size, (unsigned)device_block_size,
           (unsigned)plain_size);
    rc = 0;
    goto cleanup;

cleanup_with_end:
    (void)exchange_or_print(sp, HOST_CMD_DEC_END, NULL, 0u, &resp);
cleanup:
    if (fin != NULL) fclose(fin);
    if (fout != NULL) fclose(fout);
    return rc;
}

int main(int argc, char **argv)
{
    serial_port_t sp;
    host_packet_t startup_resp;
    uint16_t startup_block_size;
    int rc = 1;

#ifdef _WIN32
    sp.handle = INVALID_HANDLE_VALUE;
#else
    sp.fd = -1;
#endif

    if (argc < 3)
    {
        print_usage(argv[0]);
        return 1;
    }

    if (serial_open(&sp, argv[1], HOST_UART_BAUD) != 0)
    {
        fprintf(stderr, "Cannot open serial port '%s'\n", argv[1]);
        return 1;
    }

    /* Startup sync: read current block size right after launch. */
    if (exchange_or_print(&sp, HOST_CMD_GET_BLOCK_SIZE, NULL, 0u, &startup_resp) == 0 && startup_resp.len == 2u)
    {
        startup_block_size = proto_rd_u16(startup_resp.payload);
        printf("[startup] current MCU block size = %u bytes\n", (unsigned)startup_block_size);
    }
    else
    {
        fprintf(stderr, "Warning: could not read block size at startup\n");
    }

    if (strcmp(argv[2], "ping") == 0)
    {
        rc = cmd_ping(&sp);
    }
    else if ((strcmp(argv[2], "block") == 0) && (argc >= 4))
    {
        if (strcmp(argv[3], "get") == 0)
        {
            rc = cmd_block_get(&sp);
        }
        else if ((strcmp(argv[3], "set") == 0) && (argc >= 5))
        {
            rc = cmd_block_set(&sp, argv[4]);
        }
        else
        {
            print_usage(argv[0]);
        }
    }
    else if ((strcmp(argv[2], "keys") == 0) && (argc >= 4))
    {
        if (strcmp(argv[3], "list") == 0)
        {
            rc = cmd_keys_list(&sp);
        }
        else if ((strcmp(argv[3], "add") == 0) && (argc >= 7))
        {
            rc = cmd_keys_add(&sp, argv[4], argv[5], argv[6]);
        }
        else if ((strcmp(argv[3], "del") == 0) && (argc >= 5))
        {
            rc = cmd_keys_del(&sp, argv[4]);
        }
        else
        {
            print_usage(argv[0]);
        }
    }
    else if ((strcmp(argv[2], "encrypt") == 0) && (argc >= 7))
    {
        rc = cmd_encrypt(&sp, argv[3], argv[4], argv[5], argv[6]);
    }
    else if ((strcmp(argv[2], "decrypt") == 0) && (argc >= 5))
    {
        rc = cmd_decrypt(&sp, argv[3], argv[4]);
    }
    else
    {
        print_usage(argv[0]);
    }


    serial_close(&sp);
    return rc;
}
