


void resetFillInvert() {
        for (byte foo = 0; foo < 16; foo++) {
                patternData[foo][PATTERN_FILL] = false;
                patternData[foo][PATTERN_INVERSE] = false;
        }
}
void checkButtons() {
        if (buttonsEnabled && readSwitches()) {
                for (uint8_t foo = 0; foo < 16; foo++) {
                        if (justPressed(flipMode ? 15 - foo : foo)) {
                                resetIdle();
                                if (currentMode == MODE_HOME) {
                                        setMode(foo);
                                        break;
                                } else if (currentMode == MODE_AUTOPLAY) {
                                        onModeAutoPlay(foo);
                                } else if (currentMode == MODE_PERFORMANCE) {
                                        onModePerformance(foo);
                                } else if (currentMode == MODE_LENGTH_RESET) {
                                        onModeLengthReset(foo);
                                } else if (currentMode == MODE_INVERSE) {
                                        onModeInverse(foo);
                                }  else if (currentMode == MODE_GATE_RETRIGGER) {
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
                        if (justReleased(flipMode ? 15 - foo : foo)) {
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
        } else if (currentMode == MODE_PATTERN_EDIT) {
                copyPatternMode = 8;
        }
}
void onSelectTrack(uint8_t button) {
        currentTrack = button;
        if (currentMode == MODE_LENGTH) {
                setMode(MODE_LENGTH_EDIT);
        } else if (currentMode == MODE_SELECT_TRACK) {
                for (byte foo = 0; foo < 16; foo++) {
                        patternData[foo][PATTERN_BAR_SELECTED] = 0;
                }
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
                                onModeTap(track);
                        } else {
                                onModeMute(track);
                        }
                }
                if (button == bar + 4) {
                        if (performanceModeSettings[track]) {
                                onModeDelete(track);
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

void onModeLengthReset(uint8_t button) {
        patternData[button][PATTERN_LENGTH_RESET] = !patternData[button][PATTERN_LENGTH_RESET];
}
void onModeGateRetrigger(uint8_t button) {
        patternData[button][PATTERN_RETRIGGER] = !patternData[button][PATTERN_RETRIGGER];
}
void onModeMute(uint8_t button) {
        if (patternData[button][PATTERN_BAR_0] + patternData[button][PATTERN_BAR_1] + patternData[button][PATTERN_BAR_2] + patternData[button][PATTERN_BAR_3] + patternData[button][PATTERN_BAR_4] + patternData[button][PATTERN_BAR_5] + patternData[button][PATTERN_BAR_6] + patternData[button][PATTERN_BAR_7] > 0) {
                patternData[button][PATTERN_MUTE] = !patternData[button][PATTERN_MUTE];
        }
}
void onModeSetClockDivider(uint8_t button) {
        patternData[currentTrack][PATTERN_CLOCK] = button;
}

void onModeLengthEdit(uint8_t button) {
        if (button < 8) {
                patternData[currentTrack][PATTERN_LENGTH] = button;
        } else {
                patternData[currentTrack][PATTERN_BAR_LENGTH] = button - 8;
        }
}
void onModeSetMidiChannel(uint8_t button) {
        patternData[currentTrack][PATTERN_MIDI_CHANNEL] = button;
}
void onModePatternEdit(uint8_t button) {
        if (patternData[currentTrack][PATTERN_BAR_LENGTH] == 1 && patternData[currentTrack][PATTERN_LENGTH] == 7) {
                if (getTrigger(currentTrack, button, patternData[currentTrack][PATTERN_LENGTH])) {
                        if (button < 8) {
                                patternData[currentTrack][PATTERN_BAR_0] -= 1 << button;
                        } else {
                                patternData[currentTrack][PATTERN_BAR_1] -= 1 << ( button - 8 );
                        }
                } else {
                        if (button < 8) {
                                patternData[currentTrack][PATTERN_BAR_0] += 1 << button;
                        } else {
                                patternData[currentTrack][PATTERN_BAR_1] += 1 << ( button - 8 );
                        }
                }
        } else {
                if (button >= 8) {
                        if (copyPatternMode < 8) {
                                patternData[currentTrack][PATTERN_BAR_SELECTED + 1 + (button - 8)] = patternData[currentTrack][PATTERN_BAR_SELECTED + 1 + copyPatternMode];
                                copyPatternMode = 8;
                        } else {
                                patternData[currentTrack][PATTERN_BAR_SELECTED] = button - 8;
                                copyPatternMode = button - 8;
                        }
                } else {
                        if (patternData[currentTrack][PATTERN_BAR_SELECTED + patternData[currentTrack][PATTERN_BAR_SELECTED] + 1] & (1 << button)) {
                                patternData[currentTrack][PATTERN_BAR_SELECTED + patternData[currentTrack][PATTERN_BAR_SELECTED] + 1] -= 1 << button;
                        } else {
                                patternData[currentTrack][PATTERN_BAR_SELECTED + patternData[currentTrack][PATTERN_BAR_SELECTED] + 1] += 1 << button;
                        }
                }
        }
}

void onModeTap(uint8_t button) {
        if (!getTrigger(button, patternData[button][PATTERN_CURRENT_TICK], patternData[button][PATTERN_LENGTH])) {
                patternData[button][PATTERN_BAR_SELECTED + 1 + (patternData[button][PATTERN_CURRENT_TICK] / 8)] += 1 << (patternData[button][PATTERN_CURRENT_TICK] % 8);
        }
}
void onModeInverse(uint8_t button) {
        patternData[button][PATTERN_INVERSE] = !patternData[button][PATTERN_INVERSE];
}
void onModeRandom(uint8_t button) {
        patternData[button][PATTERN_BAR_0] = random(0, 255);
        patternData[button][PATTERN_BAR_1] = random(0, 255);
        patternData[button][PATTERN_BAR_2] = random(0, 255);
        patternData[button][PATTERN_BAR_3] = random(0, 255);
        patternData[button][PATTERN_BAR_4] = random(0, 255);
        patternData[button][PATTERN_BAR_5] = random(0, 255);
        patternData[button][PATTERN_BAR_6] = random(0, 255);
        patternData[button][PATTERN_BAR_7] = random(0, 255);
}
void onModeFill(uint8_t button) {
        patternData[button][PATTERN_FILL] = true;
}
void onModeDelete(uint8_t button) {
        patternData[button][PATTERN_BAR_0] = patternData[button][PATTERN_BAR_1] = patternData[button][PATTERN_BAR_2] = patternData[button][PATTERN_BAR_3] = patternData[button][PATTERN_BAR_4] = patternData[button][PATTERN_BAR_5] = patternData[button][PATTERN_BAR_6] = patternData[button][PATTERN_BAR_7] = 0;
        patternData[button][PATTERN_MUTE] = true;
}
void onModeUtil(uint8_t button) {
        if (button == 0) {
                disableAnimations = !disableAnimations;
        } else if (button == 1) {
                autoReset = !autoReset;
        } else if (button == 2) {
                flipMode = !flipMode;
        } else if (button == 7) {
                factoryReset();
        }else if(button==12) {
                startDebugMode();
        }
}
