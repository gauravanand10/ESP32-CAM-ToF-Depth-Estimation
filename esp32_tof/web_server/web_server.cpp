#include "web_server.h"

#include <Arduino.h>
#include <WebServer.h>

#include "tof_sensor.h"
#include "index_html.h"


// ============================================================
// HTTP SERVER
// ============================================================

static WebServer server(80);


// ============================================================
// ROOT PAGE
// ============================================================

static void handle_root()
{
    server.send(
        200,
        "text/html",
        INDEX_HTML
    );
}


// ============================================================
// TOF JSON ENDPOINT
// ============================================================
//
// GET /tof
//
// Returns the latest 8x8 ToF measurement:
//
// {
//     "sequence": 123,
//     "distance": [ ...64 values... ],
//     "status":   [ ...64 values... ]
// }
//
// Distance is in millimeters.
// Status 5 means "Range valid", matching the existing
// VL53L5CX firmware/data format.
//

static void handle_tof()
{
    uint16_t distances[
        TOF_ROWS
    ][
        TOF_COLS
    ];

    uint8_t statuses[
        TOF_ROWS
    ][
        TOF_COLS
    ];


    // --------------------------------------------------------
    // Copy latest sensor data
    // --------------------------------------------------------

    tof_get_distances(
        distances
    );

    tof_get_statuses(
        statuses
    );


    uint16_t sequence =
        tof_get_sequence();


    // --------------------------------------------------------
    // Build JSON response
    // --------------------------------------------------------

    String json;

    json.reserve(1200);


    json += "{";


    // --------------------------------------------------------
    // Sequence number
    // --------------------------------------------------------

    json += "\"sequence\":";
    json += String(sequence);

    json += ",";


    // --------------------------------------------------------
    // Distance array
    // --------------------------------------------------------

    json += "\"distance\":[";


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


            json += String(
                distances[row][col]
            );


            if (index < 63)
            {
                json += ",";
            }
        }
    }


    json += "],";


    // --------------------------------------------------------
    // Status array
    // --------------------------------------------------------

    json += "\"status\":[";


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


            json += String(
                statuses[row][col]
            );


            if (index < 63)
            {
                json += ",";
            }
        }
    }


    json += "]";


    json += "}";


    // --------------------------------------------------------
    // Send response
    // --------------------------------------------------------

    server.send(
        200,
        "application/json",
        json
    );
}


// ============================================================
// STATUS ENDPOINT
// ============================================================
//
// GET /status
//
// Simple endpoint for checking whether the sensor has
// successfully produced a frame.
//

static void handle_status()
{
    String json;

    json.reserve(200);


    json += "{";

    json += "\"sensor\":";

    if (tof_has_frame())
    {
        json += "\"VL53L5CX\"";
    }
    else
    {
        json += "\"waiting\"";
    }

    json += ",";

    json += "\"frame\":";
    json += String(
        tof_get_sequence()
    );

    json += "}";


    server.send(
        200,
        "application/json",
        json
    );
}


// ============================================================
// NOT FOUND
// ============================================================

static void handle_not_found()
{
    String message;

    message += "404 Not Found\n\n";

    message += "Available endpoints:\n";

    message += "/\n";

    message += "/tof\n";

    message += "/status\n";


    server.send(
        404,
        "text/plain",
        message
    );
}


// ============================================================
// INITIALIZE WEB SERVER
// ============================================================

void web_server_init()
{
    // --------------------------------------------------------
    // Main web page
    // --------------------------------------------------------

    server.on(
        "/",
        HTTP_GET,
        handle_root
    );


    // --------------------------------------------------------
    // Current ToF frame
    // --------------------------------------------------------

    server.on(
        "/tof",
        HTTP_GET,
        handle_tof
    );


    // --------------------------------------------------------
    // Sensor status
    // --------------------------------------------------------

    server.on(
        "/status",
        HTTP_GET,
        handle_status
    );


    // --------------------------------------------------------
    // Unknown URL
    // --------------------------------------------------------

    server.onNotFound(
        handle_not_found
    );


    // --------------------------------------------------------
    // Start HTTP server
    // --------------------------------------------------------

    server.begin();
}


// ============================================================
// HANDLE HTTP CLIENTS
// ============================================================

void web_server_handle()
{
    server.handleClient();
}
