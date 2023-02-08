#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <DFRobot_DHT20.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
    0x00, 0xDE, 0x96, 0x27, 0xD2, 0x02
};

// Initialize the Ethernet server library with port 80
// (port 80 is default for HTTP):
EthernetServer server(80);

// Device is connected to i2c0 on GPIO pins 0 and 1
TwoWire i2c_driver = TwoWire(i2c0, 0, 1);

// Light sensor connected to physical Pin 31, GP26, ADC0
const pin_size_t light_sensor_pin = A0;

// Initialize the dht20 object with the i2c connection and address 0x38
DFRobot_DHT20 dht20(&i2c_driver, 0x38);

// Analog read resolution in bits
const uint8_t ANALOG_RESOLUTION = 12;

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

void initTempHumSensor() {
    while(dht20.begin()){
        Serial.println("Initializing DHT20 (Temp & Humidity) sensor failed");
        delay(1000);
    }
}

void initLightSensor() {
    analogReadResolution(ANALOG_RESOLUTION);
}

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    initEthernet();
    initTempHumSensor();
    initLightSensor();
}

double temp = 0; // Celsius
double humidity = 0; // % RH
double light_intensity = 0; // % Intensity

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
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    // the connection will be closed after completion of the
                    // response
                    client.println("Connection: close");  
                    // refresh the page automatically every 5 sec
                    client.println("Refresh: 5");  
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");

                    // Output the temperature
                    client.print("Temperature: ");
                    client.print(temp);
                    client.println(" C");
                    client.println("<br />");

                    // Output the humidity
                    client.print("Humidity: ");
                    client.print(humidity);
                    client.println(" %RH");
                    client.println("<br />");

                    // Output the light intensity
                    client.print("Light: ");
                    client.print(light_intensity);
                    client.println(" %");
                    client.println("<br />");

                    client.println("</html>");
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

void readTempHumSensor() {
    temp = dht20.getTemperature(); // Celsius
    humidity = dht20.getHumidity()*100; // %RH
}

void readLightSensor() {
    light_intensity = 100. * 
        analogRead(light_sensor_pin) / (1 << ANALOG_RESOLUTION); // % of max
}

void loop() {
    maintainEthernet();
    httpServer();
    readTempHumSensor();
    readLightSensor();
    delay(1);
}