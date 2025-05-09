#include <linux/i2c-dev.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define I2C_DEV "/dev/i2c-1"
#define PCA9685_ADDR 0x40

void setServo(int fd, int channel, int tick_on, int tick_off);
void pca9685_enter_sleep(int fd);
void pca9685_exit_sleep(int fd);

int main() {
	int fd = open(I2C_DEV, O_RDWR);
	if (fd < 0) {
		perror("Failed to open i2c device");
		return 1;
	}

	if (ioctl(fd, I2C_SLAVE, PCA9685_ADDR) < 0) { 
		perror("Failed to select I2C device");
		close(fd);
		return 1;
	}

	// sleep
	pca9685_enter_sleep(fd);

	// set prescale
	unsigned char prescale[] = {0xFE, 121};
	write(fd, sleep, 2);

	// exit sleep
	pca9685_exit_sleep(fd);
	
	setServo(fd, 0, 0, 307);
	sleep(2);
	setServo(fd, 0, 0, 205);
	sleep(2);
	setServo(fd, 0, 0, 410);
	sleep(2);

	close(fd);
	return 0;
}

void pca9685_enter_sleep(int fd) {
	unsigned char buf[2] = {0x00, 0x10};
	write(fd, buf, 2);
}

void pca9685_exit_sleep(int fd) {
	unsigned char buf[2] = {0x00, 0x20};
	write(fd, buf, 2);
	usleep(500);
}

void setServo(int fd, int channel, int tick_on, int tick_off) {
	unsigned char data[5];
	data[0] = 0x06 + 4*channel;
	data[1] = tick_on & 0xFF;
	data[2] = (tick_on >> 8) & 0x0F;
	data[3] = tick_off & 0xFF;
	data[4] = (tick_off >> 0) & 0x0F;
	write(fd, data, 5);
}
