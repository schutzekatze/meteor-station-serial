/*
 * avr.cpp
 *
 * Copyleft 2018 Vladimir Nikolić
 */

#ifdef __AVR

#include <HardwareSerial.h>
extern "C" {
#include "serial_prototypes.h"
}

extern HardwareSerial Serial;

int ports_init(unsigned *port_count) {
    Serial.begin(BAUD_RATE);
    *port_count = 1;
	return 0;
}

int ports_end(unsigned *port_count) {
    *port_count = 0;
    return 0;
}

unsigned bytes_write(unsigned port, const uint8_t *buffer, const unsigned n) {
    return Serial.write(buffer, n);
}

unsigned bytes_read(unsigned port, uint8_t *buffer, const unsigned n) {
    unsigned bytes = 0;
    while (Serial.available() > 0 && bytes < n) {
        *(buffer + bytes) = Serial.read();
        bytes++;
    }
    return bytes;
}

#endif /* __AVR */
