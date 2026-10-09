#include "Keyboard.h"

const int sensorPin1 = A0;
const int sensorPin2 = A1;
const int sensorPin3 = A2;
const int sensorPin4 = A3;

const int debugMode = 0; // off

int sensorValue1 = 0;
int sensorValue2 = 0;
int sensorValue3 = 0;
int sensorValue4 = 0;

int sensorSum = 0; // 4センサーの値の合計値

const int thresholdDon = 200;  // 通常の打撃(ドン)
const int thresholdKa = 650;  // カッ

unsigned long lastHitTime = 0;
const unsigned long retriggerTime = 100;

void setup() {
  Keyboard.begin();
  Serial.begin(9600);
}

void loop() {
  sensorValue1 = analogRead(sensorPin1);
  sensorValue2 = analogRead(sensorPin2);
  sensorValue3 = analogRead(sensorPin3);
  sensorValue4 = analogRead(sensorPin4);

  sensorSum = sensorValue1 + sensorValue2 + sensorValue3 + sensorValue4;

  // 前回の入力から一定時間経過しているか
  if (millis() - lastHitTime >= retriggerTime) {

    if (sensorSum > thresholdDon && sensorSum < thresholdKa) {
      Serial.println("ドン");
      Keyboard.write('d');

      lastHitTime = millis();

    } else if (sensorSum >= thresholdKa) {
      Serial.println("カッ");
      Keyboard.write('k');

      lastHitTime = millis();
    }
  }
}