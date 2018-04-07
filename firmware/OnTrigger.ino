boolean getTrigger(byte track, byte tick) {
  return (tick < 8 ? (patternData[track][5] & (1 << tick)) : (patternData[track][6] & (1 << ( tick - 8 ))));
}

void onReset() {
  if (tick > 1 || autoClockEnabled) {
    for (byte foo = 0; foo < 16; foo++) {
      patternData[foo][PATTERN_CURRENT_TICK] = 0;
    }
    tick = 0;
    autoPlayHot = true;
    autoPlayTick = 0;
  }
}

void onClock() {
  clockIsHigh = autoClockEnabled ? autoPlayHot && autoPlayRun ? true : false : digitalRead(INTERRUPT_CLOCK);

  if (clockIsHigh) {
    onClockLowTime = onClockHighTime;
    onClockHighTime = millis();


  }
  if (autoReset) {
    if (onClockHighTime > 1000 + onClockLowTime) {
      onReset();
    }
  }
  updateLightIsHigh();

  autoPlayHot = !autoPlayHot;

  autoPlayTick %= 16;
  tick %= 16;

  updateTrigger(clockIsHigh);

  tick += clockIsHigh;
  autoPlayTick += clockIsHigh;
}
void updateTrigger(boolean hot) {
  triggerOutputData[0] = triggerOutputData[1] = 0;

  for (byte foo = 0; foo < 16; foo++) {
    if (tick == 0 && patternData[foo][PATTERN_LENGTH_RESET] && patternData[foo][PATTERN_CLOCK] == 0) {
      patternData[foo][PATTERN_CURRENT_TICK] = 0;
    }
    patternData[foo][PATTERN_CURRENT_TICK] %= (patternData[foo][PATTERN_LENGTH] + 1);

    boolean validClock = hot && tick % (patternData[foo][PATTERN_CLOCK] + 1) == 0;
    boolean playTrigger = false;
    // Is Trigger
    playTrigger =  validClock && getTrigger(foo, patternData[foo][PATTERN_CURRENT_TICK]) ? true : playTrigger;
    // Is Fill
    playTrigger =  validClock && patternData[foo][PATTERN_FILL] ? true : playTrigger;
    // Retrigger
    playTrigger = !patternData[foo][PATTERN_RETRIGGER] && getTrigger(foo, (patternData[foo][PATTERN_CURRENT_TICK] + (patternData[foo][PATTERN_LENGTH] + 1) - 1) % (patternData[foo][PATTERN_LENGTH] + 1)) ? true : playTrigger;
    // Is Invers
    playTrigger  = validClock &&  patternData[foo][PATTERN_INVERSE] ? !playTrigger : playTrigger;
    // Is Muted
    playTrigger = !patternData[foo][PATTERN_MUTE] ? false : playTrigger;

    if (playTrigger) {
      triggerOutputData[0] += triggerOutputs[0][foo];
      triggerOutputData[1] += triggerOutputs[1][foo];

    }
    if (validClock) {
      patternData[foo][PATTERN_CURRENT_TICK]++;
    }
    /* DO MIDI*/
    if (playTrigger && !patternData[foo][PATTERN_MIDI_NOTE_ON]) {
      Serial.write(0x90 + patternData[foo][PATTERN_MIDI_CHANNEL]);

      Serial.write(36 + foo);
      Serial.write(100);
    } else if (!playTrigger && patternData[foo][PATTERN_MIDI_NOTE_ON]) {
      Serial.write(0x90 + patternData[foo][PATTERN_MIDI_CHANNEL]);

      Serial.write(36 + foo);
      Serial.write(0);
    }
    patternData[foo][PATTERN_MIDI_NOTE_ON] = playTrigger;
    /* /DO MIDI */
  }

  digitalWrite(TRIGGER_OUT_LATCH, 0);
  writeTriggers(TRIGGER_OUT_DATA, TRIGGER_OUT_CLOCK, triggerOutputData[0]);
  writeTriggers(TRIGGER_OUT_DATA, TRIGGER_OUT_CLOCK, triggerOutputData[1]);
  digitalWrite(TRIGGER_OUT_LATCH, 1);
  digitalWrite(STATUS_LED, currentMode == MODE_HOME ? clockIsHigh  : HIGH);

}
void writeTriggers(int dataPin, int clockPin, byte data) {
  int i = 0;
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, 0);
  digitalWrite(clockPin, 0);
  for (i = 7; i >= 0; i--) {
    digitalWrite(clockPin, 0);
    digitalWrite(dataPin, data & (1 << i) ? 1 : 0);
    digitalWrite(clockPin, 1);
    digitalWrite(dataPin, 0);
  }
  digitalWrite(clockPin, 0);
}


void setAutoPlayTempo() {
  Timer1.setPeriod(map(autoPlayTempo, 40, 240, 93000, 15000));
}
void enDisableAutoPlay() {
  if (autoClockEnabled) {
    detachInterrupt(digitalPinToInterrupt(INTERRUPT_CLOCK));
    detachInterrupt(digitalPinToInterrupt(INTERRUPT_RESET));
    Timer1.initialize(map(autoPlayTempo, 40, 240, 93000, 15000));
    Timer1.attachInterrupt(onClock);
  } else {
    Timer1.detachInterrupt();
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_CLOCK), onClock, HIGH);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_RESET), onReset, CHANGE);
  }
}
