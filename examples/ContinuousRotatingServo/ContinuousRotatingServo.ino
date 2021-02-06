/*
 * ContinuousRotatingServo.cpp
 *
 *  Shows smooth speed ramp for a continuous rotating servo.
 *
 *  Copyright (C) 2019-2021  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of ServoEasing https://github.com/ArminJo/ServoEasing.
 *
 *  ServoEasing is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 */

#include <Arduino.h>

#include "ServoEasing.h"

#include "PinDefinitionsAndMore.h"
/*
 * Pin mapping table for different platforms
 *
 * Platform     Servo1      Servo2      Servo3      Analog
 * -------------------------------------------------------
 * AVR + SAMD   9           10          11          A0
 * ESP8266      14 // D5    12 // D6    13 // D7    0
 * ESP32        5           18          19          A0
 * BluePill     PB7         PB8         PB9         PA0
 * APOLLO3      11          12          13          A3
 */

ServoEasing Servo1;

void blinkLED();

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)  || defined(ARDUINO_attiny3217)
    delay(2000); // To be able to connect Serial monitor after reset or power up and before first printout
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_SERVO_EASING));

    // Attach servo to pin
    Serial.print(F("Attach servo at pin "));
    Serial.println(SERVO1_PIN);

    /*
     * Special attach parameters for continuous rotating servo. Only usable if stop value of your servo is exactly 1500 microseconds.
     * If the stop value of your servo is not exactly 1500 microseconds, you must modify the "MICROSECONDS_FOR_ROTATING_SERVO_STOP" value in ServoEasing.h
     *
     * -100 and +100 can be replaced with any value you like but do not forget to change them below and in loop too.
     */
    if (Servo1.attach(SERVO1_PIN, MICROSECONDS_FOR_ROTATING_SERVO_CLOCKWISE_MAX,
    MICROSECONDS_FOR_ROTATING_SERVO_COUNTER_CLOCKWISE_MAX, 100, -100) == INVALID_SERVO) {
        Serial.println(F("Error attaching servo"));
        while (true) {
            blinkLED();
        }
    }

    /**************************************************
     * Set servo to stop.
     *************************************************/
    Servo1.write(0);

    delay(2000);

    // Move slow clockwise
    Servo1.write(20);
    delay(1000);
    // Move faster clockwise
    Servo1.write(60);
    delay(1000);
    Servo1.write(0);
}

void blinkLED() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

void loop() {
    /*
     * Now move a speed ramp up and down
     */
    Serial.println(F("Rotate clockwise to maximum speed and back to stop blocking"));
    Servo1.write(0); // set start speed :-)
    Servo1.easeTo(100, 20);
    Servo1.easeTo(0, 20);
    delay(1000);

    Serial.println(F("Rotate counter clockwise to half speed and back to stop using interrupts"));
    Servo1.write(0); // set start speed :-)
    Servo1.startEaseTo(-50, 20);
    /*
     * Now you can run your program while the servo is moving.
     */
    while (Servo1.isMoving()) {
        blinkLED();
    }
    Servo1.startEaseTo(0, 20);
    while (ServoEasing::areInterruptsActive()) {
        blinkLED();
    }
    delay(1000);
}
