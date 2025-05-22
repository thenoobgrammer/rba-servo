#ifndef SERVO_H_
#define SERVO_H_

#define SERVO_STEP 0.5
#define SERVO_LEFT -1
#define SERVO_RIGHT 1

#define SERVO_01 0xF  // 15
#define SERVO_02 0xB  // 11
#define SERVO_03 0x08 // 8
#define SERVO_04 0x07 // 7
#define SERVO_05 0x04 // 4
#define SERVO_06 0x00 // 0

void openFd();
void closeFd();
void moveServo(int channel, int direction);

#endif
