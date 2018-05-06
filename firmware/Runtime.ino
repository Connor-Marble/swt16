void setup() {
        Serial.begin(31250);
        randomSeed(analogRead(4));

        pinMode(TRIGGER_OUT_LATCH, OUTPUT);
        digitalWrite(TRIGGER_OUT_LATCH, 0);
        writeTriggers(TRIGGER_OUT_DATA, TRIGGER_OUT_CLOCK, 0);
        writeTriggers(TRIGGER_OUT_DATA, TRIGGER_OUT_CLOCK, 0);
        digitalWrite(TRIGGER_OUT_LATCH, 1);

        pinMode(STATUS_LED, OUTPUT);
        pinMode(HOME_BUTTON, INPUT);

        setupButtonBoard();
        loadData();

        delay(200);

        initSequencer();
}

void startDebugMode(){
        debugMode=true;
}

void stopDebugMode(){
        debugMode=false;
}
void onDebugMode(){
        delay(30);
        debugTick=!debugTick;
        activeLeds[0]=activeLeds[1]=triggerOutputData[0]=triggerOutputData[1]=0;
        if(readSwitches()) {
                for (uint8_t foo = 0; foo < 16; foo++) {
                        if (justPressed(foo)) {
                                setLedValue(foo,true);
                                triggerOutputData[0]+=triggerOutputs[0][foo];
                                triggerOutputData[1]+=triggerOutputs[1][foo];
                        }
                        if (justReleased(foo)) {
                                setLedValue(foo,false);
                        }
                }
                writeDisplay();
                digitalWrite(TRIGGER_OUT_LATCH, 0);
                writeTriggers(TRIGGER_OUT_DATA, TRIGGER_OUT_CLOCK, triggerOutputData[0]);
                writeTriggers(TRIGGER_OUT_DATA, TRIGGER_OUT_CLOCK, triggerOutputData[1]);
                digitalWrite(TRIGGER_OUT_LATCH, 1);
                digitalWrite(STATUS_LED, debugTick);
                if (digitalRead(HOME_BUTTON) == HIGH) {
                        stopDebugMode();
                }
        }
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
        pinMode(INTERRUPT_CLOCK, INPUT_PULLUP);
        pinMode(INTERRUPT_RESET, INPUT_PULLUP);
        enDisableAutoPlay();
        setMode(MODE_HOME);
        tick = 0;
        onReset();
}

void checkIdle() {
        wasIdle = isIdle;
        isIdle = millis() > 60000 + onClockHighTime;
        if (wasIdle != isIdle) {
                for (byte foo = 0; foo < 16; foo++) {
                        setLedValue(foo, false);
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
        if(debugMode) {
                onDebugMode();
        }else{
                checkIdle();
                delay(isIdle ? 120 : 30);
                checkButtons();
                updateDisplay();
                if (digitalRead(HOME_BUTTON) == HIGH && (currentMode != MODE_HOME || isIdle)) {
                        setMode(MODE_HOME);
                }
        }
}
