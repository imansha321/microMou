#ifndef WALLS_H
#define WALLS_H

bool wallFront() {
    if (analogRead(IR_Front_Read) > 0) return true;
}

bool wallRight() {
    if (analogRead(IR_Right_Read) > 0) return true;
}

bool wallLeft() {
    if (analogRead(IR_Left_Read) > 0) return true;
}


#endif // WALLS_H