#include <stdio.h>
#include "input.h"
#include "servo.h"

#define NUM_SERVOS 6

int main()
{
    int selected_servo = 0;
    int servo_angles[NUM_SERVOS] = {90, 90, 90, 90, 90, 90};

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
                servo_angles[selected_servo] += step;
            printf("\nServo %d angle increased to %d\n", selected_servo + 1,
                   servo_angles[selected_servo]);
        }
        else if (k == 'D')
        { // Left arrow
            if (servo_angles[selected_servo] > 0)
                servo_angles[selected_servo] -= step;
            printf("\nServo %d angle decreased to %d\n", selected_servo + 1,
                   servo_angles[selected_servo]);
        }
    }

    setTerminalRawMode(0);
}