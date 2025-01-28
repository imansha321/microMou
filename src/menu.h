
#ifndef MENU_H
#define MENU_H


#include "motion.h"
#include "display.h"
#include "variables.h"

#define DEBOUNCE_DELAY 200  

// Variables to track button press times
unsigned long lastButtonUpTime = 0;
unsigned long lastButtonDownTime = 0;
unsigned long lastButtonSelectTime = 0;

// Menu items
const char* menu_items[] = {"Start", "Kp Adjust", "Kd Adjust", "Encoders", "Turns"};
int selected_index = 0;
int kp_selected_index = 0;
int kd_selected_index = 0;

bool adjusting_kp = false;
bool adjusting_kd = false;

//Turns submenu
const char* turns_submenu_items[] = {"Turn 90° Left", "Turn 90° Right", "Back"};
int turns_selected_index = 0;
bool in_turns_submenu = false;

const char* kp_items[] = {"KpL", "KpR", "KpIR", "KpE","Back"};
const char* kd_items[] = {"KdL", "KdR", "KdIR", "KdE","Back"};


volatile bool button_up_pressed = false;
volatile bool button_down_pressed = false;
volatile bool button_select_pressed = false;

void handleTurnsSubMenu() {
    if (button_up_pressed) {
        button_up_pressed = false;
        turns_selected_index = (turns_selected_index > 0) ? turns_selected_index - 1 : 2;
    }

    if (button_down_pressed) {
        button_down_pressed = false;
        turns_selected_index = (turns_selected_index < 2) ? turns_selected_index + 1 : 0;
    }

    if (button_select_pressed) {
        button_select_pressed = false;

        // Handle selection
        switch (turns_selected_index) {
            case 0: // Turn Left
                turnLeft90Degrees();
                break;
            case 1: // Turn Right
                turnRight90Degrees();
                break;
            case 2: // Back to main menu
                in_turns_submenu = false;
                in_submenu = false;
                break;
        }
    }

    // Display Turns submenu
    display.clear();
    for (int i = 0; i < 3; i++) {
        if (i == turns_selected_index) {
            display.setColor(WHITE);
            display.fillRect(0, i * 12, 128, 12); // Highlight selected
            display.setColor(BLACK);
        } else {
            display.setColor(WHITE);
        }
        display.drawString(5, i * 12, turns_submenu_items[i]);
    }
    display.display();
}

void drawMenuKpKd(const char* items[], int item_count, int current_index) {
    display.clear();
    for (int i = 0; i < item_count; i++) {
        if (i == current_index) {
            display.setColor(WHITE);
            display.fillRect(0, i * 12, 128, 12); // Highlight selected
            display.setColor(BLACK);
        } else {
            display.setColor(WHITE);
        }
        display.drawString(5, i * 12, items[i]);
    }
    display.display();
}


float& getKpReference(int index) {
    switch (index) {
        case 0: return KpL;
        case 1: return KpR;
        case 2: return KpIR;
        case 3: return KpE;
        default: return KpL; // Default to avoid issues
    }
}

float& getKdReference(int index) {
    switch (index) {
        case 0: return KdL;
        case 1: return KdR;
        case 2: return KdIR;
        case 3: return KdE;
        default: return KdL; // Default to avoid issues
    }
}


void adjustValue(const char* label, float& value) {
    while (true) {
        display.clear();
        display.drawString(0, 0, String(label) + " Adjust");
        display.drawString(0, 20, "Value:");
        display.drawString(60, 20, String(value, 2)); // Ensure two decimal places are shown
        display.drawString(0, 40, "UP/DOWN to adjust");
        display.drawString(0, 50, "SELECT to exit");
        display.display();

        if (button_up_pressed) {
            button_up_pressed = false;
            value += 1; // Increment
        }

        if (button_down_pressed) {
            button_down_pressed = false;
            value -= 1; // Decrement
        }

        if (button_select_pressed) {
            button_select_pressed = false;
            break; // Exit adjustment loop
        }
    }
}

void handleKpSubMenu() {
    drawMenuKpKd(kp_items, 5, kp_selected_index);
    if (button_up_pressed) {
        button_up_pressed = false;
        kp_selected_index = (kp_selected_index > 0) ? kp_selected_index - 1 : 4;
    }

    if (button_down_pressed) {
        button_down_pressed = false;
        kp_selected_index = (kp_selected_index < 4) ? kp_selected_index + 1 : 0;
    }

    if (button_select_pressed) {
        button_select_pressed = false;
        if (kp_selected_index == 4) { // Back option
            adjusting_kp = false;
            in_submenu = false;
        } else {
            adjustValue(kp_items[kp_selected_index], getKpReference(kp_selected_index));
        }
    }
}

void handleKdSubMenu() {
    drawMenuKpKd(kd_items, 5, kd_selected_index);
    if (button_up_pressed) {
        button_up_pressed = false;
        kd_selected_index = (kd_selected_index > 0) ? kd_selected_index - 1 : 4;
    }

    if (button_down_pressed) {
        button_down_pressed = false;
        kd_selected_index = (kd_selected_index < 4) ? kd_selected_index + 1 : 0;
    }

    if (button_select_pressed) {
        button_select_pressed = false;
        if (kd_selected_index == 4) { // Back option
            adjusting_kd = false;
            in_submenu = false;
        } else {
            adjustValue(kd_items[kd_selected_index], getKdReference(kd_selected_index));
        }
    }
}


void showEncoderCounts() {
    display.clear();
    display.drawString(0, 0, "Encoder Counts:");
    display.drawString(0, 15, "Left: " + String(left_encoder_count));
    display.drawString(0, 30, "Right: " + String(right_encoder_count));
    String irValues ="F:" +  String(cbrt(analogRead(IR_Front_Read)))+" L:"+String(cbrt(analogRead(IR_Left_Read)))+" R:"+String(cbrt(analogRead(IR_Right_Read)));
    delay(200);
    display.drawString(0, 45, irValues);
    display.display();

    if (button_select_pressed) {
        button_select_pressed = false;
        in_submenu = false; // Exit submenu
    }
}

void startRobot() {
    display.clear();
    display.drawString(0, 20, "Running");
    display.display();
    delay(2000);
    button_select_pressed = false;
    moveDistance(2);
    in_submenu = false; // Return to the main menu
}


void drawMenu() {
    display.clear();
    for (int i = 0; i < 5; i++) {
        if (i == selected_index) {
            display.setColor(WHITE);
            display.fillRect(0, i * 12, 128, 12); // Highlight selected
            display.setColor(BLACK);
        } else {
            display.setColor(WHITE);
        }
        display.drawString(5, i * 12, menu_items[i]);
    }
    display.display();
}

void handleMenuNavigation() {
    if (button_up_pressed) {
        button_up_pressed = false;
        selected_index = (selected_index > 0) ? selected_index - 1 : 4;
    }

    if (button_down_pressed) {
        button_down_pressed = false;
        selected_index = (selected_index < 4) ? selected_index + 1 : 0;
    }

    if (button_select_pressed) {
        button_select_pressed = false;
        in_submenu = true;
    }
}

void handleSubMenu() {
    switch (selected_index) {
        case 0: // Start the robot
            left_encoder_count = 0;
            right_encoder_count = 0;
            startRobot();
            break;
        case 1: // Kp Adjust
            adjusting_kp = true;
            break;
        case 2: // Kd Adjust
            adjusting_kd = true;
            break;
        case 3: // Display Encoder Counts
            showEncoderCounts();
            break;
        case 4: // Turns
            in_turns_submenu = true;
            break;
    }
}

#endif // MENU_H
