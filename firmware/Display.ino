void updateLightIsHigh() {
  lightIsHigh = (millis() - onClockHighTime < ((onClockLowTime - onClockHighTime) >> 1));
}
boolean playAnimation(byte animation[]) {
  if (disableAnimations) {
    for (byte foo = 0; foo < 16; foo++) {
      clearLed(foo);
    }
    for (byte foo = 0; foo < 10; foo++) {
      if (animation[foo] < 16) {
        setLed(animation[foo]);
      }
    }
    writeDisplay();
    delay(1000);
  } else {
    for (byte foo = 0; foo < 16; foo++) {
      clearLed(foo);
      writeDisplay();
      delay(20);
    }
    for (byte foo = 0; foo < 10; foo++) {
      if (animation[foo] < 16) {
        setLed(animation[foo]);
        writeDisplay();
        delay(30);
      }
    }
    delay(200);
    for (byte foo = 0; foo < 10; foo++) {
      clearLed(animation[foo]);
      writeDisplay();
      delay(30);
    }
    delay(20);
  }
  return true;
}
void updateDisplay() {
  updateLightIsHigh();
  if (isIdle) {
    idleTick %= 120;

    byte currentChar = idleTick % 20;
    byte currentAnimation = 18 + (idleTick / 20);

    doSetLed(animations[currentAnimation][currentChar % 10], currentChar < 10);
    idleTick += (animations[(18 + ((idleTick + 1) / 20))][((idleTick + 1) % 20) % 10] >= 16) ? 10 - idleTick % 10 : 1;
    writeDisplay();
  } else {
    idleTick = 0;
    for (uint8_t foo = 0; foo < 16; foo++) {
      if (currentMode == MODE_HOME) {
        doSetLed(foo, true);
      } else if ( currentMode == MODE_PERFORMANCE) {
        boolean lightUp = false;
        for (byte bar = 0; bar < 4; bar++) {
          byte track = (4 * performanceModeCurrentPage) + bar;
          lightUp = (foo == bar && performanceModeSettings[track] && getTrigger(track, patternData[track][PATTERN_CURRENT_TICK])) || (foo == bar && !performanceModeSettings[track] && patternData[track][PATTERN_MUTE] && !(lightIsHigh && getTrigger(track, patternData[track][PATTERN_CURRENT_TICK]) || (patternData[track][PATTERN_NOTES_ROW_0] == 0 && patternData[track][PATTERN_NOTES_ROW_1] == 0 )  )) || ((foo == bar + 4) && !performanceModeSettings[track] && (lightIsHigh &&  patternData[track][PATTERN_FILL] )) || ((foo == bar + 8) && performanceModeSettings[track]) || ((foo == bar + 12) && bar == performanceModeCurrentPage) ? true : lightUp;
        }
        doSetLed(foo, lightUp);
      } else if (currentMode == MODE_AUTOPLAY) {
        doSetLed(foo, (foo == 2  && autoClockEnabled && autoPlayTick % 4 == 0) || (foo == 3  && autoClockEnabled && autoPlayTick % 2 == 0) || ((foo == 8 || foo == 9 || foo == 12 || foo == 13) && autoClockEnabled) || ((foo == 10 || foo == 11 || foo == 14 || foo == 15) && !autoClockEnabled) || ((foo == 0 && autoClockEnabled) && autoPlayRun && autoPlayHot));
      } else if (currentMode == MODE_MUTE) {
        doSetLed(foo, patternData[foo][PATTERN_MUTE] && !(lightIsHigh && getTrigger(foo, patternData[foo][PATTERN_CURRENT_TICK]) || (patternData[foo][PATTERN_NOTES_ROW_0] == 0 && patternData[foo][PATTERN_NOTES_ROW_1] == 0 )));
      } else if (currentMode == MODE_FILL) {
        doSetLed(foo, patternData[foo][MODE_FILL] && !(lightIsHigh && getTrigger(foo, patternData[foo][PATTERN_CURRENT_TICK]) || (patternData[foo][PATTERN_NOTES_ROW_0] == 0 && patternData[foo][PATTERN_NOTES_ROW_1] == 0 )));
      } else if (currentMode == MODE_INVERSE) {
        doSetLed(foo, patternData[foo][PATTERN_INVERSE] && !(lightIsHigh && getTrigger(foo, patternData[foo][PATTERN_CURRENT_TICK]) || (patternData[foo][PATTERN_NOTES_ROW_0] == 0 && patternData[foo][PATTERN_NOTES_ROW_1] == 0 )  ));
      }  else if (currentMode == MODE_UTIL) {
        doSetLed(foo, (foo == 0 && disableAnimations) || (foo == 1 && autoReset) || (foo == 2 && flipMode));
      } else if (currentMode == MODE_LENGTH_EDIT) {
        doSetLed(foo, foo == patternData[currentTrack][PATTERN_LENGTH]);
      } else if (currentMode == MODE_SET_MIDI_CHANNEL) {
        doSetLed(foo, foo == patternData[currentTrack][PATTERN_MIDI_CHANNEL]);
      } else if (currentMode == MODE_SET_CLOCK_DIVIDER) {
        doSetLed(foo, foo == patternData[currentTrack][PATTERN_CLOCK]);
      } else if (currentMode == MODE_PATTERN_EDIT) {
        doSetLed(foo, getTrigger(currentTrack, foo));
      } else if (currentMode == MODE_LENGTH_RESET || currentMode == MODE_GATE_RETRIGGER) {
        doSetLed(foo, patternData[foo][currentMode == MODE_GATE_RETRIGGER ? PATTERN_RETRIGGER : currentMode == MODE_LENGTH_RESET ? PATTERN_LENGTH_RESET :  0]);
      } else {
        doSetLed(foo, lightIsHigh && getTrigger(foo, patternData[foo][PATTERN_CURRENT_TICK]));
      }
      if (currentMode == MODE_HOME || currentMode == MODE_SELECT_TRACK || currentMode == MODE_LENGTH || currentMode == MODE_NOTES || currentMode == MODE_CLOCK) {
        doSetLed(foo, tick != foo);
      }

    }
    if ( currentMode == MODE_PATTERN_EDIT ) {
      doSetLed(patternData[currentTrack][PATTERN_CURRENT_TICK], lightIsHigh);
    }
    writeDisplay();
  }
}
void doSetLed(uint8_t led, boolean lightUp) {
  if (lightUp) {
    setLed(led);
  } else {
    clearLed(led);
  }
}
