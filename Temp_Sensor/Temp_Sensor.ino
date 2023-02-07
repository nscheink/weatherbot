#include <Wire.h>
#include <DFRobot_DHT20.h>

// Device is connected to i2c0 on GPIO pins 0 and 1
TwoWire i2c_driver = TwoWire(i2c0, 0, 1);

// Initialize the dht20 object with the i2c connection and address 0x38
DFRobot_DHT20 dht20(&i2c_driver, 0x38);

// Setup communications
void setup(){
    Serial.begin(9600);
    while(dht20.begin()){
        Serial.println("Initialize sensor failed");
        delay(1000);
    }
}

double temp = 0;
double humidity = 0;
void loop(){
    temp = dht20.getTemperature(); // Celsius
    humidity = dht20.getHumidity()*100; // %RH

    Serial.print("temperature:"); 
    Serial.print(temp);
    Serial.print("C  ");

    Serial.print("humidity:"); 
    Serial.print(humidity);
    Serial.println(" %RH");

    delay(500);
}
