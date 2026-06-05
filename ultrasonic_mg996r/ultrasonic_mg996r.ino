/*
MG996R+Ulrasonics

Wire Connection
    MG996R servo
        Power (Red) → 6V external power supply
        GND (Brown) → shared GND with Arduino
        Signal (Orange) → Pin 9
    HC-SR04
        VCC → 5V (from Arduino)
        GND → GND
        TRIG → D10
        ECHO → D11
Result 
12:52:45.804 -> Rotating to 45°
12:52:46.301 -> Distance: 249 cm
12:52:46.301 -> Rotating to 60°
12:52:46.792 -> Distance: 248 cm
12:52:46.825 -> Rotating to 75°
12:52:47.300 -> Distance: 94 cm

Less than 10cm, shutdown MG996R

*/

#include <Servo.h>

#define TRIG_PIN 10
#define ECHO_PIN 11
#define SERVO_PIN 9

Servo servo;

int angle = 0;
int step = 1;
bool isStopped = false;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servo.attach(SERVO_PIN);
  servo.write(angle);  // Start at 0°
}

long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout at 30ms
  if (duration == 0) return -1;
  return duration * 0.034 / 2;  // Distance in cm
}

void loop() {
  long distance = measureDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance < 10) {
    if (!isStopped) {
      Serial.println("Too close (<10 cm) — Stopping servo.");
      servo.detach();  // Stop PWM signal
      isStopped = true;
    }
    delay(100);
    return;
  }

  if (isStopped) {
    Serial.println("Object moved away — Resuming servo.");
    servo.attach(SERVO_PIN);  // Resume PWM control
    servo.write(angle);
    isStopped = false;
  }

  // Sweep only between 0° and 10°
  angle += step;
  if (angle >= 10 || angle <= 0) {
    step = -step;
    angle += step;
  }

  servo.write(angle);
  Serial.print("Rotating to ");
  Serial.print(angle);
  Serial.println("°");

  delay(300);
}
