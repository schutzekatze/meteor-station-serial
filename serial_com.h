/*
 * serial_com.h
 *
 * Copyright 2018 Vladimir Nikolić
 */

#ifndef SERIAL_COM_H
#define SERIAL_COM_H

#include <stdint.h>

/*
 *   Include this header to use serial communication.
 *   All routines return 0 on success, and negative on failure.
 */

extern unsigned serial_port_count;

int serial_init();
int serial_end();
int serial_send(unsigned port, const uint32_t msg);
int serial_receive(unsigned port, uint32_t *msg);

#endif /* SERIAL_COM_H */
