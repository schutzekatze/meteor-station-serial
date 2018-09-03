/*
 * linux.c
 *
 * Copyright 2018 Vladimir Nikolić
 */

#ifdef __linux__

#include "serial_prototypes.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>

#define TTY_APPEND_UP_TO 256
#define TTY_MAX_COUNT 256
#define TTY_PATH_BUFFER_SIZE 256
#define TIMEOUT_WINDOW 20 // In tenths of a second
#define DEVICE_FLUSH_DELAY 2 // In seconds
#define DEVICE_RECOGNITION_DELAY 4 // In seconds

static const char* TTY_PATHS[] = { "/dev/ttyACM", "/dev/ttyUSB" }; // Append the port number to get full path
static int ttys[TTY_MAX_COUNT]; // Lol titties

static int baud2constant(const int baud)
{
    switch (baud)
    {
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        default: return -1;
    }
}

// Return tty number or -1 on failure.
static int port_open(char* port_path) {
    int tty = open(port_path, O_RDWR | O_NOCTTY);
    if (tty == -1) {
        return -1;
    } else {
        fcntl(tty, F_SETFL, 0);
    }

    struct termios options;
    tcgetattr(tty, &options);

    // Set baud rate
    cfsetispeed(&options, baud2constant(SERIAL_BAUD_RATE));
    cfsetospeed(&options, baud2constant(SERIAL_BAUD_RATE));

    // Necessary flags
    options.c_cflag |= (CLOCAL | CREAD);

    // Set character size and disable parity
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Enable the following line if parity is turned on
    // options.c_iflag |= (INPCK | ISTRIP); //Enable parity check and strip

    // Disable hardware and software flow control respectively
    options.c_cflag &= ~CRTSCTS;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Enable raw input

    options.c_oflag &= ~OPOST; // Enable raw output

    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = TIMEOUT_WINDOW;

    tcsetattr(tty, TCSANOW, &options);

    return tty;
}

int ports_init(unsigned *port_count) {
    char port_path[TTY_PATH_BUFFER_SIZE];
    int i, j, ports_found = 0, ports_recognized = 0;

    for (i = 0; i < sizeof(TTY_PATHS) / sizeof(TTY_PATHS[0]); i++) {
        for (j = 0; j < TTY_APPEND_UP_TO; j++) {
            strcpy(port_path, TTY_PATHS[i]);
            sprintf(port_path + strlen(TTY_PATHS[i]), "%d", j);

            if (access(port_path, F_OK) != -1) {
                int tty = port_open(port_path);
                if (tty >= 0) {
                    ttys[ports_found++] = tty;
                }
            }
        }
    }

	sleep(DEVICE_FLUSH_DELAY);
    for (i = 0; i < ports_found; i++) {
        tcflush(ttys[i], TCIOFLUSH);
    }
    sleep(DEVICE_RECOGNITION_DELAY);

    for(i = 0; i < ports_found; i++) {
        int fail = 0, bytes_avail;

        ioctl(ttys[i], FIONREAD, &bytes_avail);
        if (bytes_avail >= sizeof(RECOGNIZE_TOKENS)) {
            uint8_t tokens[sizeof(RECOGNIZE_TOKENS)];
            FILE *file = fdopen(ttys[i], "r+");
            for (j = 0; j < sizeof(RECOGNIZE_TOKENS); j++) {
                tokens[j] = getc(file);
                if (tokens[j] != RECOGNIZE_TOKENS[j]) {
                    for (; j >= 0; j--) {
                        ungetc(tokens[j], file);
                    }
                    fail = 1;
                    break;
                }
            }
        } else {
            fail = 1;
        }

        if (fail) {
            close(ttys[i]);
        } else {
            ttys[ports_recognized++] = ttys[i];
        }
    }
    *port_count = ports_recognized;

	return 0;
}

int ports_end(unsigned *port_count) {
    while (*port_count > 0) {
        if (close(ttys[(*port_count) - 1]) < 0) {
            return -1;
        }
        *port_count--;
    }
    return 0;
}

int bytes_write(unsigned port, const uint8_t *buffer, const unsigned n) {
    return write(ttys[port], buffer, n);
}

int bytes_read(unsigned port, uint8_t *buffer, const unsigned n) {
    return read(ttys[port], buffer, n);
}

#endif /* __linux__ */
