#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "input.h"

void setTerminalRawMode(int enable) {
  static struct termios oldt, newt;
  static int initialized = 0;
  if (!initialized) {
    tcgetattr(STDIN_FILENO, &oldt);
    initialized = 1;
  }
  if (enable) {
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  }
}

char get_key(void) {
  char c;
  if (read(STDIN_FILENO, &c, 1) != 1)
    return 0;
  if (c == '\x1B') {
    char seq[2];
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return 0;
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return 0;
    if (seq[0] == '[') {
      return seq[1];
    } else {
      return 0;
    }
  }
  return c;
}
