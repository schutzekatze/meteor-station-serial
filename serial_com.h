/*
 * serial_com.h
 *
 * Copyleft 2018 Vladimir Nikolić
 */

#ifndef SERIAL_COM_H
#define SERIAL_COM_H

/*
 * Sends/receives given messages and returns 0 on success,
 * or non-zero on failure.
 */

int serial_init();
int serial_end();
int serial_send(const uint32_t msg);
int serial_receive(uint32_t *msg);

#endif /* SERIAL_COM_H */