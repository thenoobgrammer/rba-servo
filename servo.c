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

static int fd = -1;

void setPWM(int fd, int channel, int on, int off)
{
  int reg = 0x06 + 4 * channel;
  i2c_smbus_write_byte_data(fd, reg, on & 0xFF);
  i2c_smbus_write_byte_data(fd, reg + 1, (on >> 8) & 0x0F);
  i2c_smbus_write_byte_data(fd, reg + 2, off & 0xFF);
  i2c_smbus_write_byte_data(fd, reg + 3, (off >> 8) & 0x0F);
}

void openFd()
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

  setPWM(fd, SERVO_01, 0, pwm(SERVO_01_START_ANGLE));
  setPWM(fd, SERVO_02, 0, pwm(SERVO_02_START_ANGLE));
  setPWM(fd, SERVO_03, 0, pwm(SERVO_03_START_ANGLE));
}

void closeFd()
{
  setPWM(fd, SERVO_01, 0, pwm(SERVO_01_START_ANGLE));
  setPWM(fd, SERVO_02, 0, pwm(SERVO_02_START_ANGLE));
  setPWM(fd, SERVO_03, 0, pwm(SERVO_03_START_ANGLE));

  usleep(500000);

  close(fd);
}

void moveServo(int channel, int angle)
{
  setPWM(fd, channel, 0, pwm(angle));
}

int pwm(int angle)
{
  return 150 + (int)((600 - 150) * (angle / 180.0));
}