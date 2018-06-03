/**
 * Robaux SWT16 Firmware Version 1.1.0
 * by Robert Pøul Menzel
 * musique@robaux.co
 */
#include <EEPROM.h>
#include <Wire.h>
#include "TimerOne.h"

#define VERSION 111

#define INTERRUPT_CLOCK  2
#define INTERRUPT_RESET  3

#define HOME_BUTTON  5
#define STATUS_LED  4

#define BUTTON_LATCH_PIN 11
#define BUTTON_DATA_PIN 9
#define BUTTON_CLOCK_PIN 10

#define LEDS_LATCH_PIN A2
#define LEDS_CLOCK_PIN A3
#define LEDS_DATA_PIN A0

#define TRIGGER_OUT_LATCH  7
#define TRIGGER_OUT_CLOCK  8
#define TRIGGER_OUT_DATA  6

#define MODE_SELECT_TRACK 0
#define MODE_TAP 1
#define MODE_RANDOM 2
#define MODE_DELETE 3
#define MODE_MUTE 4
#define MODE_FILL 5
#define MODE_INVERSE 6
#define MODE_PERFORMANCE 7
#define MODE_LENGTH 8
#define MODE_LENGTH_RESET 9
#define MODE_GATE_RETRIGGER 10
#define MODE_CLOCK 11
#define MODE_AUTOPLAY 12
#define MODE_NOTES 13
#define MODE_WRITE 14
#define MODE_UTIL 15
#define MODE_HOME 16
#define MODE_PATTERN_EDIT 17
#define MODE_LENGTH_EDIT 18
#define MODE_SET_CLOCK_DIVIDER 19
#define MODE_SET_MIDI_CHANNEL 20

#define PATTERN_MUTE 0
#define PATTERN_FILL 1
#define PATTERN_CURRENT_TICK 2
#define PATTERN_RETRIGGER 3
#define PATTERN_MIDI_CHANNEL 4
#define PATTERN_LENGTH 5
#define PATTERN_LENGTH_RESET 6
#define PATTERN_CLOCK 7
#define PATTERN_MIDI_NOTE_ON 8
#define PATTERN_INVERSE 9
#define PATTERN_BAR_LENGTH 10
#define PATTERN_BAR_SELECTED 11
#define PATTERN_BAR_0 12
#define PATTERN_BAR_1 13
#define PATTERN_BAR_2 14
#define PATTERN_BAR_3 15
#define PATTERN_BAR_4 16
#define PATTERN_BAR_5 17
#define PATTERN_BAR_6 18
#define PATTERN_BAR_7 19

unsigned long onClockHighTime = 0;
unsigned long onClockLowTime = 0;
boolean lightIsHigh = false;

boolean debugMode=false;
boolean debugTick=false;

boolean isIdle = false;
boolean wasIdle = false;
int idleTick = 0;
byte copyPatternMode = 8;
byte autoPlayHot = false;
byte autoPlayTick = 0;
byte autoPlayRun = false;
byte flipMode = false;
long autoPlayTempo = 120;

byte autoClockEnabled = 0;
byte disableAnimations = 0;
byte autoReset = 0;

boolean performanceModeSettings[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte performanceModeCurrentPage = 0;
byte currentTrack = 0;
byte tick = 0;
boolean clockIsHigh = false;
byte currentMode;
boolean buttonsEnabled = false;
byte clock = 0;

byte patternData[16][20] = {
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 1, 9, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

byte triggerOutputData[2] = {0, 0};
byte triggerOutputs[2][16] = {
        {0, 0, 1, 16, 0, 0, 2, 128, 0, 0, 4, 64, 0, 0, 8, 32}, {128, 8, 0, 0, 64, 4, 0, 0, 32, 2, 0, 0, 16, 1, 0, 0}
};

byte animations[24][10] = {
        /* STEP         */ {3, 2, 1, 4, 5, 10, 11, 14, 13, 12},
        /* TAP          */ {0, 1, 5, 9, 13, 14, 10, 6, 2, 3},
        /* RANDOM       */ {15, 10, 7, 2, 1, 0, 4, 8, 9, 12},
        /* DELETE       */ {12, 8, 4, 0, 1, 2, 7, 11, 14, 13},
        /* MUTE         */ {12, 8, 4, 0, 5, 6, 3, 7, 11, 15},
        /* FILL         */ {10, 9, 12, 8, 4, 0, 1, 2, 3, 69},
        /* INVERSE      */ { 1, 5, 9, 13, 14, 10, 6, 2, 69, 69},
        /* PERFORM      */ {9, 10, 7, 2, 1, 0, 4, 8, 12, 69},
        /* LENGTH       */ {0, 4, 8, 12, 13, 14, 15, 69, 69, 69},
        /* LENGTH RESET */ {0, 4, 8, 12, 13, 14, 15, 7, 3, 2},
        /* GATE         */ {11, 15, 14, 13, 8, 4, 1, 2, 3, 69},
        /* CLOCK        */ {15, 14, 13, 8, 4, 1, 2, 3, 69, 69},
        /* AUTOPLAY     */ {12, 8, 9, 4, 1, 2, 7, 10, 11, 15},
        /* NOTES        */ {12, 8, 4, 0, 5, 10, 15, 11, 7, 3},
        /* WRITE        */ {0, 4, 8, 12, 9, 10, 15, 11, 7, 3},
        /* UTIL         */ {0, 4, 8, 13, 14, 11, 7, 3, 69, 69},
        /* HOME         */ {13, 9, 5, 4, 1, 2, 7, 6, 10, 14},
        /* INSIDE       */ {8, 4, 5, 1, 2, 7, 11, 10, 14, 13},
        /* r            */ {7, 6, 5, 8, 12, 69, 69, 69, 69, 69},
        /* o            */ {13, 14, 11, 6, 5, 8, 69, 69, 69, 69},
        /* b            */ {0, 4, 8, 12, 13, 14, 11, 6, 5, 69},
        /* a            */ {8, 13, 14, 15, 11, 7, 6, 5, 69, 69},
        /* u            */ {4, 8, 13, 14, 11, 7, 69, 69, 69, 69},
        /* x            */ {4, 9, 12, 7, 10, 15, 69, 69, 69, 69}
};
