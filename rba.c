#include <linux/i2c-dev.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define I2C_DEV "/dev/i2c-1"
#define PCA9685_ADDR 0x40

void setServo(int fd, int channel, int tick_on, int tick_off);
void pca9685_enter_sleep(int fd);
void pca9685_exit_sleep(int fd);
void read_registers(int fd, unsigned char reg, unsigned char* buf, int len);

int main() {
    int fd = open(I2C_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return 1;
    }
    if (ioctl(fd, I2C_SLAVE, PCA9685_ADDR) < 0) {
        perror("Failed to select PCA9685");
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

    setServo(fd, 4, 0, 307);
    sleep(2);
    setServo(fd, 4, 0, 205);
    sleep(2);
    setServo(fd, 4, 0, 410);
    sleep(2);

    unsigned char buf[4];
    read_registers(fd, 0x06 + 4 * 4, buf, 4);
    printf("PWM settings for channel 4: ON_L=0x%02X, ON_H=0x%02X, OFF_L=0x%02X, OFF_H=0x%02X\n",
           buf[0], buf[1], buf[2], buf[3]);

    close(fd);
    return 0;
}

void pca9685_enter_sleep(int fd) {
    unsigned char buf[2] = {0x00, 0x10};
    if (write(fd, buf, 2) != 2) {
        perror("Failed to enter sleep mode");
    }
}

void pca9685_exit_sleep(int fd) {
    unsigned char buf[2] = {0x00, 0x20};
    if (write(fd, buf, 2) != 2) {
        perror("Failed to exit sleep mode");
    }
    usleep(1000);
}

void setServo(int fd, int channel, int tick_on, int tick_off) {
    unsigned char data[5];
    data[0] = 0x06 + 4 * channel;
    data[1] = tick_on & 0xFF;
    data[2] = (tick_on >> 8) & 0x0F;
    data[3] = tick_off & 0xFF;
    data[4] = (tick_off >> 8) & 0x0F;
    if (write(fd, data, 5) != 5) {
        perror("Failed to write PWM values");
    }
}

void read_registers(int fd, unsigned char reg, unsigned char* buf, int len) {
    if (write(fd, &reg, 1) != 1) {
        perror("Failed to set register address for read");
        return;
    }
    if (read(fd, buf, len) != len) {
        perror("Failed to read from I2C register");
    } else {
        printf("Read %d bytes from register 0x%02X: ", len, reg);
        for (int i = 0; i < len; i++) {
            printf("0x%02X ", buf[i]);
        }
        printf("\n");
    }
}
