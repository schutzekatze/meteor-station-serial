/*
 * serial_prototypes.h
 *
 * Copyright 2018 Vladimir Nikolić
 */

#ifndef SERIAL_PROTOTYPES_H
#define SERIAL_PROTOTYPES_H

/*
 * This header declares routines that need platform specific implementations
 * required by serial_com.
 *
 * The mode used is 8N1, 8 bit character size, no parity and 1 stop bit.
 */

#include <stdint.h>

extern const unsigned SERIAL_BAUD_RATE;
// Values with which microcontroller is recognized on a serial port.
extern const uint8_t RECOGNIZE_TOKENS[3];

// These return 0 on success and negative on failure.
int ports_init(unsigned *port_count);
int ports_end(unsigned *port_count);

// These return the number of written/read bytes on success and negative on failure.
int bytes_write(unsigned port, const uint8_t *buffer, const unsigned n);
int bytes_read(unsigned port, uint8_t *buffer, const unsigned n);

#endif /* SERIAL_PROTOTYPES_H */
