#include "Keyboard.h"

const int sensorLeftPin1 = A0;  // 左カッ
const int sensorLeftPin2 = A1;  // 左ドン
const int sensorRightPin3 = A2; // 右ドン
const int sensorRightPin4 = A3; // 右カッ

const int debugMode = 1;

int sensorValue1 = 0;
int sensorValue2 = 0;
int sensorValue3 = 0;
int sensorValue4 = 0;

const int thresholdDon = 100;
const int thresholdKa = 260;

unsigned long lastHitTime = 0;
const unsigned long retriggerTime = 45;

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
    int sensorValues[] = {
      sensorValue1,
      sensorValue2,
      sensorValue3,
      sensorValue4
    };

    const char keys[] = {'d', 'f', 'j', 'k'};
    const char* names[] = {
      "左カッ",
      "左ドン",
      "右ドン",
      "右カッ"
    };

    int maxIndex = 0;
    int maxValue = sensorValues[0];

    // 最大値と、そのセンサーのインデックスを取得
    for (int i = 1; i < 4; i++) {
      if (sensorValues[i] > maxValue) {
        maxValue = sensorValues[i];
        maxIndex = i;
      }
    }

    // 打撃を検出した場合のみキーを送信
    if (maxValue >= thresholdDon) {
      Serial.println(names[maxIndex]);
      Keyboard.write(keys[maxIndex]);
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
