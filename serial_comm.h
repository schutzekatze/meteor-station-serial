/*
 * serial_comm.h
 *
 *  Copyright 2017 Vladimir Nikolić
 */

#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

/*
 * This header declares routines for serial communication between
 * a computer and a microcontroller.
 *
 * The mode used is 8N1, 8 bit character size, no parity and 1 stop bit.
 * All messages are fixed size of 4 bytes in network byte order, i.e. big endian.
 * Every message is followed by a checksum byte, a simple sum of message bytes.
 * If on receive the checksum is correct, an ACKNOWLEDGE byte is sent back.
 * If the checksum is incorrect, a NEGATIVE_ACKNOWLEDGE byte is sent back.
 * If an error occurs with an acknowledge byte, assume values greater than 127
 * to be ACKNOWLEDGE, and less than or equal to 127 to be NEGATIVE_ACKNOWLEDGE.
 * On negative acknowledge, the message is resent up to ATTEMPTS_BEFORE_ABORT times.
 * If all attempts fail, the communication should abort and throw an error.
 *
 * Both sides of the communication should include this header, and implement
 * the bytes_write, bytes_read, endianess_host2net and endianess_net2host
 * specific to their platform. You can then use serial_comm_send and
 * serial_comm_receive functions that perform the communication.
 *
 * serial_comm_send/receive return 0 on successful communication and non-zero
 * on error;
 */

#include <stdint.h>

const unsigned BAUD_RATE = 9600;

const uint8_t ACKNOWLEDGE = 255;
const uint8_t NEGATIVE_ACKNOWLEDGE = 0;

const unsigned ATTEMPTS_BEFORE_ABORT = 3;

// These functions require platform specific implementations to use serial communication.
unsigned bytes_write(const uint8_t *buffer, const unsigned n);
unsigned bytes_read(uint8_t *buffer, const unsigned n);

uint32_t endianess_net2host(const uint32_t msg);
uint32_t endianess_host2net(const uint32_t msg);

int serial_comm_send(const uint32_t msg);
int serial_comm_receive(uint32_t *msg);

#endif /* SERIAL_COMM_H */