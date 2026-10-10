#include "Keyboard.h"

// センサー配置
const int sensorPins[4] = {A0, A1, A2, A3};

// A0: 左カッ、A1: 左ドン、A2: 右ドン、A3: 右カッ
const char keys[4] = {'d', 'f', 'j', 'k'};
const char* names[4] = {
  "左カッ", "左ドン", "右ドン", "右カッ"
};
// ===== 調整する設定値 =====

// センサーごとの検出閾値
const int thresholds[4] = {160, 100, 100, 160};

// 最初の反応後、ピークを集める時間 (ms)
const unsigned long decisionWindowMs = 8;

// 1回の打撃後、次の打撃を抑制する時間 (ms)
const unsigned long retriggerTimeMs = 45;

// 直前の打撃からの経過時間がこれ以内なら、
// 弱い共振候補を抑制する (ms)
const unsigned long resonanceWindowMs = 100;

// 共振候補の最大振幅 / 直前の打撃振幅
// 0.0～1.0。小さいほど共振を抑制しやすい
const float resonanceRatio = 0.45;

// デバッグ出力
const bool debugMode = true;

// ===== 内部状態 =====

enum State {
  IDLE,
  COLLECTING
};

State state = IDLE;

int peakValues[4] = {0, 0, 0, 0};
int currentValues[4] = {0, 0, 0, 0};

unsigned long firstHitTime = 0;
unsigned long lastHitTime = 0;

int lastHitIndex = -1;
int lastHitPeak = 0;

void resetCollection() {
  for (int i = 0; i < 4; i++) {
    peakValues[i] = 0;
  }
}

void readSensors() {
  for (int i = 0; i < 4; i++) {
    currentValues[i] = analogRead(sensorPins[i]);
  }
}

void updatePeaks() {
  for (int i = 0; i < 4; i++) {
    if (currentValues[i] > peakValues[i]) {
      peakValues[i] = currentValues[i];
    }
  }
}

void sendHit(int index, int peak) {
  Keyboard.write(keys[index]);

  lastHitTime = millis();
  lastHitIndex = index;
  lastHitPeak = peak;

  if (debugMode) {
    Serial.print("HIT: ");
    Serial.print(names[index]);
    Serial.print(" peak=");
    Serial.println(peak);
  }
}

void setup() {
  Serial.begin(115200);
  Keyboard.begin();
}

void loop() {
  readSensors();

  unsigned long now = millis();

  // 常時センサー値を表示
  if (debugMode) {
    static unsigned long lastDebugTime = 0;

    if (now - lastDebugTime >= 20) {
      lastDebugTime = now;

      Serial.print("A0:");
      Serial.print(currentValues[0]);
      Serial.print(" A1:");
      Serial.print(currentValues[1]);
      Serial.print(" A2:");
      Serial.print(currentValues[2]);
      Serial.print(" A3:");
      Serial.println(currentValues[3]);
    }
  }

  // 最初の打撃候補を検出
  if (state == IDLE) {
    // 前回入力からのクールダウン
    if (now - lastHitTime < retriggerTimeMs &&
        lastHitTime != 0) {
      return;
    }

    int triggerIndex = -1;

    for (int i = 0; i < 4; i++) {
      if (currentValues[i] >= thresholds[i]) {
        triggerIndex = i;
        break;
      }
    }

    if (triggerIndex >= 0) {
      resetCollection();
      updatePeaks();

      firstHitTime = now;
      state = COLLECTING;
    }

    return;
  }

  // 判定期間中は各センサーのピークを収集
  updatePeaks();

  if (now - firstHitTime < decisionWindowMs) {
    return;
  }

  // 閾値を超えたセンサーのうち最大のものを選ぶ
  int bestIndex = -1;
  int bestPeak = 0;

  for (int i = 0; i < 4; i++) {
    if (peakValues[i] >= thresholds[i] &&
        peakValues[i] > bestPeak) {
      bestPeak = peakValues[i];
      bestIndex = i;
    }
  }

  if (bestIndex >= 0) {
    bool suppressAsResonance = false;

    // 直前の打撃直後に発生した弱い信号を抑制
    if (lastHitIndex >= 0 &&
        now - lastHitTime < resonanceWindowMs &&
        bestPeak < lastHitPeak * resonanceRatio) {
      suppressAsResonance = true;
    }

    if (!suppressAsResonance) {
      sendHit(bestIndex, bestPeak);
    } else if (debugMode) {
      Serial.println("SUPPRESSED: possible resonance");
    }
  }

  state = IDLE;
}