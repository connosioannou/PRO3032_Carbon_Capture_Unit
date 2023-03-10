/*
   Every time communication is initiated, a code is subsequently
   generated to represent the result of the attempt. This can
   be used to monitor your program and pinpoint any difficulties
   your program might be having.

   The codes are as followed:

	RESULT_NULL = 0,             // This should not occur, and suggests a library logic error
	RESULT_OK = 1,               // Communication was successful
	RESULT_TIMEOUT = 2,          // Timed out waiting for a response
	RESULT_MATCH = 3,            // Received data does not match the usual syntax expected
	RESULT_CRC = 4,              // Received data does not match the CRC given
    RESULT_FILTER = 5,           // Filter was triggered (see FilterUsage example)
	RESULT_FAILED = 6            // Not currently used
*/

#include <Arduino.h>
#include "MHZ19.h"

#define RX_PIN 10
#define TX_PIN 11
#define BAUDRATE 9600                                      // Native to the sensor (do not change)

MHZ19 myMHZ19;
#if defined(ESP32)
HardwareSerial mySerial(2);                                // On ESP32 we do not require the SoftwareSerial library, since we have 2 USARTS available
#else
#include <SoftwareSerial.h>                                //  Remove if using HardwareSerial or non-uno compatible device
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
#endif

unsigned long getDataTimer = 0;

void setRange(int range);

void setup()
{
    Serial.begin(9600);

    mySerial.begin(BAUDRATE);                                // Uno Example: Begin Stream with MHZ19 baudrate
    myMHZ19.printCommunication();                            // Error Codes are also included here if found (mainly for debugging/interest)

    myMHZ19.begin(mySerial);                                 // *Important, Pass your Stream reference
}

void loop()
{

    if (millis() - getDataTimer >= 2000)
    {
        int CO2;                                        // Buffer for CO2
        CO2 = myMHZ19.getCO2();                         // Request CO2 (as ppm)

        if(myMHZ19.errorCode == RESULT_OK)              // RESULT_OK is an alias for 1. Either can be used to confirm the response was OK.
        {
            Serial.print("CO2 Value successfully Received: ");
            Serial.println(CO2);
            Serial.print("Response Code: ");
            Serial.println(myMHZ19.errorCode);          // Get the Error Code value
        }

        else
        {
            Serial.println("Failed to recieve CO2 value - Error");
            Serial.print("Response Code: ");
            Serial.println(myMHZ19.errorCode);          // Get the Error Code value
        }

        getDataTimer = millis();
    }
}
