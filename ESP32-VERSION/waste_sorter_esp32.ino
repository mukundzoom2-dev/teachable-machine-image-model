// Smart Waste Segregator - ESP32 version
// Two servo lifts controlled from the webcam page.
// The page sends "L1\n" to raise lift 1 and "L2\n" to raise lift 2 (9600 baud),
// exactly like the Arduino Uno version, so the page needs no changes.
//
// Library needed: "ESP32Servo" by Kevin Harrington (Arduino IDE -> Library Manager)
// Board: "ESP32 Dev Module" (or your exact ESP32 board)
//
// Wiring (each servo):
//   signal -> GPIO pin below
//   power  -> external 5V supply (NOT the ESP32 pin)
//   ground -> supply ground AND ESP32 GND (common ground)
//
// GPIO 18 and 19 are safe general-purpose pins. Avoid GPIO 1 and 3 (USB serial),
// GPIO 6-11 (flash), and GPIO 0, 2, 12, 15 (boot pins) for the servos.

#include <ESP32Servo.h>

const int SERVO1_PIN = 18;
const int SERVO2_PIN = 19;

const int REST_ANGLE = 0;           // lowered position
const int LIFT_ANGLE = 90;          // raised position
const unsigned long HOLD_MS = 3000; // how long a lift stays up
const int STEP_DELAY_MS = 10;       // smaller = faster movement

Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(9600);

  // ESP32Servo needs a 50 Hz PWM signal; these timers are reserved for servos
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);

  // 500-2400 us pulse range suits SG90 / MG90S / MG996R
  servo1.attach(SERVO1_PIN, 500, 2400);
  servo2.attach(SERVO2_PIN, 500, 2400);

  servo1.write(REST_ANGLE);
  servo2.write(REST_ANGLE);

  Serial.println("Waste sorter ready (ESP32)");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "L1") {
      liftAndLower(servo1, 1);
    } else if (cmd == "L2") {
      liftAndLower(servo2, 2);
    }
  }
}

// Raise the servo smoothly, hold, then lower it smoothly
void liftAndLower(Servo &s, int number) {
  for (int a = REST_ANGLE; a <= LIFT_ANGLE; a++) {
    s.write(a);
    delay(STEP_DELAY_MS);
  }
  delay(HOLD_MS);
  for (int a = LIFT_ANGLE; a >= REST_ANGLE; a--) {
    s.write(a);
    delay(STEP_DELAY_MS);
  }
  Serial.print("Lift ");
  Serial.print(number);
  Serial.println(" done");
}
