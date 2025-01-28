#ifndef MOTION_H
#define MOTION_H

#include <Arduino.h>
#include "variables.h"
#include "display.h"
#include "menu.h"
#include "floodfill.h"

// PWM Channels
#define LEFT_FORWARD_CHANNEL 0
#define LEFT_BACKWARD_CHANNEL 1
#define RIGHT_FORWARD_CHANNEL 2
#define RIGHT_BACKWARD_CHANNEL 3

// Constants
#define DISTANCE_THRESHOLD 500
#define TICKS_PER_CM 10
#define TARGET_DISTANCE 18
#define PWM_FREQUENCY 1000
#define PWM_RESOLUTION 8

// Motion profile parameters
float current_speed = 0.0;
float wheel_circumference = 3.14 * 45;
float counts_per_revolution = 360;

// Wheel parameters
const float wheel_diameter = 0.043;
const int pulses_per_revolution = 360;

volatile long left_encoder_count = 0;
volatile long right_encoder_count = 0;

void moveForward() {
    
}

void turnLeft() {
    
}

void turnRight() {
    
}

void setMotors(float left_speed, float right_speed) {
    left_speed = constrain(left_speed, -255, 255);
    right_speed = constrain(right_speed, -255, 255);

    if (left_speed >= 0) {
        ledcWrite(LEFT_FORWARD_CHANNEL, left_speed);
        ledcWrite(LEFT_BACKWARD_CHANNEL, 0);
    } else {
        ledcWrite(LEFT_FORWARD_CHANNEL, 0);
        ledcWrite(LEFT_BACKWARD_CHANNEL, -left_speed);
    }

    if (right_speed >= 0) {
        ledcWrite(RIGHT_FORWARD_CHANNEL, right_speed);
        ledcWrite(RIGHT_BACKWARD_CHANNEL, 0);
    } else {
        ledcWrite(RIGHT_FORWARD_CHANNEL, 0);
        ledcWrite(RIGHT_BACKWARD_CHANNEL, -right_speed);
    }
}

void stopMotors() {
    ledcWrite(LEFT_FORWARD_CHANNEL, 0);
    ledcWrite(LEFT_BACKWARD_CHANNEL, 0);
    ledcWrite(RIGHT_FORWARD_CHANNEL, 0);
    ledcWrite(RIGHT_BACKWARD_CHANNEL, 0);
}

void turnLeft90Degrees() {
    // Reset encoder counts
    left_encoder_count = 0;
    right_encoder_count = 0;
    delay(2000);
    // Target encoder value for a 90-degree turn
    long turnTargetEncoderValue = 200; // Adjust based on your robot

    // PD control loop
    while (left_encoder_count < turnTargetEncoderValue || right_encoder_count < turnTargetEncoderValue) {
        // Calculate errors
        left_turn_error = turnTargetEncoderValue - left_encoder_count;
        right_turn_error = turnTargetEncoderValue - right_encoder_count;

        // Derivative (rate of change of error)
        left_turn_derivative = left_turn_error - left_turn_prev_error;
        right_turn_derivative = right_turn_error - right_turn_prev_error;

        // PD outputs
        int left_motor_speed = KpTurn * left_turn_error + KdTurn * left_turn_derivative;
        int right_motor_speed = KpTurn * right_turn_error + KdTurn * right_turn_derivative;

        // Constrain motor speeds to a safe range
        left_motor_speed = constrain(left_motor_speed, 0, 255);
        right_motor_speed = constrain(right_motor_speed, 0, 255);

        // Set motors for turning (one forward, one backward)
        setMotors(-left_motor_speed, right_motor_speed);

        // Update previous errors
        left_turn_prev_error = left_turn_error;
        right_turn_prev_error = right_turn_error;
    }

    // Stop the robot after the turn
    stopMotors();
    in_submenu = false; // Return to the main menu
}
   
void turnRight90Degrees() {
    // Reset encoder counts
    left_encoder_count = 0;
    right_encoder_count = 0;
    delay(2000);

    // Target encoder value for a 90-degree turn
    long turnTargetEncoderValue = 193; // Adjust based on your robot

    // PD control loop
    while (left_encoder_count < turnTargetEncoderValue || right_encoder_count < turnTargetEncoderValue) {
        // Calculate errors
        left_turn_error = turnTargetEncoderValue - left_encoder_count;
        right_turn_error = turnTargetEncoderValue - right_encoder_count;

        // Derivative (rate of change of error)
        left_turn_derivative = left_turn_error - left_turn_prev_error;
        right_turn_derivative = right_turn_error - right_turn_prev_error;

        // PD outputs
        int left_motor_speed = KpTurn * left_turn_error + KdTurn * left_turn_derivative;
        int right_motor_speed = KpTurn * right_turn_error + KdTurn * right_turn_derivative;

        // Constrain motor speeds to a safe range
        left_motor_speed = constrain(left_motor_speed, 0, 255);
        right_motor_speed = constrain(right_motor_speed, 0, 255);

        // Set motors for turning (one forward, one backward)
        setMotors(left_motor_speed, -right_motor_speed);

        // Update previous errors
        left_turn_prev_error = left_turn_error;
        right_turn_prev_error = right_turn_error;
    }

    // Stop the robot after the turn
    stopMotors();
    in_submenu = false; // Return to the main menu
}

float calculateDistance(float encoder_count) {
    return (encoder_count / counts_per_revolution) * wheel_circumference;
}


double SideIRpidOutput() {
    int leftValue = cbrt(analogRead(IR_Left_Read));
    int rightValue = cbrt(analogRead(IR_Right_Read));

    // Check if walls exist based on sensor thresholds
    bool leftWallExists = leftValue > 0;  // Adjust threshold as needed
    bool rightWallExists = rightValue > 0;

    double errorIR = 0;

    // Variables for dynamic Kp and Kd
    float Kp, Kd;

    if (leftWallExists && rightWallExists) {
        
        // Maintain equal distance from both walls (center alignment)
        errorIR = -(leftValue - rightValue);
        DisplayText(String(errorIR),0,0);

        // Set Kp and Kd for both walls present
        Kp = KpIR;
        Kd = KdIR;
    } else if (leftWallExists) {
        
        // Follow the left wall
        errorIR =(setpointL - leftValue);
        DisplayText2("Left", 0, 0, String(errorIR) ,0,20);

        // Set Kp and Kd for left wall only
        Kp = KpL;
        Kd = KdL;
    } else if (rightWallExists) {
       
        // Follow the right wall
        errorIR = (rightValue - setpointR);
        DisplayText2("Right", 0, 0, String(errorIR),0,20);

        // Set Kp and Kd for right wall only
        Kp = KpR;
        Kd = KdR;
    } else {
        Kp = KpE;
        Kd = KdE;
        DisplayText("None  "+String(Kp), 0, 0);
        // No walls detected, rely on encoders to move straight
        static long leftEncoderPrev = left_encoder_count;
        static long rightEncoderPrev = right_encoder_count;

        // Calculate encoder-based correction
        long leftDelta = left_encoder_count - leftEncoderPrev;
        long rightDelta = right_encoder_count - rightEncoderPrev;

        // Error is the difference in distances covered by wheels
        errorIR = (rightDelta - leftDelta);

        // Update previous encoder counts
        leftEncoderPrev = left_encoder_count;
        rightEncoderPrev = right_encoder_count;

        // Set Kp and Kd for no walls
        
    }

    // Calculate derivative
    double derivative = errorIR - lastErrorIR;

    // Compute PID output
    outputIR = Kp * errorIR + Kd * derivative;

    // Update last error
    lastErrorIR = errorIR;

    return outputIR;
}
void moveDistance(float distance) {
    long pulses_needed = (distance / (PI * wheel_diameter)) * pulses_per_revolution;
    left_encoder_count = 0;
    right_encoder_count = 0;

    while (right_encoder_count < pulses_needed && left_encoder_count < pulses_needed) {
        //String text2 = "left:" + String(analogRead(IR_Left_Read)) + " right:" + String(analogRead(IR_Right_Read)) + " front:" + String(analogRead(IR_Front_Read));
        int baseSpeed = 60;
        setMotors(baseSpeed - SideIRpidOutput(), baseSpeed + SideIRpidOutput());
    }
    stopMotors();
}

void start(){
    
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        for (int k = 0; k < 4; k++) {
         WallMatrix[i][j][k] = 0;
        }
      }
    }

    CalculateDistances();
    

    
    //Index 0: Top wall.
    //Index 1: Right wall.
    //Index 2: Bottom wall.
    //Index 3: Left wall.

    while (Distances[current_cell[0]][current_cell[1]] != 0) {
            UpdateWalls();
            CalculateDistances();
            char nextMove = ChooseNextMove();

            if (nextMove == 'f') {
                moveForward();
            } else if (nextMove == 'l') {
                turnLeft();
            
            } else if (nextMove == 'r') {
                turnRight();
            
            } else if (nextMove == 'b') {
                // Turn 180 degrees and move forward to backtrack
                turnLeft();
                turnLeft();
                moveForward();
                
            }

            
            UpdatePosition(nextMove);
        }
        

        CalculateDistances2();


        while (Distances[current_cell[0]][current_cell[1]] != 0) {
            UpdateWalls();
            CalculateDistances2();
            char nextMove = ChooseNextMove();

            if (nextMove == 'f') {
                moveForward();
            } else if (nextMove == 'l') {
                turnLeft();
            
            } else if (nextMove == 'r') {
                turnRight();
            
            } else if (nextMove == 'b') {
                // Turn 180 degrees and move forward to backtrack
                turnLeft();
                turnLeft();
                moveForward();
                
            }

            
            UpdatePosition(nextMove);
        }

        CalculateDistances();

        while (Distances[current_cell[0]][current_cell[1]] != 0) {
            UpdateWalls();
            CalculateDistances();
            char nextMove = ChooseNextMove();

            if (nextMove == 'f') {
                moveForward();
            } else if (nextMove == 'l') {
                turnLeft();
            
            } else if (nextMove == 'r') {
                turnRight();
            
            } else if (nextMove == 'b') {
                // Turn 180 degrees and move forward to backtrack
                turnLeft();
                turnLeft();
                moveForward();
                
            }

            
            UpdatePosition(nextMove);
        }
}

#endif // MOTION_H
