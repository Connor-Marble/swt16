
void saveData() {
  EEPROM.write(0, VERSION);
  EEPROM.write(1, autoClockEnabled);
  EEPROM.write(2, disableAnimations);
  EEPROM.write(3, autoReset);
  EEPROM.write(4, autoPlayTempo);
  for (byte foo = 0; foo < 16; foo++) {
    EEPROM.write((foo * 5) + 5, steps2num(patternData[foo][PATTERN_LENGTH]) + (patternData[foo][PATTERN_LENGTH_RESET] * 128));
    EEPROM.write((foo * 5) + 6, steps2num(patternData[foo][PATTERN_MIDI_CHANNEL]) + (patternData[foo][PATTERN_RETRIGGER] * 128));
    EEPROM.write((foo * 5) + 7, steps2num(patternData[foo][PATTERN_CLOCK]));
    EEPROM.write((foo * 5) + 8, patternData[foo][PATTERN_NOTES_ROW_0]);
    EEPROM.write((foo * 5) + 9, patternData[foo][PATTERN_NOTES_ROW_1]);
  }
}
void factoryReset() {
  autoClockEnabled = false;
  enDisableAutoPlay();
  autoPlayRun = false;
  disableAnimations = 0;
  autoReset = 1;
  autoPlayTempo = 120;
  for (byte foo = 0; foo < 16; foo++) {
    patternData[foo][PATTERN_LENGTH] = 15;
    patternData[foo][PATTERN_LENGTH_RESET] = 1;
    patternData[foo][PATTERN_MIDI_CHANNEL] = 9;
    patternData[foo][PATTERN_RETRIGGER] = 1;
    patternData[foo][PATTERN_CLOCK] = 0;
    patternData[foo][PATTERN_NOTES_ROW_0] = foo == 0 ? 17 : foo == 1 ? 16 : foo == 2 ? 68 : foo == 12 ? 1 : foo == 13 ? 1 : foo == 14 ? 17 : foo == 15 ? 255 : 0;
    patternData[foo][PATTERN_NOTES_ROW_1] = foo == 0 ? 17 : foo == 1 ? 16 : foo == 2 ? 68 : foo == 3 ? 128 : foo == 13 ? 1 : foo == 14 ? 17 : foo == 15 ? 255 : 0;
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
    for (byte foo = 0; foo < 16; foo++) {
      byte val = EEPROM.read((foo * 5) + 5);
      patternData[foo][PATTERN_LENGTH] = num2steps(val);
      patternData[foo][PATTERN_LENGTH_RESET] = val & 1 << 7 ? 1 : 0;

      val = EEPROM.read((foo * 5) + 6);
      patternData[foo][PATTERN_MIDI_CHANNEL] = num2steps(val);
      patternData[foo][PATTERN_RETRIGGER] = val & 1 << 7 ? 1 : 0;

      val = EEPROM.read((foo * 5) + 7);
      patternData[foo][PATTERN_CLOCK] = num2steps(val);

      patternData[foo][PATTERN_NOTES_ROW_0] = EEPROM.read((foo * 5) + 8);
      patternData[foo][PATTERN_NOTES_ROW_1] = EEPROM.read((foo * 5) + 9);
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
