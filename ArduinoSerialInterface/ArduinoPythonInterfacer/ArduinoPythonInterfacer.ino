#include <Servo.h>;



volatile int count = 0; //number of encoder counts (using a quadrature encoder)

//encoder pins: pins 2,3 are hardware interrupts
const int encoderA = 2; 
const int encoderB = 3;

//actuator pins: pins 3,5,6,9,10,11 are capable of hardware pwm
Servo motor;
Servo steering;
const int motorPin = 5;
const int servoPin = 6;

//end flag and delimiter used to parse incoming serial message of setpoints, etc.
//Your message should be of the form: "XXX,YYY,ZZZ,etc.f". Note that there are no spaces.
char flag = 'f';
char delim[2] = ",";

volatile unsigned long curr_time;
volatile unsigned long temp;
char *parser;
float setPoint[3];
char readIn[16];
int i;

void setup() {
  // put your setup code here, to run once:
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);
  motor.attach(motorPin);
  steering.attach(servoPin);
  
  attachInterrupt(digitalPinToInterrupt(encoderA), inc, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB), inc, CHANGE);
  curr_time = millis();
  Serial.begin(250000);
  Serial.println("Running");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  temp = millis()-curr_time;
  if (temp > 2) {
    Serial.println(count/temp * 1000); //prints velocity out to the serial port
    count = 0;
    curr_time = millis();
  }


  //parses any available incoming data. Use atof() to convert the characters to floats (for math manipulation)
  temp = Serial.available();
  if (temp > 0) {
    i = 0;
    Serial.readBytesUntil(flag, readIn, 8);
    parser = strtok(readIn, delim);
    while (parser != NULL) {
      //Serial.println(atof(parser) + 2);
      setPoint[i] = atof(parser);
      i++;
      parser = strtok(NULL, delim);
    }

    motor.write(saturate(setPoint[0]));
    steering.write(saturate(setPoint[1]));
  }
  
  delay(1);
}

void inc() {
  count++;
}

float saturate(float value) {
  if (value>180) {
    value = 180; 
  } else if (value<0) {
    value = 0;
  }
  return value;
}
