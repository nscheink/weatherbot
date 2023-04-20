#define INTRPT_PIN 8 

#define SWITCH_PIN 9 

 
 

unsigned long timeInit, timeCurrent;
float Total_time, speed, averagespeed; 

int cycles = 0; 

int cycflag = 0; 

 
 

void setup() { 

pinMode(INTRPT_PIN, INPUT); 

attachInterrupt(digitalPinToInterrupt(INTRPT_PIN), cycle, RISING); 


 
 

Serial.begin(9600); 

timeInit = millis(); 

} 

 
 

void loop() { 


Serial.print(cycles); 

Serial.print(" "); 

Serial.print(Total_time); 

Serial.print(" "); 

Serial.print(averagespeed); 

Serial.print(" km/hour"); 

Serial.print("\n"); 

delay(1000);

} 

 
 

void cycle() { 

cycles++; 

timeCurrent = millis(); 

Total_time = timeCurrent-timeInit; 

timeInit = timeCurrent; 

speed = 2400/(Total_time); //2.4km/hr per 1 second click 
// 

//average values
averagespeed = (99.*averagespeed + speed)/100.;

} 

 
 
 

 