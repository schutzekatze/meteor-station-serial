/*
 * serial_com.c
 *
 * Copyright 2018 Vladimir Nikolić
 */

#include "serial_com.h"
#include "serial_prototypes.h"

#include <stdint.h>
#include <math.h>

static const uint8_t ACKNOWLEDGE = 255;
static const uint8_t NEGATIVE_ACKNOWLEDGE = 0;
static const unsigned ATTEMPTS_BEFORE_ABORT = 3;

unsigned serial_port_count = 0;

static uint32_t htonl(const uint32_t n)
{
    uint32_t r;
    unsigned char *rp = (unsigned char *)&r;

    rp[0] = (n & 0xff000000) >> 24;
    rp[1] = (n & 0x00ff0000) >> 16;
    rp[2] = (n & 0x0000ff00) >>  8;
    rp[3] = (n & 0x000000ff)      ;

    return r;
}

static uint32_t ntohl(const uint32_t n)
{
    unsigned char *np = (unsigned char *)&n;

    return
        ((uint32_t)np[0] << 24) |
        ((uint32_t)np[1] << 16) |
        ((uint32_t)np[2] <<  8) |
        ((uint32_t)np[3]      );
}

int serial_init() {
    if (serial_port_count == 0) {
        return ports_init(&serial_port_count);
    }
    return 0;
}

int serial_end() {
    if (serial_port_count > 0) {
        return ports_end(&serial_port_count);
    }
    return 0;
}

int serial_send(unsigned port, const uint32_t msg) {
    uint32_t network_msg = htonl(msg);
    uint8_t checksum, response;
	unsigned result, bytes_sent;
	unsigned i;

	checksum = 0;
    for (i = 0; i < sizeof(network_msg); i++) {
        checksum += (network_msg >> i * 8) & 0x000000FFUL;
    }

    for (i = 0; i < ATTEMPTS_BEFORE_ABORT; i++) {
        bytes_sent = 0;
        while (bytes_sent < sizeof(network_msg)) {
			result = bytes_write(
                port,
                (const uint8_t*)&network_msg + bytes_sent,
                sizeof(network_msg) - bytes_sent
            );
            if (result <= 0) return -1;

            bytes_sent += result;
        }

        if (bytes_write(port, &checksum, sizeof(checksum)) <= 0) return -1;
        if (bytes_read (port, &response, sizeof(response)) <= 0) return -1;

        if (fabs((int)response - ACKNOWLEDGE) < fabs((int)response - NEGATIVE_ACKNOWLEDGE)) {
            return 0;
        }
    }

	return -1;
}

int serial_receive(unsigned port, uint32_t *msg) {
    uint32_t network_msg;
    uint8_t checksum, calculated_checksum;
    unsigned result, bytes_received;
    unsigned i, j;

    for(i = 0; i < ATTEMPTS_BEFORE_ABORT; i++) {
        bytes_received = 0;
        while (bytes_received < sizeof(network_msg)) {
			result = bytes_read(
                port,
                (uint8_t*)&network_msg + bytes_received,
                sizeof(network_msg) - bytes_received
            );
            if (result <= 0) return -1;

            bytes_received += result;
        }

        if (bytes_read(port, &checksum, sizeof(checksum)) <= 0) return -1;

        calculated_checksum = 0;
        for (j = 0; j < sizeof(network_msg); j++) {
            calculated_checksum += (network_msg >> j * 8) & 0x000000FFUL;
        }

        if (calculated_checksum == checksum) {
            if (bytes_write(port, (const uint8_t*)&ACKNOWLEDGE, sizeof(ACKNOWLEDGE)) <= 0) return -1;
            *msg = ntohl(network_msg);
            return 0;
        } else {
            if (bytes_write(port, (const uint8_t*)&NEGATIVE_ACKNOWLEDGE, sizeof(NEGATIVE_ACKNOWLEDGE)) <= 0) return -1;
        }
    }

	return -1;
}
