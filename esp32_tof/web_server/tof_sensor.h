#ifndef TOF_SENSOR_H
#define TOF_SENSOR_H


#include <Arduino.h>


// ============================================================
// VL53L5CX CONFIGURATION
// ============================================================

#define TOF_ROWS       8
#define TOF_COLS       8
#define TOF_NUM_ZONES  64


// ============================================================
// TOF SENSOR INITIALIZATION
// ============================================================

bool tof_init();


// ============================================================
// TOF SENSOR UPDATE
// ============================================================
//
// Reads a new ranging frame when one is available.
//
// This function does NOT send anything through Serial.
//
// The latest measurements are stored internally and can be
// accessed using the functions below.
//

void tof_update();


// ============================================================
// ACCESSORS
// ============================================================


// Get distance of a particular zone.
//
// row : 0 ... 7
// col : 0 ... 7
//
// Returns distance in millimeters.

uint16_t tof_get_distance(
    uint8_t row,
    uint8_t col
);


// Get target status of a particular zone.
//
// 5 = Range valid
//
// Other values indicate an invalid/non-ranging target.

uint8_t tof_get_status(
    uint8_t row,
    uint8_t col
);


// Get the current frame sequence number.

uint16_t tof_get_sequence();


// ============================================================
// BULK DATA ACCESS
// ============================================================


// Copy the complete 8×8 distance map.

void tof_get_distances(
    uint16_t output[TOF_ROWS][TOF_COLS]
);


// Copy the complete 8×8 status map.

void tof_get_statuses(
    uint8_t output[TOF_ROWS][TOF_COLS]
);


// ============================================================
// FRAME STATUS
// ============================================================


// Returns true when at least one valid ToF frame has been
// acquired.

bool tof_has_frame();


// Returns true when a new ToF frame has been acquired since
// the previous call.
//
// Calling this function clears the new-frame flag.

bool tof_new_frame();


#endif
