/*
 * serial_com.c
 *
 * Copyleft 2018 Vladimir Nikolić
 */

#include "serial.h"
#include "serial_com.h"

#include <math.h>

int serial_init() {
    return init();	
}

int serial_end() {
    return end();	
}

int serial_comm_send(const uint32_t msg) {
    uint32_t network_msg = endiannness_host2net(msg);
    uint8_t checksum, response;
	unsigned bytes_sent;
	unsigned i;
	
	checksum = 0;
    for (i = 0; i < sizeof(network_msg); i++) {
        checksum += (network_msg >> i * 8) & 0x000000FFUL;
    }

    for (i = 0; i < ATTEMPTS_BEFORE_ABORT; i++) {
        bytes_sent = 0;
        while (bytes_sent < sizeof(network_msg)) {
			bytes_sent += bytes_write((const uint8_t*)&network_msg + bytes_sent, sizeof(network_msg) - bytes_sent);
        }

		while (bytes_write(&checksum, sizeof(checksum)) == 0);

        while (bytes_read(&response, sizeof(response)) == 0);

        if (fabs((int)response - ACKNOWLEDGE) < fabs((int)response - NEGATIVE_ACKNOWLEDGE)) {
            return 0;
        }
    }
	
	return -1;
}

int serial_comm_receive(uint32_t *msg) {
    uint32_t network_msg;
    uint8_t checksum, calculated_checksum;
    unsigned bytes_received;
    unsigned i, j;
	
    for(i = 0; i < ATTEMPTS_BEFORE_ABORT; i++) {
        bytes_received = 0;
        while (bytes_received < sizeof(network_msg)) {
			bytes_received += bytes_read((uint8_t*)&network_msg + bytes_received, sizeof(network_msg) - bytes_received);
        }
		
		while (bytes_read(&checksum, sizeof(checksum)) == 0);

        calculated_checksum = 0;
        for (j = 0; j < sizeof(network_msg); j++) {
            calculated_checksum += (network_msg >> j * 8) & 0x000000FFUL;
        }
		
        if (calculated_checksum == checksum) {
			while (bytes_write((const uint8_t*)&ACKNOWLEDGE, sizeof(ACKNOWLEDGE)) == 0);
            break;
        } else {
			while (bytes_write((const uint8_t*)&NEGATIVE_ACKNOWLEDGE, sizeof(NEGATIVE_ACKNOWLEDGE)) == 0);
        }
    }
	
	if (i == ATTEMPTS_BEFORE_ABORT) {
		return -1;
	}

    *msg = endianness_net2host(network_msg);
	return 0;
}