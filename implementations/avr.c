/*
 * avr.c
 *
 * Copyleft 2018 Vladimir Nikolić
 */

#ifdef __AVR

#include "../serial.h"

int init() {
    Serial.begin(BAUD_RATE);
	return 0;
}

int end() {
    return 0;
}

unsigned bytes_write(const uint8_t *buffer, const unsigned n) {
    return Serial.write(buffer, n);
}

unsigned bytes_read(uint8_t *buffer, const unsigned n) {
    unsigned bytes = 0;
    while (Serial.available() > 0 && bytes < n) {
        *(buffer + bytes) = Serial.read();
        bytes++;
    }
    return bytes;
}

#define htons(x) ( ((x)<< 8 & 0xFF00) | \
                   ((x)>> 8 & 0x00FF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

uint32_t endianness_host2net(const uint32_t msg) {
    return htonl(msg);
}

uint32_t endianness_net2host(const uint32_t msg) {
    return ntohl(msg);
}

#endif /* __AVR */
