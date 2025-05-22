#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "input.h"
#include "servo.h"

#define NUM_SERVOS 6

void handle_sigint(int sig)
{
    printf("\nCaught SIGINT, cleaning up...\n");
    closeFd();
    restoreTerminal();
    exit(0);
}

int main()
{
    signal(SIGINT, handle_sigint);

    openFd();
    int selected_servo = 0;
    int servo_angles[NUM_SERVOS] = {0, 0, 0, 0, 0, 0};
    int servo_chs[NUM_SERVOS] = {SERVO_01, SERVO_02, SERVO_03, SERVO_04, SERVO_05, SERVO_06};

    moveServo(SERVO_01, 0);
    moveServo(SERVO_02, 0);
    moveServo(SERVO_03, 0);

    setTerminalRawMode(1);
    printf("Select servo (1-6), then use ← or → arrows. Press 'q' to quit.\n");

    while (1)
    {
        printf("\rServo %d selected. Current angle: %d   ", selected_servo + 1,
               servo_angles[selected_servo]);
        fflush(stdout);

        char k = get_key();

        if (k == 'q')
            break;

        if (k >= '1' && k <= '6')
        {
            selected_servo = k - '1';
            printf("\nServo %d is now selected.\n", selected_servo + 1);
            continue;
        }

        int step = 5;
        if (k == 'C')
        { // Right arrow
            if (servo_angles[selected_servo] < 180)
            {
                int angle = servo_angles[selected_servo] += SERVO_STEP;
                moveServo(servo_chs[selected_servo], angle);
            }
            printf("\nServo %d angle increased to %d\n", selected_servo + 1,
                   servo_angles[selected_servo]);
        }
        else if (k == 'D')
        { // Left arrow
            if (servo_angles[selected_servo] > 0)
            {
                int angle = servo_angles[selected_servo] -= SERVO_STEP;
                moveServo(servo_chs[selected_servo], angle);
            }
            printf("\nServo %d angle decreased to %d\n", selected_servo + 1,
                   servo_angles[selected_servo]);
        }
    }

    setTerminalRawMode(0);
    closeFd();
}
