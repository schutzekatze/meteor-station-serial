/*
 * comm_protocol.h
 *
 *  Copyright 2017 Vladimir Nikolić
 */

#ifndef COMM_PROTOCOL_H
#define COMM_PROTOCOL_H

#include <stdio.h>

enum Command: uint32_t {
    SHUTTER_OPEN,
    SHUTTER_CLOSE,
    CAMERA_TURN_ON,
    CAMERA_TURN_OFF,
    DHT_INFO_GET
};

#endif /* COMM_PROTOCOL_H */