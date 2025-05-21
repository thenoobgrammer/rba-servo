#include <stdio.h>
#include <termios.h>
#include <unistd.h>

void setTerminalRawMode(int enable) {
  static struct termios oldt, newt;

  if (enable) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // disable buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore settings
  }
}

int main() {
  setTerminalRawMode(1);

  printf("Press arrow keys (q to quit):\n");

  while (1) {
    char c;
    read(STDIN_FILENO, &c, 1);
    if (c == 'q')
      break;

    if (c == '\x1B') { // ESC
      char seq[2];
      if (read(STDIN_FILENO, &seq[0], 1) == 0)
        continue;
      if (read(STDIN_FILENO, &seq[1], 1) == 0)
        continue;

      if (seq[0] == '[') {
        switch (seq[1]) {
        case 'A':
          printf("↑ Up\n");
          break;
        case 'B':
          printf("↓ Down\n");
          break;
        case 'C':
          printf("→ Right\n");
          break;
        case 'D':
          printf("← Left\n");
          break;
        }
      }
    } else {
      printf("Pressed: %c\n", c);
    }
  }

  setTerminalRawMode(0); // Restore normal terminal behavior
  return 0;
}
