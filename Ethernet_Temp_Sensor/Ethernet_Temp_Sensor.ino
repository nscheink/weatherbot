#include <ArduinoJson.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <DFRobot_DHT20.h>
#include <SparkFunBMP384.h>
#include <Adafruit_VEML7700.h>
#include <RPi_Pico_TimerInterrupt.h>
#define WM_ADC_RESOLUTION 4096
#include <WeatherMeters.h>

/**
 * i2c0 connected on GPIO pins 0 and 1
 * The windvane pin is connected to ADC1
 * The anemometer pin is connected to GP15
 * The raingauge pin is connected to GP14
**/

/*******************************************************
 *               BEGIN PRESSURE SENSOR                 *
 *******************************************************/

/**
 * The internal object communicating with the pressure sensor hardware
 */
BMP384 pressureSensor;

/**
 * The i2c address of the pressure sensor
 */
uint8_t pressureSensorAddr = BMP384_I2C_ADDRESS_DEFAULT; // 0x77

/**
 * Initialize the pressure sensor
 * @param TwoWire i2c_driver - the i2c driver that handles the sensor connection
 */
void initPressureSensor(TwoWire *i2c_driver) {
    while(pressureSensor.beginI2C(pressureSensorAddr, *i2c_driver) != BMP3_OK) {
        // Inform and wait for connection
        Serial.println("Initializing BMP384 (Pressure) sensor failed!");
        delay(1000);
    }

    int8_t err = BMP3_OK;

    bmp3_odr_filter_settings osrMultipliers = 
    {
        .press_os = BMP3_OVERSAMPLING_32X,
        .temp_os = BMP3_OVERSAMPLING_2X,
        0,0
    };
    err = pressureSensor.setOSRMultipliers(osrMultipliers);
    if(err)
    {
        Serial.print("Error! while setting Pressure OSR, error code: ");
        Serial.println(err);
    }

    uint8_t odr=0;
    err = pressureSensor.getODRFrequency(&odr);
    if(err)
    {
        Serial.print("Error! while getting Pressure ODR! Error code: ");
        Serial.println(err);
    }

    Serial.print("ODR Frequency: ");
    Serial.print(200 / pow(2, odr));
    Serial.println("Hz");

}

/**
 * Read the pressure sensor data
 * @param double* pressure - where to store the read pressure data
 * @param double* bmp_temp - where to store the read temperature data
 */
void readPressureSensor(double *pressure, double *bmp_temp) {
    bmp3_data pressure_data;
    int8_t err = pressureSensor.getSensorData(&pressure_data);
    if (err == BMP3_OK) {
        *pressure = pressure_data.pressure;         
        *bmp_temp = pressure_data.temperature;
    } 
    else {
        Serial.print("Error! while retrieving pressure data, error code: ");
        Serial.println(err);
    }
}

/*******************************************************
 *                 BEGIN LIGHT SENSOR                  *
 *******************************************************/

/**
 * The internal object communicating with the light sensor hardware
**/
Adafruit_VEML7700 veml7700 = Adafruit_VEML7700();

/**
 * Initialize the light sensor
 * @param TwoWire i2c_driver - the i2c driver that handles the sensor connection
**/
void initLightSensor(TwoWire *i2c_driver) {
    while(!veml7700.begin(i2c_driver)) {
        Serial.println("Initializing VEML7700 (Light) sensor failed!");
        delay(1000);
    }
}

/**
 * Read the light sensor data
 * @param float* light_lx - where to store the read light_lx data
 * @param float* light_intensity - where to store the read light_intensity data
**/
void readLightSensor(float *light_lx, double *light_intensity) {
    *light_lx = veml7700.readLux(VEML_LUX_AUTO);
    *light_intensity = (*light_lx / 140000.) * 100.;
}

/*******************************************************
 *               BEGIN TEMP/HUM SENSOR                 *
 *******************************************************/

/**
 * The internal object communicating with the temperature/humidity sensor 
 * hardware
 */
DFRobot_DHT20 dht20;

/**
 * Initialize the temperature/humidity sensor
 * @param TwoWire i2c_driver - the i2c driver that handles the sensor connection
 */
void initTempHumSensor(TwoWire *i2c_driver) {
    dht20 = DFRobot_DHT20(i2c_driver, 0x38);
    while(dht20.begin()){
        Serial.println("Initializing DHT20 (Temp & Humidity) sensor failed!");
        delay(1000);
    }
}

/**
 * Read the temperature/humidity sensor data
 * @param double* temp - where to store the read temperature data
 * @param double* humidity - where to store the read humidity data
 */
void readTempHumSensor(double *temp, double *humidity) {
    *temp = dht20.getTemperature(); // Celsius
    *humidity = dht20.getHumidity()*100; // %RH
}

/*******************************************************
 *              BEGIN WEATHERKIT SENSOR                *
 *******************************************************/

/**
 * The windvane output pin is connected to analog pin A1
**/
const int windvane_pin = A1;

/**
 * The anemometer output pin is connected to GPIO pin 15
**/
const int anemometer_pin = 15;

/**
 * The raingauge output pin is connected to GPIO pin 14
**/
const int raingauge_pin = 14;

/**
 * The bit resolution to use for analog reads
 *
 * 12 is the maximum the Pi Pico supports
**/
const int ANALOG_READ_RESOLUTION = 12; // bits

/**
 * The sampling time to use for the weather station in seconds
 *
 * Rainfall and windspeed is integrated for these sampling times, and reset each
 * sample.
**/
const int SAMPLING_TIME = 4; // seconds

/**
 * The internal object communicating with the weather station sensor hardware
 *
 * The <0> disables the built-in moving average computations
**/
WeatherMeters <0> meters(windvane_pin, SAMPLING_TIME);

/**
 * Boolean that detects whether data is ready to be read from the library
**/
volatile bool meters_read_data = false;

// Create timer for weather kit
RPI_PICO_Timer ITimer(0);
const float TIMER_FREQ_HZ = 1.0;

/**
 * Called everytime an interrupt is generated by the rainfall sensor
**/
void interruptRainGauge() {
    meters.intRaingauge();
}

/**
 * Called everytime an interrupt is generated by the anemometer
**/
void interruptAnemometer() {
    meters.intAnemometer();
}


/**
 * Called periodically by the hardwire timer to run library timing code
**/
bool weatherMeterTimerHandler(struct repeating_timer *t) {
    (void) t;
    meters.timer();
    return true;
}

/**
 * Callback used to keep track of when data is ready to be read from the library
**/
void readDone(void) {
    meters_read_data = true;
}

/**
 * Initialize the weather kit
**/
void initWeatherKit() {
    analogReadResolution(ANALOG_READ_RESOLUTION);
    ITimer.attachInterrupt(TIMER_FREQ_HZ, weatherMeterTimerHandler);
    pinMode(anemometer_pin, INPUT_PULLUP);
    pinMode(raingauge_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(anemometer_pin), interruptAnemometer, FALLING);
    attachInterrupt(digitalPinToInterrupt(raingauge_pin), interruptRainGauge, FALLING);
    meters.attach(readDone);
    meters.reset();
}

/**
 * Read the weather kit data
 * @param float* wind_speed - where to store the read wind_speed data
 * @param float* wind_dir - where to store the read wind_dir data
 * @param float* rainfall - where to store the read rainfall data
**/
void readWeatherMeterKit(float *wind_speed, float *wind_dir, float *rainfall) {
    if (meters_read_data) {
        meters_read_data = false;
        *wind_speed = meters.getSpeed();
        *wind_dir = meters.getDir();
        *rainfall = meters.getRain();
    }
}

/*******************************************************
 *                 BEGIN HTTP SERVER                   *
 *******************************************************/

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
    0x00, 0xDE, 0x96, 0x27, 0xD2, 0x02
};

// Initialize the Ethernet server library with port 80
// (port 80 is default for HTTP):
EthernetServer server(80);

// Initialize the JSON object
DynamicJsonDocument jsonDoc(1024);

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

/*******************************************************
 *                    BEGIN MAIN                       *
 *******************************************************/

// Device is connected to i2c0 on GPIO pins 0 and 1
TwoWire i2c_driver = TwoWire(i2c0, 0, 1);

int debug = 0;

/**
 * Raspberry Pi Pico's entry point
**/
void setup() {

    // waitForSerial();

    // Initialize the Ethernet connection, which also attempts to grab an IP
    // address from a DHCP server
    initEthernet();

    initTempHumSensor(&i2c_driver);
    initLightSensor(&i2c_driver);
    initPressureSensor(&i2c_driver);
    initWeatherKit();
}

// Variables storing the data retrieved from the sensors

double temp = 0;            // Celsius
double humidity = 0;        // % RH
double light_intensity = 0; // % Intensity
float light_lx = 0;         // lx
double pressure = 0;        // Pascal
double bmp_temp = 0;        // Celsius
float wind_dir = 0;         // Degrees
float wind_speed = 0;       // km/h
float rainfall = 0;         // mm

/**
 * Halts code until a USB serial device is connected. Useful for debugging.
**/
void waitForSerial() {
    Serial.begin(9600);
    while (!Serial) {
        delay(100);
        ; // wait for serial port to connect. Needed for native USB port only
    }
}

/**
 * Displays sensor data on the serial monitor. Useful for debugging.
**/
void serialMonitor() {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %RH");
    
    Serial.print("Light Intensity: ");
    Serial.print(light_intensity);
    Serial.println(" \% of max");

    Serial.print("Air Pressure: ");
    Serial.print(pressure);
    Serial.println("Pa");

    Serial.print("BMP384 Temperature: ");
    Serial.print(bmp_temp);
    Serial.println(" C");

    Serial.println(" ---------- ");
    delay(200);
}

/**
 * Helper function to convert celsius to fahrenheit
 * @param double celsius - the input temperature in celsius
 * @return double fahrenheit - the output temperature in fahrenheit
**/
double convertCelsiusToFahrenheit(double celsius) {
    return (celsius * (9./5.)) + 32.;
}

/**
 * Variable used for timing when to collect data from the sensors. Data
 * collection needs to be delayed (for instance, every 4 seconds instead of
 * every 1 milliseconds) to prevent sensors from heating up.
**/
int counter=0;

/**
 * Pico function that continuously runs after the `setup` function is completed.
**/
void loop() {

    // Increment counter to keep track of the amount of milliseconds
    // Increment by 2 because delay of 1 in this loop, and delay of 1 in
    // ethernet. Timing could be improved
    counter += 2;

    // Only collect data every 4 seconds (4000ms)
    if (counter >= 4000){

        // Collect data
        readTempHumSensor(&temp, &humidity);
        readLightSensor(&light_lx, &light_intensity);
        readPressureSensor(&pressure, &bmp_temp);
        readWeatherMeterKit(&wind_speed, &wind_dir, &rainfall);

        // Update the JSON data
        updateJsonDoc(
            &temp,
            &humidity,
            &light_lx,
            &light_intensity,
            &pressure,
            &rainfall,
            &wind_dir,
            &wind_speed
        );
        
        // Reset the counter
        counter = 0;
    }

    // Maintain the HTTP server
    maintainEthernet();
    httpServer();

    delay(1);
}
