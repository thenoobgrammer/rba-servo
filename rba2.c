#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <i2c/smbus.h>

#define PCA9685_ADDR 0X40
#define MODE1 0X00
#define PRESCALE 0XFE

void setPWM(int fd, int channel, int on, int off)
{
	int reg = 0x06 + 4 * channel;
	i2c_smbus_write_byte_data(fd, reg, on & 0xFF);
	i2c_smbus_write_byte_data(fd, reg + 1, (on >> 8) & 0x0F);
	i2c_smbus_write_byte_data(fd, reg + 2, off & 0xFF);
	i2c_smbus_write_byte_data(fd, reg + 3, (off >> 8) & 0x0F);
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

	int curr_angle = 150;
	int delta = 225;
	int direction;
	int loop_count = 50;

	for (int i = 0; i < loop_count; i++)
	{
		if (curr_angle <= 150)
		{
			direction = 1;
		}
		else if (curr_angle >= 600)
		{
			direction = -1;
		}
		curr_angle += (225 * direction);

		setPWM(fd, 15, 0, curr_angle);
		usleep(1000000); // 1 second
	}

	close(fd);
	return 0;
}
