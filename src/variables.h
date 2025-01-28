#ifndef VARIABLE_H
#define VARIABLE_H
// PID Constants
double setpoint = 0;
double setpointL = 5.95;
double setpointR = 5.6;

double inputIR, outputIR;
double lastErrorIR = 0;


float KpL = 19, KpR = 19, KpIR = 4, KpE = 10;
float KdL = 20, KdR =20, KdIR = 8, KdE = 5;



//Turn PID
float KpTurn = 1;
float KdTurn =1.7;

// Variables for Turn PD calculations
float left_turn_error = 0, right_turn_error = 0;
float left_turn_prev_error = 0, right_turn_prev_error = 0;
float left_turn_derivative = 0, right_turn_derivative = 0;

bool in_submenu = false;
#endif // VARIABLE_H