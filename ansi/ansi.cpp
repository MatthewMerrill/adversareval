#include <cstdio>
#include <iostream>

#include "ansi.hpp"

const char* adversarevalBlocks[] = {
" **** * ****** ******** **** *** *  ",
"* ** ** *** ***** * ****** * ** **  ",
"**** *****  **  ******* *  * *****  ",
"* **** ****** ***** ** **** *** ****",
};

const char* adversareval3d =
"           __                                                                        ___      \n"
"          /\\ \\                                                                      /\\_ \\     \n"
"   __     \\_\\ \\  __  __     __   _ __   ____     __     _ __    __   __  __     __  \\//\\ \\    \n"
" /'__`\\   /'_` \\/\\ \\/\\ \\  /'__`\\/\\`'__\\/',__\\  /'__`\\  /\\`'__\\/'__`\\/\\ \\/\\ \\  /'__`\\  \\ \\ \\   \n"
"/\\ \\L\\.\\_/\\ \\L\\ \\ \\ \\_/ |/\\  __/\\ \\ \\//\\__, `\\/\\ \\L\\.\\_\\ \\ \\//\\  __/\\ \\ \\_/ |/\\ \\L\\.\\_ \\_\\ \\_ \n"
"\\ \\__/.\\_\\ \\___,_\\ \\___/ \\ \\____\\\\ \\_\\\\/\\____/\\ \\__/.\\_\\\\ \\_\\\\ \\____\\\\ \\___/ \\ \\__/.\\_\\/\\____\\\n"
" \\/__/\\/_/\\/__,_ /\\/__/   \\/____/ \\/_/ \\/___/  \\/__/\\/_/ \\/_/ \\/____/ \\/__/   \\/__/\\/_/\\/____/\n";

const int googleColors[] = { BLUE, RED, YELLOW, BLUE, GREEN, RED }; 

// Copied from:
// http://athena.csus.edu/~gordonvs/180/colorAnsi.html
void textcolor(int attr, int fg, int bg) {
  char command[13];
  std::sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
  printf("%s", command);
}

void textattr(int attr) {
  char command[13];
  std::sprintf(command, "%c[%dm", 0x1B, attr);
  printf("%s", command);
}

void textfg(int fg) {
  char command[13];
  std::sprintf(command, "%c[%dm", 0x1B, fg + 30);
  printf("%s", command);
}

void textbg(int bg) {
  char command[13];
  std::sprintf(command, "%c[%dm", 0x1B, bg + 40);
  printf("%s", command);
}

void resettext() {
  printf("%c[0;;m", 0x1B);
}

void printGooglyHeader(const char* text[], int len, const char* ch, int termWidth) {
  int pad = (termWidth - len);
  for (int r = 0; r < 4; ++r){
    for (int i = 3; i < pad / 2; ++i) {
      printf(".");
    }
    printf("   ");
    for (int idx = 0; idx < len; idx += 3) {
      textfg(googleColors[(idx / 3) % 6]);
      printf("%s", ((text[r] + idx)[0] == ' ') ? " " : ch);
      printf("%s", ((text[r] + idx)[1] == ' ') ? " " : ch);
      printf("%s", ((text[r] + idx)[2] == ' ') ? " " : ch);
      resettext();
    }
    printf("   ");
    for (int i = 3; i < (pad-1)/2 + 1; ++i) {
      printf(".");
    }
    printf("\n");
  }
}

void printHeader(const char* text[], int len, int fg0, int fg1, const char* ch, int termWidth) {
  int pad = (termWidth - len);
  for (int r = 0; r < 4; ++r){
    for (int i = 3; i < pad / 2; ++i) {
      printf(".");
    }
    printf("   ");
    for (int idx = 0; idx < len; idx += 3) {
      textattr((idx & 1) ? fg1 : fg0);
      printf("%s", ((text[r] + idx)[0] == ' ') ? " " : ch);
      printf("%s", ((text[r] + idx)[1] == ' ') ? " " : ch);
      printf("%s", ((text[r] + idx)[2] == ' ') ? " " : ch);
      resettext();
    }
    printf("   ");
    for (int i = 3; i < (pad-1)/2 + 1; ++i) {
      printf(".");
    }
    printf("\n");
  }
}
