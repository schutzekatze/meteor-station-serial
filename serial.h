/*
 * serial.h
 *
 * Copyleft 2018 Vladimir Nikolić
 */

#ifndef SERIAL_H
#define SERIAL_H

/*
 * This header declares routines for serial communication between
 * any two endpoints.
 *
 * The mode used is 8N1, 8 bit character size, no parity and 1 stop bit.
 * All messages are fixed size of 4 bytes in network byte order, i.e. big endian.
 * Every message is followed by a checksum byte, a simple sum of message bytes.
 * If on receive the checksum is correct, an ACKNOWLEDGE byte (255) is sent back.
 * If the checksum is incorrect, a NEGATIVE_ACKNOWLEDGE byte (0) is sent back.
 * If an error occurs with an acknowledge byte, assume values greater than 127
 * to be ACKNOWLEDGE, and less than or equal to 127 to be NEGATIVE_ACKNOWLEDGE.
 * On negative acknowledge, the message is resent up to ATTEMPTS_BEFORE_ABORT (3) times.
 * If all attempts fail, the communication should abort and throw an error.
 *
 * Both sides of the communication should include this header, and implement
 * the init, end, bytes_write, bytes_read, endianness_host2net and endianness_net2host
 * specific to their platform. You can then use serial_send and
 * serial_receive functions that perform the communication.
 */

#include <stdint.h>

extern const unsigned BAUD_RATE;

// These functions require platform specific implementations to use serial communication.
int port_init();
int port_end();

unsigned bytes_write(const uint8_t *buffer, const unsigned n);
unsigned bytes_read(uint8_t *buffer, const unsigned n);

uint32_t endianness_net2host(const uint32_t msg);
uint32_t endianness_host2net(const uint32_t msg);

#endif /* SERIAL_H */
