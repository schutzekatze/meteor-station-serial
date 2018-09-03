/*
 * avr.cpp
 *
 * Copyright 2018 Vladimir Nikolić
 */

#ifdef __AVR

#include <HardwareSerial.h>
#include <Arduino.h>
extern "C" {
#include "serial_prototypes.h"
}

#define DELAY_BEFORE_RECOGNITION 4

extern HardwareSerial Serial;

int ports_init(unsigned *port_count) {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.flush();
    delay(DELAY_BEFORE_RECOGNITION * 1000);
    while (Serial.available() > 0) Serial.read();
    Serial.write(RECOGNIZE_TOKENS, sizeof(RECOGNIZE_TOKENS));
    *port_count = 1;
    return 0;
}

int ports_end(unsigned *port_count) {
    *port_count = 0;
    return 0;
}

int bytes_write(unsigned port, const uint8_t *buffer, const unsigned n) {
    return Serial.write(buffer, n);
}

int bytes_read(unsigned port, uint8_t *buffer, const unsigned n) {
    unsigned bytes = 0;
    while (bytes < n) {
        while (Serial.available() == 0);
        *(buffer + bytes) = Serial.read();
        bytes++;
    }
    return bytes;
}

#endif /* __AVR */
