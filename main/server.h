#ifndef SERVER_H
#define SERVER_H
#include <Ethernet.h>
#include <ArduinoJson.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
static byte mac[] = {
    0x00, 0xDE, 0x96, 0x27, 0xD2, 0x02
};

// Initialize the Ethernet server library with port 80
// (port 80 is default for HTTP):
static EthernetServer server(80);

// Initialize the JSON object
static DynamicJsonDocument jsonDoc(1024);

/**
 * Initialize the Ethernet connection with a dynamic IP address
**/
void initEthernet() {
    Ethernet.init(17);  // CS pin for W5500-EVB-Pico 

    // start the Ethernet connection:
    Serial.println("Initialize Ethernet with DHCP:");
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
            Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        } else if (Ethernet.linkStatus() == LinkOFF) {
            Serial.println("Ethernet cable is not connected.");
        }
    }
    // print your local IP address:
    server.begin();
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
}

/**
 * Maintain the Ethernet connection, should be called fairly periodically to
 * keep the IP address assigned.
**/
void maintainEthernet() {
    switch (Ethernet.maintain()) {
        case 1:
            //renewed fail
            Serial.println("Error: renewed fail");
            break;

        case 2:
            //renewed success
            Serial.println("Renewed success");
            //print your local IP address:
            Serial.print("My IP address: ");
            Serial.println(Ethernet.localIP());
            break;

        case 3:
            //rebind fail
            Serial.println("Error: rebind fail");
            break;

        case 4:
            //rebind success
            Serial.println("Rebind success");
            //print your local IP address:
            Serial.print("My IP address: ");
            Serial.println(Ethernet.localIP());
            break;

        default:
            //nothing happened
            break;
    }
}

/**
 * Setup and maintain the HTTP server
 *
 * Returns the JSON data on HTTP requests
 *
 * TODO: Add the correct CORS header, is probably just adding
 * `Access-Control-Allow-Origin: *` below "Content-Type: text/html"
**/
void httpServer() {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
        Serial.println("new client");
        // an http request ends with a blank line
        bool currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the http request has ended,
                // so you can send a reply
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println(F("HTTP/1.1 200 OK"));
                    client.println(F("Content-Type: text/html"));
                    // the connection will be closed after completion of the
                    // response
                    client.println(F("Connection: close"));  
                    // refresh the page automatically every 5 sec
                    client.println(F("Refresh: 5"));  
                    client.print(F("Content-Length: "));
                    client.println(measureJsonPretty(jsonDoc));
                    client.println();
                    serializeJsonPretty(jsonDoc, client);
                    break;
                }
                if (c == '\n') {
                    // you're starting a new line
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}

/**
 * Updates the internal JSON document with the new data
 */
void updateJsonDoc(
    double *temp,
    double *humidity,
    float *light_lx,
    double *light_intensity,
    double *pressure,
    float *rainfall,
    float *wind_dir,
    float *wind_speed
) {
    jsonDoc["temp_celsius"] = *temp;
    jsonDoc["humidity_rh"] = *humidity;
    jsonDoc["light_lx"] = *light_lx;
    jsonDoc["light_intensity"] = *light_intensity;
    jsonDoc["pressure_pa"] = *pressure;
    jsonDoc["rainfall_mm"] = *rainfall;
    jsonDoc["wind_dir_deg"] = *wind_dir;
    jsonDoc["wind_speed_kmh"] = *wind_speed;

}
#endif
