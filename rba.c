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
    pca9685_enter_sleep(fd);

    unsigned char prescale[] = {0xFE, 121};
    if (write(fd, prescale, 2) != 2) {
        perror("Failed to write prescale");
    }

    pca9685_exit_sleep(fd);

    unsigned char mode2[] = {0x01, 0x04};
    if (write(fd, mode2, 2) != 2) {
        perror("Failed to set MODE2");
    }

    // Move servo to 3 positions
    setServo(fd, 0, 0, 307);  // Center position
    sleep(2);
    setServo(fd, 0, 0, 205);  // Left
    sleep(2);
    setServo(fd, 0, 0, 410);  // Right
    sleep(2);

    close(fd);
    return 0;
}

void pca9685_enter_sleep(int fd) {
    unsigned char buf[2] = {0x00, 0x10}; // MODE1 with SLEEP set
    if (write(fd, buf, 2) != 2) {
        perror("Failed to write sleep");
    }
}

void pca9685_exit_sleep(int fd) {
    unsigned char buf[2] = {0x00, 0x20};
    if (write(fd, buf, 2) != 2) {
        perror("Failed to exit sleep");
    }
    usleep(1000);
}

void setServo(int fd, int channel, int tick_on, int tick_off) {
    unsigned char data[5];
    data[0] = 0x06 + 4*channel;
    data[1] = tick_on & 0xFF;
    data[2] = (tick_on >> 8) & 0x0F;
    data[3] = tick_off & 0xFF;
    data[4] = (tick_off >> 8) & 0x0F;
    if (write(fd, data, 5) != 5) {
        perror("Failed to set PWM");
    }
}
