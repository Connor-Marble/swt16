void resetFillInvert() {
  for (byte foo = 0; foo < 16; foo++) {
    patternData[foo][PATTERN_FILL] = false;
    patternData[foo][PATTERN_INVERSE] = false;
  }
}
void checkButtons() {
  if (buttonsEnabled && readSwitches()) {
    for (uint8_t foo = 0; foo < 16; foo++) {
      if (justPressed(foo)) {
        resetIdle();
        if (currentMode == MODE_HOME) {
          setMode(foo);
          break;
        } else if (currentMode == MODE_AUTOPLAY) {
          onModeAutoPlay(foo);
        } else if (currentMode == MODE_PERFORMANCE) {
          onModePerformance(foo);
        } else if (currentMode == MODE_INVERSE) {
          onModeInverse(foo);
        } else if (currentMode == MODE_LENGTH_RESET) {
          onModeLengthReset(foo);
        } else if (currentMode == MODE_GATE_RETRIGGER) {
          onModeGateRetrigger(foo);
        } else if (currentMode == MODE_MUTE) {
          onModeMute(foo);
        }  else if (currentMode == MODE_SET_CLOCK_DIVIDER) {
          onModeSetClockDivider(foo);
          break;
        } else if (currentMode == MODE_LENGTH_EDIT) {
          onModeLengthEdit(foo);
          break;
        } else if (currentMode == MODE_SET_MIDI_CHANNEL) {
          onModeSetMidiChannel(foo);
          break;
        } else if (currentMode == MODE_PATTERN_EDIT) {
          onModePatternEdit(foo);
        } else if (currentMode == MODE_TAP) {
          onModeTap(foo);
        } else if (currentMode == MODE_RANDOM) {
          onModeRandom(foo);
        } else if (currentMode == MODE_FILL) {
          onModeFill(foo);
        } else if (currentMode == MODE_DELETE) {
          onModeDelete(foo);
        } else if (currentMode == MODE_UTIL) {
          onModeUtil(foo);
        } else if (currentMode == MODE_SELECT_TRACK || currentMode == MODE_LENGTH || currentMode == MODE_NOTES || currentMode == MODE_CLOCK) {
          onSelectTrack(foo);
        }
      }
      if (justReleased(foo)) {
        onButtonReleased(foo);
      }
    }
  }
}
void onButtonReleased(uint8_t button) {
  if (currentMode == MODE_FILL) {
    patternData[button][PATTERN_FILL] = false;
  } else if (currentMode == MODE_INVERSE) {
    patternData[button][PATTERN_INVERSE] = false;
  } else if ( currentMode == MODE_PERFORMANCE) {
    for (byte bar = 0; bar < 4; bar++) {
      byte track = (4 * performanceModeCurrentPage) + bar;
      if (button == bar + 4) {
        if (!performanceModeSettings[track]) {
          patternData[track][PATTERN_FILL] = false;
        }
      }
    }
  }
}
void onSelectTrack(uint8_t button) {
  currentTrack = button;
  if (currentMode == MODE_LENGTH) {
    setMode(MODE_LENGTH_EDIT);
  } else if (currentMode == MODE_SELECT_TRACK) {
    setMode(MODE_PATTERN_EDIT);
  } else if (currentMode == MODE_NOTES) {
    setMode(MODE_SET_MIDI_CHANNEL);
  } else if (currentMode == MODE_CLOCK) {
    setMode(MODE_SET_CLOCK_DIVIDER);
  }
}

void onModeAutoPlay(uint8_t button) {
  if (button >= 8 && button <= 15) {
    autoClockEnabled = !autoClockEnabled;
    if (!autoClockEnabled) {
      autoPlayRun = false;
      autoPlayHot = true;
    }
    enDisableAutoPlay();
  } else if (button == 0) {
    autoPlayRun = autoClockEnabled ? !autoPlayRun : false;

    onReset();
  } else if (button == 1) {
    onReset();
  }  if (button == 2) {
    if (autoPlayTempo > 40) {
      autoPlayTempo--;
      setAutoPlayTempo();
    }
  }  if (button == 3) {
    if (autoPlayTempo < 240) {
      autoPlayTempo++;
      setAutoPlayTempo();
    }
  }
}
void onModePerformance(uint8_t button) {
  for (byte bar = 0; bar < 4; bar++) {
    byte track = (4 * performanceModeCurrentPage) + bar;
    if (button == bar) {
      if (performanceModeSettings[track]) {
        if ( !getTrigger(track, patternData[track][PATTERN_CURRENT_TICK])) {
          if (patternData[track][PATTERN_CURRENT_TICK] < 8) {
            patternData[track][PATTERN_NOTES_ROW_0] += 1 << patternData[track][PATTERN_CURRENT_TICK];
          } else {
            patternData[track][PATTERN_NOTES_ROW_1] += 1 << ( patternData[track][PATTERN_CURRENT_TICK] - 8 );
          }
        }
      } else {
        if (patternData[track][PATTERN_NOTES_ROW_0] + patternData[track][PATTERN_NOTES_ROW_1] > 0) {
          patternData[track][PATTERN_MUTE] = !patternData[track][PATTERN_MUTE];
        }
      }
    }
    if (button == bar + 4) {
      if (performanceModeSettings[track]) {
        patternData[track][PATTERN_NOTES_ROW_0] = patternData[track][PATTERN_NOTES_ROW_1] = 0;
        patternData[track][PATTERN_MUTE] = true;
      } else {
        patternData[track][PATTERN_FILL] = true;
      }
    }
    if (button == bar + 8) {
      performanceModeSettings[track] = !performanceModeSettings[track];
      resetFillInvert();
    }
    if (button == bar + 12) {
      performanceModeCurrentPage = bar;
      resetFillInvert();
    }
  }
}
void onModeInverse(uint8_t button) {
  patternData[button][PATTERN_INVERSE] = !patternData[button][PATTERN_INVERSE];
}
void onModeLengthReset(uint8_t button) {
  patternData[button][PATTERN_LENGTH_RESET] = !patternData[button][PATTERN_LENGTH_RESET];
}
void onModeGateRetrigger(uint8_t button) {
  patternData[button][PATTERN_RETRIGGER] = !patternData[button][PATTERN_RETRIGGER];
}
void onModeMute(uint8_t button) {
  if (patternData[button][PATTERN_NOTES_ROW_0] + patternData[button][PATTERN_NOTES_ROW_1] > 0) {
    patternData[button][PATTERN_MUTE] = !patternData[button][PATTERN_MUTE];
  }
}
void onModeSetClockDivider(uint8_t button) {
  patternData[currentTrack][PATTERN_CLOCK] = button;
}
void onModeLengthEdit(uint8_t button) {
  patternData[currentTrack][PATTERN_LENGTH] = button;
}
void onModeSetMidiChannel(uint8_t button) {
  patternData[currentTrack][PATTERN_MIDI_CHANNEL] = button;
}
void onModePatternEdit(uint8_t button) {
  if (getTrigger(currentTrack, button)) {
    if (button < 8) {
      patternData[currentTrack][PATTERN_NOTES_ROW_0] -= 1 << button;
    } else {
      patternData[currentTrack][PATTERN_NOTES_ROW_1] -= 1 << ( button - 8 );
    }
  } else {
    if (button < 8) {
      patternData[currentTrack][PATTERN_NOTES_ROW_0] += 1 << button;
    } else {
      patternData[currentTrack][PATTERN_NOTES_ROW_1] += 1 << ( button - 8 );
    }
  }
}
void onModeTap(uint8_t button) {
  if (!getTrigger(button, patternData[button][PATTERN_CURRENT_TICK])) {
    if (patternData[button][PATTERN_CURRENT_TICK] < 8) {
      patternData[button][PATTERN_NOTES_ROW_0] += 1 << patternData[button][PATTERN_CURRENT_TICK];
    } else {
      patternData[button][PATTERN_NOTES_ROW_1] += 1 << ( patternData[button][PATTERN_CURRENT_TICK] - 8 );
    }
  }
}
void onModeRandom(uint8_t button) {
  patternData[button][PATTERN_NOTES_ROW_0] = random(0, 255);
  patternData[button][PATTERN_NOTES_ROW_1] = random(0, 255);
}
void onModeFill(uint8_t button) {
  patternData[button][PATTERN_FILL] = true;
}
void onModeDelete(uint8_t button) {
  patternData[button][PATTERN_NOTES_ROW_0] = patternData[button][PATTERN_NOTES_ROW_1] = 0;
  patternData[button][PATTERN_MUTE] = true;
}
void onModeUtil(uint8_t button) {
  if (button == 0) {
    disableAnimations = !disableAnimations;
  } else if (button == 1) {
    autoReset = !autoReset;
  } else if (button == 3) {
    factoryReset();
  }
}
