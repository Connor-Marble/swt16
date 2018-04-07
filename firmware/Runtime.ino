void setup() {
  Serial.begin(31250);

  randomSeed(analogRead(4));

  pinMode(TRIGGER_OUT_LATCH, OUTPUT);
  digitalWrite(TRIGGER_OUT_LATCH, 0);
  writeTriggers(TRIGGER_OUT_DATA, TRIGGER_OUT_CLOCK, 0);
  writeTriggers(TRIGGER_OUT_DATA, TRIGGER_OUT_CLOCK, 0);
  digitalWrite(TRIGGER_OUT_LATCH, 1);
  pinMode(INTERRUPT_CLOCK, INPUT_PULLUP);
  pinMode(INTERRUPT_RESET, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(HOME_BUTTON, INPUT);

  setupButtonBoard();
  loadData();
  initSequencer();
}

void setMode(byte modeId) {
  resetIdle();
  if (modeId == MODE_WRITE) {
    saveData();
    playAnimation(animations[MODE_WRITE]);
  } else {
    buttonsEnabled = false;
    currentMode = modeId;

    resetFillInvert();

    if (currentMode < 17) {
      playAnimation(animations[currentMode]);
    } else {
      playAnimation(animations[17]);
    }

    buttonsEnabled = true;
  }
}
void initSequencer() {
  delay(500);
  enDisableAutoPlay();
  setMode(MODE_HOME);

}

void checkIdle() {
  wasIdle = isIdle;
  isIdle = millis() > 60000 + onClockHighTime;
  if (wasIdle != isIdle) {
    for (byte foo = 0; foo < 16; foo++) {
      doSetLed(foo, false);
    }
    writeDisplay();
  }
}
void resetIdle() {
  wasIdle = false;
  isIdle = false;
  onClockHighTime = millis() - 1;
}
void loop() {
  checkIdle();
  delay(isIdle ? 120 : 30);
  checkButtons();
  updateDisplay();
  if (digitalRead(HOME_BUTTON) == HIGH && (currentMode != MODE_HOME || isIdle)) {
    setMode(MODE_HOME);
  }
}
