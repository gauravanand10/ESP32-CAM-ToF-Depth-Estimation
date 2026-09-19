#include <Arduino.h>
#include <WiFi.h>

#include "tof_sensor.h"
#include "web_server.h"


// ============================================================
// WIFI CONFIGURATION
// ============================================================

const char* WIFI_SSID = "Gaurav";
const char* WIFI_PASSWORD = "1234567890";


// ============================================================
// SETUP
// ============================================================

void setup()
{
    // --------------------------------------------------------
    // Debug serial ONLY
    //
    // IMPORTANT:
    // This Serial interface is NOT used to transmit ToF data.
    // It is only used to print startup/debug information.
    // --------------------------------------------------------

    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println(" VL53L5CX Wi-Fi ToF WebServer");
    Serial.println("========================================");
    Serial.println();


    // --------------------------------------------------------
    // Initialize ToF sensor
    // --------------------------------------------------------

    Serial.println("Initializing VL53L5CX...");

    if (!tof_init())
    {
        Serial.println("ERROR: VL53L5CX initialization failed!");

        while (1)
        {
            delay(1000);
        }
    }

    Serial.println("VL53L5CX initialized.");
    Serial.println();


    // --------------------------------------------------------
    // Connect to Wi-Fi
    // --------------------------------------------------------

    Serial.print("Connecting to Wi-Fi");

    WiFi.mode(WIFI_STA);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );


    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        Serial.print(".");
    }


    Serial.println();
    Serial.println();

    Serial.println("Wi-Fi connected.");

    Serial.print("ESP32 IP address: ");

    Serial.println(
        WiFi.localIP()
    );

    Serial.println();


    // --------------------------------------------------------
    // Start HTTP web server
    // --------------------------------------------------------

    web_server_init();

    Serial.println("HTTP server started.");

    Serial.print("Open in browser: http://");

    Serial.print(
        WiFi.localIP()
    );

    Serial.println("/");

    Serial.println();
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
    // --------------------------------------------------------
    // Acquire newest VL53L5CX frame
    // --------------------------------------------------------

    tof_update();


    // --------------------------------------------------------
    // Handle HTTP requests
    // --------------------------------------------------------

    web_server_handle();


    // --------------------------------------------------------
    // Small delay
    // --------------------------------------------------------

    delay(1);
}
