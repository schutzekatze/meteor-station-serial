/*
 * windows.c
 *
 * Copyright 2018 Vladimir Nikolić
 */

#ifdef _WIN32

#include "serial_prototypes.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define COM_APPEND_UP_TO 256
#define COM_MAX_COUNT 256
#define COM_PATH_BUFFER_SIZE 256
#define TIMEOUT_WINDOW 2000 // In milliseconds
#define DEVICE_FLUSH_DELAY 2 // In seconds
#define DEVICE_RECOGNITION_DELAY 4 // In seconds

static const char* COM_PATH = "\\\\.\\COM"; // Append the port number to get full path
static HANDLE coms[COM_MAX_COUNT];

static int baud2constant(const int baud)
{
    switch (baud)
    {
        case 4800: return CBR_4800;
        case 9600: return CBR_9600;
        case 19200: return CBR_19200;
        case 38400: return CBR_38400;
        case 57600: return CBR_57600;
        default: return -1;
    }
}

static int port_open(char* port_path, HANDLE *com) {
    SetLastError(0);

    HANDLE handle = CreateFile(port_path,
    GENERIC_READ | GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL);

    if (GetLastError() != ERROR_FILE_NOT_FOUND) return -1;
    if (handle == INVALID_HANDLE_VALUE) return -1;

	DCB dcb_serial_params = {0};
	if (!GetCommState(handle, &dcb_serial_params)) return -1;

    dcb_serial_params.BaudRate = baud2constant(SERIAL_BAUD_RATE);
    dcb_serial_params.ByteSize = 8;
    dcb_serial_params.StopBits = ONESTOPBIT;
    dcb_serial_params.Parity = NOPARITY;

    dcb_serial_params.fDtrControl = DTR_CONTROL_ENABLE;

    if (!SetCommState(handle, &dcb_serial_params)) return -1;
    COMMTIMEOUTS timeouts = { 0, 0, TIMEOUT_WINDOW, 0, TIMEOUT_WINDOW };
    if (!SetCommTimeouts(handle, &timeouts)) return -1;

    *com = handle;

	return 0;
}

int ports_init(unsigned *port_count) {
    char port_path[COM_PATH_BUFFER_SIZE];
    int i, j, ports_found = 0, ports_recognized = 0;

    for (i = 0; i < TTY_APPEND_UP_TO; i++) {
        strcpy(port_path, COM_PATH);
        sprintf(port_path + strlen(COM_PATH), "%d", i);

        HANDLE com;
        if (port_open(port_path, &com) == 0) {
            coms[ports_found++] = com;
        }
    }

    Sleep(DEVICE_FLUSH_DELAY * 1000);
    for (i = 0; i < ports_found; i++) {
        PurgeComm(coms[i], PURGE_RXCLEAR | PURGE_TXCLEAR);
    }
    Sleep(DEVICE_RECOGNITION_DELAY * 1000);

    for(i = 0; i < ports_found; i++) {
        int fail = 0, bytes_avail;

        ioctl(ttys[i], FIONREAD, &bytes_avail);
        if (bytes_avail >= sizeof(RECOGNIZE_TOKENS)) {
            uint8_t tokens[sizeof(RECOGNIZE_TOKENS)];
            FILE *file = fdopen(ttys[i], "r+");
            for (j = 0; j < sizeof(RECOGNIZE_TOKENS); j++) {
                tokens[j] = getc(file);
                if (tokens[j] != RECOGNIZE_TOKENS[j]) {
                    for (; j >= 0; j--) {
                        ungetc(tokens[j], file);
                    }
                    fail = 1;
                    break;
                }
            }
        } else {
            fail = 1;
        }

        if (fail) {
            CloseHandle(coms[i]);
        } else {
            coms[ports_recognized++] = coms[i];
        }
    }
    *port_count = ports_recognized;

    return 0;
}

int ports_end(unsigned *port_count) {
    while (*port_count > 0) {
        if (CloseHandle(coms[(*port_count) - 1]) == 0) {
            return -1;
        }
        *port_count--;
    }
    return 0;
}

int bytes_write(unsigned port, const uint8_t *buffer, const unsigned n) {
    DWORD bytes_written = 0;
    if (WriteFile(coms[port], buffer, n, &bytes_written, NULL) == 0) {
        return -1;
    }
	return bytes_written;
}

int bytes_read(unsigned port, uint8_t *buffer, const unsigned n) {
    DWORD bytes_read = 0;

	COMSTAT status;
	DWORD errors;
    ClearCommError(coms[port], &errors, &status);

    if (status.cbInQue > 0 &&
    ReadFile(coms[port], buffer, status.cbInQue < n ? status.cbInQue : n, &bytes_read, NULL) == 0) {
        return -1;
    }

    return bytes_read;
}

#endif /* _WIN32 */
