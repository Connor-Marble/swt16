boolean getTrigger(byte track, byte tick, byte length) {
        return (patternData[track][PATTERN_BAR_SELECTED + 1 + (tick / (length + 1))] & (1 << (tick % (length + 1))));
}
void onReset() {
        if(!debugMode) {
                if (tick > 1 || autoClockEnabled) {
                        for (byte foo = 0; foo < 16; foo++) {
                                patternData[foo][PATTERN_CURRENT_TICK] = 0;
                        }
                        tick = 0;
                        autoPlayHot = true;
                        autoPlayTick = 0;
                }
        }
}
void onClock() {
        if(!debugMode) {
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

                autoPlayTick %= 64;
                tick %= 64;

                updateTrigger(clockIsHigh);

                tick += clockIsHigh;
                autoPlayTick += clockIsHigh;
        }
}

void updateTrigger(boolean hot) {
        triggerOutputData[0] = triggerOutputData[1] = 0;
        byte lngth = 0;
        for (byte foo = 0; foo < 16; foo++) {
                if (tick == 0 && patternData[foo][PATTERN_LENGTH_RESET] && patternData[foo][PATTERN_CLOCK] == 0) {
                        patternData[foo][PATTERN_CURRENT_TICK] = 0;
                }
                lngth = (patternData[foo][PATTERN_LENGTH] + 1) * (patternData[foo][PATTERN_BAR_LENGTH] + 1);
                patternData[foo][PATTERN_CURRENT_TICK] %= lngth;
                boolean validClock = hot && tick % (patternData[foo][PATTERN_CLOCK] + 1) == 0;

                boolean playTrigger = false;
                // Is Trigger
                playTrigger = validClock && getTrigger(foo, patternData[foo][PATTERN_CURRENT_TICK], patternData[foo][PATTERN_LENGTH]) ? true : playTrigger;
                // Retrigger
                playTrigger = !patternData[foo][PATTERN_RETRIGGER] && !validClock  && getTrigger(foo, (patternData[foo][PATTERN_CURRENT_TICK] - 1 + lngth) % lngth,   patternData[foo][PATTERN_LENGTH]) ? true : playTrigger;
                // Is Muted
                playTrigger = !patternData[foo][PATTERN_MUTE] ? false : playTrigger;
                // Is Invers
                playTrigger  = validClock &&  patternData[foo][PATTERN_INVERSE] ? !playTrigger : playTrigger;
                // Is Fill
                playTrigger =  validClock && patternData[foo][PATTERN_FILL] ? true : playTrigger;

                if (playTrigger) {
                        triggerOutputData[0] += triggerOutputs[0][flipMode ? 15 - foo : foo];
                        triggerOutputData[1] += triggerOutputs[1][flipMode ? 15 - foo : foo];
                }
                if (validClock) {
                        patternData[foo][PATTERN_CURRENT_TICK]++;
                }
                /* DO MIDI*/
                if (playTrigger && !patternData[foo][PATTERN_MIDI_NOTE_ON]) {
                        Serial.write(0x90 + patternData[foo][PATTERN_MIDI_CHANNEL]);
                        Serial.write(36 + (flipMode ? 15 - foo : foo));
                        Serial.write(100);
                } else if (!playTrigger && patternData[foo][PATTERN_MIDI_NOTE_ON]) {
                        Serial.write(0x90 + patternData[foo][PATTERN_MIDI_CHANNEL]);
                        Serial.write(36 + (flipMode ? 15 - foo : foo));
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
