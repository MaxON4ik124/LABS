#include "serial_port.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32

static void make_windows_port_name(const char *src, char *dst, size_t dst_size)
{
    if ((strncmp(src, "\\\\.\\", 4) == 0) || (strncmp(src, "\\\\?\\", 4) == 0))
    {
        snprintf(dst, dst_size, "%s", src);
    }
    else
    {
        snprintf(dst, dst_size, "\\\\.\\%s", src);
    }
}

int serial_open(serial_port_t *sp, const char *port_name, uint32_t baudrate)
{
    char full_name[64];
    DCB dcb;
    COMMTIMEOUTS timeouts;

    make_windows_port_name(port_name, full_name, sizeof(full_name));
    sp->handle = CreateFileA(full_name,
                             GENERIC_READ | GENERIC_WRITE,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL);
    if (sp->handle == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(sp->handle, &dcb))
    {
        CloseHandle(sp->handle);
        sp->handle = INVALID_HANDLE_VALUE;
        return -2;
    }

    dcb.BaudRate = baudrate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;

    if (!SetCommState(sp->handle, &dcb))
    {
        CloseHandle(sp->handle);
        sp->handle = INVALID_HANDLE_VALUE;
        return -3;
    }

    memset(&timeouts, 0, sizeof(timeouts));
    timeouts.ReadIntervalTimeout = 20;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(sp->handle, &timeouts);

    PurgeComm(sp->handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
    return 0;
}

void serial_close(serial_port_t *sp)
{
    if ((sp != NULL) && (sp->handle != INVALID_HANDLE_VALUE))
    {
        CloseHandle(sp->handle);
        sp->handle = INVALID_HANDLE_VALUE;
    }
}

int serial_write_all(serial_port_t *sp, const uint8_t *data, size_t size)
{
    size_t done = 0;
    while (done < size)
    {
        DWORD written = 0;
        if (!WriteFile(sp->handle, data + done, (DWORD)(size - done), &written, NULL))
        {
            return -1;
        }
        done += (size_t)written;
    }
    return 0;
}

int serial_read_exact(serial_port_t *sp, uint8_t *data, size_t size, uint32_t timeout_ms)
{
    DWORD start = GetTickCount();
    size_t done = 0;

    while (done < size)
    {
        DWORD got = 0;
        if (!ReadFile(sp->handle, data + done, (DWORD)(size - done), &got, NULL))
        {
            return -1;
        }
        if (got > 0)
        {
            done += (size_t)got;
            continue;
        }
        if ((GetTickCount() - start) > timeout_ms)
        {
            return -2;
        }
    }
    return 0;
}

#else

#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <termios.h>

static speed_t to_posix_baud(uint32_t baudrate)
{
    switch (baudrate)
    {
        case 9600u:   return B9600;
        case 19200u:  return B19200;
        case 38400u:  return B38400;
        case 57600u:  return B57600;
        case 115200u: return B115200;
        default:      return B38400;
    }
}

int serial_open(serial_port_t *sp, const char *port_name, uint32_t baudrate)
{
    struct termios tio;

    sp->fd = open(port_name, O_RDWR | O_NOCTTY | O_SYNC);
    if (sp->fd < 0)
    {
        return -1;
    }

    if (tcgetattr(sp->fd, &tio) != 0)
    {
        close(sp->fd);
        sp->fd = -1;
        return -2;
    }

    tio.c_iflag &= (tcflag_t)~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tio.c_oflag &= (tcflag_t)~OPOST;
    tio.c_lflag &= (tcflag_t)~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tio.c_cflag &= (tcflag_t)~(CSIZE | PARENB);
    tio.c_cflag |= CS8;

    cfsetispeed(&tio, to_posix_baud(baudrate));
    cfsetospeed(&tio, to_posix_baud(baudrate));
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;

    if (tcsetattr(sp->fd, TCSANOW, &tio) != 0)
    {
        close(sp->fd);
        sp->fd = -1;
        return -3;
    }

    tcflush(sp->fd, TCIOFLUSH);
    return 0;
}

void serial_close(serial_port_t *sp)
{
    if ((sp != NULL) && (sp->fd >= 0))
    {
        close(sp->fd);
        sp->fd = -1;
    }
}

int serial_write_all(serial_port_t *sp, const uint8_t *data, size_t size)
{
    size_t done = 0;
    while (done < size)
    {
        ssize_t wr = write(sp->fd, data + done, size - done);
        if (wr < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        done += (size_t)wr;
    }
    return 0;
}

int serial_read_exact(serial_port_t *sp, uint8_t *data, size_t size, uint32_t timeout_ms)
{
    size_t done = 0;

    while (done < size)
    {
        fd_set set;
        struct timeval tv;
        int rc;
        ssize_t rd;

        FD_ZERO(&set);
        FD_SET(sp->fd, &set);
        tv.tv_sec = (time_t)(timeout_ms / 1000u);
        tv.tv_usec = (suseconds_t)((timeout_ms % 1000u) * 1000u);

        rc = select(sp->fd + 1, &set, NULL, NULL, &tv);
        if (rc < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        if (rc == 0)
        {
            return -2;
        }

        rd = read(sp->fd, data + done, size - done);
        if (rd < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -3;
        }
        if (rd == 0)
        {
            return -4;
        }
        done += (size_t)rd;
    }

    return 0;
}

#endif
