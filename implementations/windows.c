/*
 * windows.c
 *
 * Copyleft 2018 Vladimir Nikolić
 */

#ifdef _WIN32

#include "../serial.h"

#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

const int INIT_WAIT_SECONDS = 2;

HANDLE serial_handle;

int port_init() {
	serial_handle = CreateFile("COM7",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

	if (serial_handle == INVALID_HANDLE_VALUE) {
	    return -1;
	}

	DCB dcb_serial_params = {0};

	if (!GetCommState(serial_handle, &dcb_serial_params)) {
	    return -1;
	}

    dcb_serial_params.BaudRate = BAUD_RATE; //CBR_9600;
    dcb_serial_params.ByteSize = 8;
    dcb_serial_params.StopBits = ONESTOPBIT;
    dcb_serial_params.Parity = NOPARITY;

    dcb_serial_params.fDtrControl = DTR_CONTROL_ENABLE;

    if(!SetCommState(serial_handle, &dcb_serial_params)) {
        return -1;
    }

	PurgeComm(serial_handle, PURGE_RXCLEAR | PURGE_TXCLEAR);

    Sleep(INIT_WAIT_SECONDS * 1000);

	return 0;
}

int port_end() {
    if (CloseHandle(serial_handle)) {
        return 0;
    }
    return -1;
}

unsigned bytes_write(const uint8_t *buffer, const unsigned n) {
    DWORD bytes_written = 0;
    WriteFile(serial_handle, buffer, n, &bytes_written, NULL);
	return bytes_written;
}

unsigned bytes_read(uint8_t *buffer, const unsigned n) {
    DWORD bytes_read = 0;
    unsigned to_read;

	COMSTAT status;
	DWORD errors;
    ClearCommError(serial_handle, &errors, &status);

    if (status.cbInQue > 0) {
		to_read = n;
		if (status.cbInQue < n) {
		    to_read = status.cbInQue;
		}

        ReadFile(serial_handle, buffer, to_read, &bytes_read, NULL);
    }

    return bytes_read;
}

uint32_t endianness_host2net(const uint32_t msg) {
    return htonl(msg);
}

uint32_t endianness_net2host(const uint32_t msg) {
    return ntohl(msg);
}

#endif /* _WIN32 */
