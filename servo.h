#ifndef SERVO_H_
#define SERVO_H_

void openFd();
void closeFd();
void moveServo(int fd, int channel);

#endif
