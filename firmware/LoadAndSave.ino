void saveData() {
        EEPROM.write(0, VERSION);
        EEPROM.write(1, autoClockEnabled);
        EEPROM.write(2, disableAnimations);
        EEPROM.write(3, autoReset);
        EEPROM.write(4, autoPlayTempo);
        EEPROM.write(5, flipMode);
        for (byte foo = 0; foo < 16; foo++) {
                EEPROM.write((foo * 12) + 6, steps2num(patternData[foo][PATTERN_LENGTH]) + (patternData[foo][PATTERN_LENGTH_RESET] * 128));
                EEPROM.write((foo * 12) + 7, steps2num(patternData[foo][PATTERN_MIDI_CHANNEL]) + (patternData[foo][PATTERN_RETRIGGER] * 128));
                EEPROM.write((foo * 12) + 8, steps2num(patternData[foo][PATTERN_CLOCK]));
                EEPROM.write((foo * 12) + 9, patternData[foo][PATTERN_BAR_LENGTH]);
                EEPROM.write((foo * 12) + 10, patternData[foo][PATTERN_BAR_0]);
                EEPROM.write((foo * 12) + 11, patternData[foo][PATTERN_BAR_1]);
                EEPROM.write((foo * 12) + 12, patternData[foo][PATTERN_BAR_2]);
                EEPROM.write((foo * 12) + 13, patternData[foo][PATTERN_BAR_3]);
                EEPROM.write((foo * 12) + 14, patternData[foo][PATTERN_BAR_4]);
                EEPROM.write((foo * 12) + 15, patternData[foo][PATTERN_BAR_5]);
                EEPROM.write((foo * 12) + 16, patternData[foo][PATTERN_BAR_6]);
                EEPROM.write((foo * 12) + 17, patternData[foo][PATTERN_BAR_7]);
        }
}
void factoryReset() {
        autoClockEnabled = false;
        enDisableAutoPlay();
        autoPlayRun = false;
        disableAnimations = 0;
        autoReset = 1;
        autoPlayTempo = 120;
        flipMode = false;
        for (byte foo = 0; foo < 16; foo++) {
                patternData[foo][PATTERN_LENGTH] = 7;
                patternData[foo][PATTERN_LENGTH_RESET] = 1;
                patternData[foo][PATTERN_MIDI_CHANNEL] = 9;
                patternData[foo][PATTERN_RETRIGGER] = 1;
                patternData[foo][PATTERN_CLOCK] = 0;
                patternData[foo][PATTERN_BAR_LENGTH] = 1;
                patternData[foo][PATTERN_BAR_0] = foo == 0 ? 17 : foo == 1 ? 16 : foo == 2 ? 68 : foo == 12 ? 1 : foo == 13 ? 1 : foo == 14 ? 17 : foo == 15 ? 255 : 0;
                patternData[foo][PATTERN_BAR_1] = foo == 0 ? 17 : foo == 1 ? 16 : foo == 2 ? 68 : foo == 3 ? 128 : foo == 13 ? 1 : foo == 14 ? 17 : foo == 15 ? 255 : 0;
                patternData[foo][PATTERN_BAR_2] = 0;
                patternData[foo][PATTERN_BAR_3] = 0;
                patternData[foo][PATTERN_BAR_4] = 0;
                patternData[foo][PATTERN_BAR_5] = 0;
                patternData[foo][PATTERN_BAR_6] = 0;
                patternData[foo][PATTERN_BAR_7] = 0;
        }
        onReset();
        saveData();
}


void loadData() {
        if (EEPROM.read(0) == VERSION) {
                autoClockEnabled = EEPROM.read(1);
                disableAnimations = EEPROM.read(2);
                autoReset = EEPROM.read(3);
                autoPlayTempo = EEPROM.read(4);
                flipMode = EEPROM.read(5);
                for (byte foo = 0; foo < 16; foo++) {

                        byte val = EEPROM.read((foo * 12) + 6);
                        patternData[foo][PATTERN_LENGTH] = num2steps(val);
                        patternData[foo][PATTERN_LENGTH_RESET] = val & 1 << 7 ? 1 : 0;

                        val = EEPROM.read((foo * 12) + 7);
                        patternData[foo][PATTERN_MIDI_CHANNEL] = num2steps(val);
                        patternData[foo][PATTERN_RETRIGGER] = val & 1 << 7 ? 1 : 0;

                        val = EEPROM.read((foo * 12) + 8);
                        patternData[foo][PATTERN_CLOCK] = num2steps(val);

                        patternData[foo][PATTERN_BAR_LENGTH] = EEPROM.read((foo * 12) + 9);

                        patternData[foo][PATTERN_BAR_0] = EEPROM.read((foo * 12) + 10);
                        patternData[foo][PATTERN_BAR_1] = EEPROM.read((foo * 12) + 11);
                        patternData[foo][PATTERN_BAR_2] = EEPROM.read((foo * 12) + 12);
                        patternData[foo][PATTERN_BAR_3] = EEPROM.read((foo * 12) + 13);
                        patternData[foo][PATTERN_BAR_4] = EEPROM.read((foo * 12) + 14);
                        patternData[foo][PATTERN_BAR_5] = EEPROM.read((foo * 12) + 15);
                        patternData[foo][PATTERN_BAR_6] = EEPROM.read((foo * 12) + 16);
                        patternData[foo][PATTERN_BAR_7] = EEPROM.read((foo * 12) + 17);
                }
        } else {
                factoryReset();
        }
}

byte steps2num(byte s) {
        return ((((s / 4) >> 0) % 2) << 1) + ((((s / 8) >> 0) % 2) << 2) + (1 << (((((s * 100) / 4) - (((s / 4) >> 0) * 100)) / 25) + 3));
}

byte num2steps(byte n) {
        return (n & (1 << 1) ? 4 : 0) + (n & (1 << 2) ? 8 : 0) + (n & (1 << 3) ? 1 : 0) + (n & (1 << 4) ? 2 : 0) + (n & (1 << 5) ? 3 : 0) + (n & (1 << 6) ? 4 : 0) - 1;;
}
