#include <Arduino.h>
#include "esp_system.h"
#include "pins.h"
#include "menu.h"
#include "variables.h"
#include "motion.h"
#include "display.h"



// Button interrupt handlers
// void IRAM_ATTR handleButtonUp() { button_up_pressed = true; }
// void IRAM_ATTR handleButtonDown() { button_down_pressed = true; }
// void IRAM_ATTR handleButtonSelect() { button_select_pressed = true; }

// Button interrupt handlers with debouncing
void IRAM_ATTR handleButtonUp() {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonUpTime > DEBOUNCE_DELAY) {
        button_up_pressed = true;
        lastButtonUpTime = currentTime;
    }
}

void IRAM_ATTR handleButtonDown() {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonDownTime > DEBOUNCE_DELAY) {
        button_down_pressed = true;
        lastButtonDownTime = currentTime;
    }
}

void IRAM_ATTR handleButtonSelect() {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonSelectTime > DEBOUNCE_DELAY) {
        button_select_pressed = true;
        lastButtonSelectTime = currentTime;
    }
}


// Encoder ISR
void IRAM_ATTR leftEncoderISR() {
    left_encoder_count++;
}

void IRAM_ATTR rightEncoderISR() {
    right_encoder_count++;
}




void setup() {
    pinMode(LEFT_ENCODER_PIN, INPUT);
    pinMode(RIGHT_ENCODER_PIN, INPUT);

    pinMode(IR_Front, OUTPUT);
    pinMode(IR_Left, OUTPUT);
    pinMode(IR_Right, OUTPUT);

    pinMode(IR_Front_Read, INPUT);
    pinMode(IR_Left_Read, INPUT);
    pinMode(IR_Right_Read, INPUT);

    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_SELECT, INPUT_PULLUP);
    pinMode(BUTTON_SELECT2, INPUT_PULLUP);



     // Attach interrupts for buttons
    attachInterrupt(digitalPinToInterrupt(BUTTON_UP), handleButtonUp, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN), handleButtonDown, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_SELECT), handleButtonSelect, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_SELECT2), handleButtonSelect, FALLING);

    attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_PIN), leftEncoderISR, RISING);
    attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_PIN), rightEncoderISR, RISING);

    Serial.begin(115200);

    

    // Initialize the display
    display.init();
    display.flipScreenVertically(); 
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Menu Ready!");
    display.display();

    ledcSetup(LEFT_FORWARD_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(LEFT_BACKWARD_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(RIGHT_FORWARD_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(RIGHT_BACKWARD_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);

    ledcAttachPin(LEFT_FORWARD, LEFT_FORWARD_CHANNEL);
    ledcAttachPin(LEFT_BACKWARD, LEFT_BACKWARD_CHANNEL);
    ledcAttachPin(RIGHT_FORWARD, RIGHT_FORWARD_CHANNEL);
    ledcAttachPin(RIGHT_BACKWARD, RIGHT_BACKWARD_CHANNEL);

    
    digitalWrite(IR_Front, HIGH);
    digitalWrite(IR_Left, HIGH);
    digitalWrite(IR_Right, HIGH);
    delay(2000);
}

void loop() {

    if (adjusting_kp) {
        handleKpSubMenu();
    } else if (adjusting_kd) {
        handleKdSubMenu();
    } else if (in_turns_submenu) {
        handleTurnsSubMenu();
    } else if (in_submenu) {
        handleSubMenu();
    } else {
        drawMenu();
        handleMenuNavigation();
    }
}
