#include "servo.h"
#include <fcntl.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define PCA9685_ADDR 0X40
#define MODE1 0X00
#define PRESCALE 0XFE

#define SERVO_01 0xF  // 15
#define SERVO_02 0xB  // 11
#define SERVO_03 0x08 // 8
#define SERVO_04 0x07 // 7
#define SERVO_05 0x04 // 4
#define SERVO_06 0x00 // 0

static int fd = -1;

int openFd()
{
  fd = open("/dev/i2c-1", O_RDWR);
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

  unsigned char oldmode = i2c_smbus_read_byte_data(fd, MODE1);
  unsigned char newmode = (oldmode & 0x7F) | 0x10;
  i2c_smbus_write_byte_data(fd, MODE1, newmode);
  i2c_smbus_write_byte_data(fd, PRESCALE, 121);
  i2c_smbus_write_byte_data(fd, MODE1, oldmode);

  return 1;
}

int closeFd()
{
  return close(fd);
}

void setPWM(int fd, int channel, int on, int off)
{
  int reg = 0x06 + 4 * channel;
  i2c_smbus_write_byte_data(fd, reg, on & 0xFF);
  i2c_smbus_write_byte_data(fd, reg + 1, (on >> 8) & 0x0F);
  i2c_smbus_write_byte_data(fd, reg + 2, off & 0xFF);
  i2c_smbus_write_byte_data(fd, reg + 3, (off >> 8) & 0x0F);
}

void moveServo(int fd, int channel, int direction, int curr_angle)
{
  int angle = curr_angle += (direction * 0.5);
  int pwm = 150 + (int)((600 - 150) * (curr_angle / 180.0));
  setPWM(fd, channel, 0, pwm);
  // usleep(20000);
  setPWM(fd, channel, 0, 0); // release the servo's torque mechanism
}
