SerialUSB ser = SerialUSB();

void setup() {
    ser.begin(9600);

}

int x = 0;

void loop() {
    String prompt = String("Hello world! Num: ");
    ser.write(prompt.c_str(), prompt.length() + 1);
    String num_str = String(x);
    ser.write(num_str.c_str(), num_str.length() + 1);
    ser.write('\n');
    ser.write('\r');
    x += 1;
    delay(200);
}
