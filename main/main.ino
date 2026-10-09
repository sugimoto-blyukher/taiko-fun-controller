#include "Keyboard.h"

const int sensorLeftPin1 = A0;
const int sensorLeftPin2 = A1;
const int sensorRightPin3 = A2;
const int sensorRightPin4 = A3;

const int debugMode = 0; // 1: on, 0: off

int sensorValue1 = -1;
int sensorValue2 = -1;
int sensorValue3 = -1;
int sensorValue4 = -1;

const int thresholdDon = 200;
const int thresholdKa = 300;

unsigned long lastHitTime = 0;
const unsigned long retriggerTime = 1;

void setup() {
  Keyboard.begin();
  Serial.begin(9600);
}

void loop() {
  sensorValue1 = analogRead(sensorLeftPin1);
  sensorValue2 = analogRead(sensorLeftPin2);
  sensorValue3 = analogRead(sensorRightPin3);
  sensorValue4 = analogRead(sensorRightPin4);

  if (millis() - lastHitTime >= retriggerTime) {

    // 左カッ
    if (sensorValue1 >= thresholdKa) {
      Serial.println("左カッ");
      Keyboard.write('d');
      lastHitTime = millis();
    }

    // 左ドン
    else if (sensorValue2 >= thresholdDon) {
      Serial.println("左ドン");
      Keyboard.write('f');
      lastHitTime = millis();
    }

    // 右カッ
    else if (sensorValue3 >= thresholdKa) {
      Serial.println("右カッ");
      Keyboard.write('k');
      lastHitTime = millis();
    }

    // 右ドン
    else if (sensorValue4 >= thresholdDon) {
      Serial.println("右ドン");
      Keyboard.write('j');
      lastHitTime = millis();
    }
  }

  if (debugMode == 1) {
    Serial.print("A0: ");
    Serial.print(sensorValue1);
    Serial.print(" A1: ");
    Serial.print(sensorValue2);
    Serial.print(" A2: ");
    Serial.print(sensorValue3);
    Serial.print(" A3: ");
    Serial.println(sensorValue4);
  }
}
