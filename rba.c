#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "rba.h"
#include "input.h"

#define PCA9685_ADDR 0X40
#define MODE1 0X00
#define PRESCALE 0XFE

#define NUM_SERVOS 6

#define SERVO_01 0xF  // 15
#define SERVO_02 0xB  // 11
#define SERVO_03 0x08 // 8
#define SERVO_04 0x07 // 7
#define SERVO_05 0x04 // 4
#define SERVO_06 0x00 // 0

void setPWM(int fd, int channel, int on, int off)
{
  int reg = 0x06 + 4 * channel;
  i2c_smbus_write_byte_data(fd, reg, on & 0xFF);
  i2c_smbus_write_byte_data(fd, reg + 1, (on >> 8) & 0x0F);
  i2c_smbus_write_byte_data(fd, reg + 2, off & 0xFF);
  i2c_smbus_write_byte_data(fd, reg + 3, (off >> 8) & 0x0F);
}

void moveServo(int fd, int channel, int direction, int start_angle, int end_angle, int delay_us)
{
  for (float angle = start_angle; angle >= end_angle; angle += (int)(direction * 0.5))
  {
    int pwm = 150 + (int)((600 - 150) * (angle / 180.0));
    setPWM(fd, channel, 0, pwm);
    usleep(delay_us);
  }

  setPWM(fd, channel, 0, 0);
}

int main()
{
  int fd = open("/dev/i2c-1", O_RDWR);
  if (fd < 0)
  {
    perror("open");
    exit(1);
  }

  if (ioctl(fd, I2C_SLAVE, PCA9685_ADDR) < 0)
  {
    perror("ioctl");
    exit(1);
  }

  i2c_smbus_write_byte_data(fd, MODE1, 0x00);

  usleep(5000);

  unsigned char oldmode = i2c_smbus_read_byte_data(fd, MODE1);
  unsigned char newmode = (oldmode & 0x7F) | 0x10;
  i2c_smbus_write_byte_data(fd, MODE1, newmode);

  i2c_smbus_write_byte_data(fd, PRESCALE, 121);

  i2c_smbus_write_byte_data(fd, MODE1, oldmode);
  usleep(5000);

  int selected_servo = 0;  // Start with servo 1 (index 0)
      int servo_angles[NUM_SERVOS] = {90, 90, 90, 90, 90, 90}; // initial angles

      setTerminalRawMode(1);
      printf(
          "Select servo (1-6), then use ← or → arrows. Press 'q' to quit.\n"
      );

  while (1) {
          printf("\rServo %d selected. Current angle: %d   ",
                  selected_servo + 1, servo_angles[selected_servo]);
          fflush(stdout);

          char k = get_key();

          if (k == 'q') break;

          if (k >= '1' && k <= '6') {
              selected_servo = k - '1';
              printf("\nServo %d is now selected.\n", selected_servo + 1);
              continue;
          }

          int step = 5;
          if (k == 'C') { // Right arrow
              if (servo_angles[selected_servo] < 180)
                  servo_angles[selected_servo] += step;
              printf("\nServo %d angle increased to %d\n", selected_servo + 1, servo_angles[selected_servo]);
              // CALL YOUR SERVO CONTROL HERE
              // move_servo(selected_servo, servo_angles[selected_servo]);
          } else if (k == 'D') { // Left arrow
              if (servo_angles[selected_servo] > 0)
                  servo_angles[selected_servo] -= step;
              printf("\nServo %d angle decreased to %d\n", selected_servo + 1, servo_angles[selected_servo]);
              // CALL YOUR SERVO CONTROL HERE
              // move_servo(selected_servo, servo_angles[selected_servo]);
          }
      }

   setTerminalRawMode(0);
  sweepServoSlow(fd, SERVO_01, 0, 90, 25000);
  sweepServoSlow(fd, SERVO_01, 90, 0, 25000);

  sweepServoSlow(fd, SERVO_02, 20, 120, 25000);
  sweepServoSlow(fd, SERVO_02, 120, 20, 25000);

  close(fd);
  return 0;
}
