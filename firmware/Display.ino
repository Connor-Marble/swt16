void updateLightIsHigh() {
        lightIsHigh = (millis() - onClockHighTime < ((onClockLowTime - onClockHighTime) >> 1));
}
boolean playAnimation(byte animation[]) {
        if (disableAnimations) {
                for (byte foo = 0; foo < 16; foo++) {
                        setLedValue(foo, false);
                }
                for (byte foo = 0; foo < 10; foo++) {
                        if (animation[foo] < 16) {
                                setLedValue(animation[foo], true);
                        }
                }
                writeDisplay();
                delay(1000);
        } else {
                for (byte foo = 0; foo < 16; foo++) {
                        setLedValue(foo, false);
                        writeDisplay();
                        delay(20);
                }
                for (byte foo = 0; foo < 10; foo++) {
                        if (animation[foo] < 16) {
                                setLedValue(animation[foo], true);
                                writeDisplay();
                                delay(30);
                        }
                }
                delay(200);
                for (byte foo = 0; foo < 10; foo++) {
                        setLedValue(animation[foo], false);
                        writeDisplay();
                        delay(30);
                }
                delay(20);
        }
        return true;
}
byte pulseLight = 0;
void updateDisplay() {
        updateLightIsHigh();
        pulseLight %= 5;
        pulseLight++;
        if (isIdle) {
                idleTick %= 120;

                byte currentChar = idleTick % 20;
                byte currentAnimation = 18 + (idleTick / 20);

                setLedValue(animations[currentAnimation][currentChar % 10], currentChar < 10);
                idleTick += (animations[(18 + ((idleTick + 1) / 20))][((idleTick + 1) % 20) % 10] >= 16) ? 10 - idleTick % 10 : 1;
                writeDisplay();
        } else {
                idleTick = 0;
                for (uint8_t foo = 0; foo < 16; foo++) {
                        if (currentMode == MODE_HOME) {
                                setLedValue(foo, true);
                        } else if ( currentMode == MODE_PERFORMANCE) {
                                boolean lightUp = false;
                                for (byte bar = 0; bar < 4; bar++) {
                                        byte track = (4 * performanceModeCurrentPage) + bar;
                                        lightUp = (foo == bar && performanceModeSettings[track] && getTrigger(track, patternData[track][PATTERN_CURRENT_TICK], patternData[track][PATTERN_LENGTH])) || (foo == bar && !performanceModeSettings[track] && patternData[track][PATTERN_MUTE] && !(lightIsHigh && getTrigger(track,
                                                                                                                                                                                                                                                                                                                         patternData[track][PATTERN_CURRENT_TICK], patternData[track][PATTERN_LENGTH]) || (patternData[track][PATTERN_BAR_0] == 0 && patternData[track][PATTERN_BAR_1] == 0 )  )) || ((foo == bar + 4) && !performanceModeSettings[track] && (lightIsHigh &&  patternData[track][PATTERN_FILL] )) || ((foo == bar + 8) && performanceModeSettings[track]) || ((foo == bar + 12) && bar == performanceModeCurrentPage) ? true : lightUp;
                                }
                                setLedValue(foo, lightUp);
                        } else if (currentMode == MODE_AUTOPLAY) {
                                setLedValue(foo, (foo == 2  && autoClockEnabled && autoPlayTick % 4 == 0) || (foo == 3  && autoClockEnabled && autoPlayTick % 2 == 0) || ((foo == 8 || foo == 9 || foo == 12 || foo == 13) && autoClockEnabled) || ((foo == 10 || foo == 11 || foo == 14 || foo == 15) && !autoClockEnabled) || ((foo == 0 && autoClockEnabled) && autoPlayRun && autoPlayHot));
                        } else if (currentMode == MODE_MUTE) {
                                setLedValue(foo, patternData[foo][PATTERN_MUTE] && !(lightIsHigh && getTrigger(foo, patternData[foo][PATTERN_CURRENT_TICK], patternData[foo][PATTERN_LENGTH]) || (patternData[foo][PATTERN_BAR_0] == 0 && patternData[foo][PATTERN_BAR_1] == 0 )));
                        } else if (currentMode == MODE_FILL) {
                                setLedValue(foo, patternData[foo][MODE_FILL] && !(lightIsHigh && getTrigger(foo, patternData[foo][PATTERN_CURRENT_TICK], patternData[foo][PATTERN_LENGTH]) || (patternData[foo][PATTERN_BAR_0] == 0 && patternData[foo][PATTERN_BAR_1] == 0 )));
                        }  else if (currentMode == MODE_UTIL) {
                                setLedValue(foo, (foo == 0 && disableAnimations) || (foo == 1 && autoReset) || (foo == 2 && flipMode));
                        } else if (currentMode == MODE_LENGTH_EDIT) {
                                setLedValue(foo, foo == patternData[currentTrack][PATTERN_LENGTH] || foo == patternData[currentTrack][PATTERN_BAR_LENGTH] + 8);
                        } else if (currentMode == MODE_SET_MIDI_CHANNEL) {
                                setLedValue(foo, foo == patternData[currentTrack][PATTERN_MIDI_CHANNEL]);
                        } else if (currentMode == MODE_SET_CLOCK_DIVIDER) {
                                setLedValue(foo, foo == patternData[currentTrack][PATTERN_CLOCK]);
                        } else if (currentMode == MODE_PATTERN_EDIT) {
                                setLedValue(foo, (patternData[currentTrack][PATTERN_BAR_LENGTH] == 1 && patternData[currentTrack][PATTERN_LENGTH] == 7) ? (getTrigger(currentTrack, foo, patternData[currentTrack][PATTERN_LENGTH]) && (tick % 16) != foo) || (!getTrigger(currentTrack, foo, patternData[currentTrack][PATTERN_LENGTH]) && (tick % 16) == foo) : foo < 8 ? patternData[currentTrack][PATTERN_BAR_SELECTED + patternData[currentTrack][PATTERN_BAR_SELECTED] + 1] & (1 << foo) ? true : false : foo >= 8 ? foo == patternData[currentTrack][PATTERN_BAR_SELECTED] + 8 ? (pulseLight > 1) : (copyPatternMode < 8 && foo == copyPatternMode + 8) ? false : foo < 9 + patternData[currentTrack][PATTERN_BAR_LENGTH] : false);

                        } else if (currentMode == MODE_LENGTH_RESET || currentMode == MODE_GATE_RETRIGGER) {
                                setLedValue(foo, patternData[foo][currentMode == MODE_GATE_RETRIGGER ? PATTERN_RETRIGGER : currentMode == MODE_LENGTH_RESET ? PATTERN_LENGTH_RESET :  0]);
                        } else {
                                setLedValue(foo, lightIsHigh && getTrigger(foo, patternData[foo][PATTERN_CURRENT_TICK], patternData[foo][PATTERN_LENGTH]));
                        }
                        if (currentMode == MODE_HOME || currentMode == MODE_SELECT_TRACK || currentMode == MODE_LENGTH || currentMode == MODE_NOTES || currentMode == MODE_CLOCK) {
                                setLedValue(foo, (tick % 16) != foo);
                        }
                }
                writeDisplay();
        }
}
