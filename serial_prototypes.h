/*
 * serial_prototypes.h
 *
 * Copyleft 2018 Vladimir Nikolić
 */

#ifndef SERIAL_PROTOTYPES_H
#define SERIAL_PROTOTYPES_H

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
 * the ports_init, ports_end, bytes_write, bytes_read
 * specific to their platform.
 */

#include <stdint.h>

extern const unsigned SERIAL_BAUD_RATE;
extern const uint8_t RECOGNIZE_TOKENS[3];

int ports_init(unsigned *port_count);
int ports_end(unsigned *port_count);

// These return the number of written/read bytes, or negative on error
int bytes_write(unsigned port, const uint8_t *buffer, const unsigned n);
int bytes_read(unsigned port, uint8_t *buffer, const unsigned n);

#endif /* SERIAL_PROTOTYPES_H */
