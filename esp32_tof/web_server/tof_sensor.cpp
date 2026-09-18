#include "tof_sensor.h"

#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>


// ============================================================
// VL53L5CX OBJECT
// ============================================================

SparkFun_VL53L5CX myImager;


// ============================================================
// RANGING DATA
// ============================================================

VL53L5CX_ResultsData measurementData;


// ============================================================
// LOCAL TOF STORAGE
// ============================================================

static uint16_t distance_map[TOF_ROWS][TOF_COLS];

static uint8_t status_map[TOF_ROWS][TOF_COLS];

static uint16_t frame_sequence = 0;


// ============================================================
// FRAME FLAGS
// ============================================================

static bool frame_available = false;

static bool new_frame_flag = false;


// ============================================================
// INITIALIZE VL53L5CX
// ============================================================

bool tof_init()
{
    // --------------------------------------------------------
    // Initialize I2C
    // --------------------------------------------------------

    Wire.begin(
        21,
        22
    );

    Wire.setClock(400000);


    // --------------------------------------------------------
    // Initialize VL53L5CX
    // --------------------------------------------------------

    if (
        myImager.begin(
            0x29,
            Wire
        ) == false
    )
    {
        return false;
    }


    // --------------------------------------------------------
    // Configure resolution
    // --------------------------------------------------------

    if (
        myImager.setResolution(
            8 * 8
        ) == false
    )
    {
        return false;
    }


    // --------------------------------------------------------
    // Configure ranging frequency
    // --------------------------------------------------------

    if (
        myImager.setRangingFrequency(
            15
        ) == false
    )
    {
        return false;
    }


    // --------------------------------------------------------
    // Start ranging
    // --------------------------------------------------------

    if (
        myImager.startRanging()
        == false
    )
    {
        return false;
    }


    // --------------------------------------------------------
    // Clear local data
    // --------------------------------------------------------

    for (
        uint8_t row = 0;
        row < TOF_ROWS;
        row++
    )
    {
        for (
            uint8_t col = 0;
            col < TOF_COLS;
            col++
        )
        {
            distance_map[row][col] = 0;

            status_map[row][col] = 0;
        }
    }


    frame_sequence = 0;

    frame_available = false;

    new_frame_flag = false;


    return true;
}


// ============================================================
// UPDATE TOF SENSOR
// ============================================================

void tof_update()
{
    // --------------------------------------------------------
    // Check whether a new ranging measurement is available
    // --------------------------------------------------------

    if (
        myImager.isDataReady()
        == false
    )
    {
        return;
    }


    // --------------------------------------------------------
    // Read ranging data
    // --------------------------------------------------------

    if (
        myImager.getRangingData(
            &measurementData
        ) == false
    )
    {
        return;
    }


    // --------------------------------------------------------
    // Copy the 64 zones into our local 8×8 maps
    // --------------------------------------------------------

    for (
        uint8_t row = 0;
        row < TOF_ROWS;
        row++
    )
    {
        for (
            uint8_t col = 0;
            col < TOF_COLS;
            col++
        )
        {
            uint8_t index =
                row * TOF_COLS + col;


            // ------------------------------------------------
            // Distance in millimeters
            // ------------------------------------------------

            distance_map[row][col] =
                measurementData.distance_mm[
                    index
                ];


            // ------------------------------------------------
            // Target status
            // ------------------------------------------------

            status_map[row][col] =
                measurementData.target_status[
                    index
                ];
        }
    }


    // --------------------------------------------------------
    // Increment frame sequence
    // --------------------------------------------------------

    frame_sequence++;

    frame_available = true;

    new_frame_flag = true;
}


// ============================================================
// GET SINGLE DISTANCE
// ============================================================

uint16_t tof_get_distance(
    uint8_t row,
    uint8_t col
)
{
    if (
        row >= TOF_ROWS
        ||
        col >= TOF_COLS
    )
    {
        return 0;
    }


    return distance_map[row][col];
}


// ============================================================
// GET SINGLE STATUS
// ============================================================

uint8_t tof_get_status(
    uint8_t row,
    uint8_t col
)
{
    if (
        row >= TOF_ROWS
        ||
        col >= TOF_COLS
    )
    {
        return 0;
    }


    return status_map[row][col];
}


// ============================================================
// GET FRAME SEQUENCE
// ============================================================

uint16_t tof_get_sequence()
{
    return frame_sequence;
}


// ============================================================
// GET COMPLETE DISTANCE MAP
// ============================================================

void tof_get_distances(
    uint16_t output[TOF_ROWS][TOF_COLS]
)
{
    for (
        uint8_t row = 0;
        row < TOF_ROWS;
        row++
    )
    {
        for (
            uint8_t col = 0;
            col < TOF_COLS;
            col++
        )
        {
            output[row][col] =
                distance_map[row][col];
        }
    }
}


// ============================================================
// GET COMPLETE STATUS MAP
// ============================================================

void tof_get_statuses(
    uint8_t output[TOF_ROWS][TOF_COLS]
)
{
    for (
        uint8_t row = 0;
        row < TOF_ROWS;
        row++
    )
    {
        for (
        uint8_t col = 0;
        col < TOF_COLS;
        col++
        )
        {
            output[row][col] =
                status_map[row][col];
        }
    }
}


// ============================================================
// CHECK WHETHER A FRAME EXISTS
// ============================================================

bool tof_has_frame()
{
    return frame_available;
}


// ============================================================
// CHECK FOR NEW FRAME
// ============================================================

bool tof_new_frame()
{
    if (
        new_frame_flag
        == false
    )
    {
        return false;
    }


    new_frame_flag = false;

    return true;
}
