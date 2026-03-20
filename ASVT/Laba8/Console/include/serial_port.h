#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
typedef struct
{
    HANDLE handle;
} serial_port_t;
#else
#include <unistd.h>
typedef struct
{
    int fd;
} serial_port_t;
#endif

int serial_open(serial_port_t *sp, const char *port_name, uint32_t baudrate);
void serial_close(serial_port_t *sp);
int serial_write_all(serial_port_t *sp, const uint8_t *data, size_t size);
int serial_read_exact(serial_port_t *sp, uint8_t *data, size_t size, uint32_t timeout_ms);

#endif /* SERIAL_PORT_H */
