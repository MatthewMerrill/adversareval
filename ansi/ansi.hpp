#ifndef ANSI_H
#define ANSI_H

// Copied from:
// http://athena.csus.edu/~gordonvs/180/colorAnsi.html
#define RESET           0
#define BRIGHT          1
#define DIM             2
#define UNDERLINE       4
#define BLINK           5
#define REVERSE         7
#define HIDDEN          8
#define BLACK           0
#define RED             1
#define GREEN           2
#define YELLOW          3
#define BLUE            4
#define MAGENTA         5
#define CYAN            6
#define WHITE           7

extern const char* adversarevalBlocks[4];
extern const char* adversareval3d;


void printGooglyHeader(const char* text[], int len, const char* ch, int termWidth);
void printHeader(const char* text[], int len, int fg0, int fg1, const char* ch, int leftpad);
void textcolor(int attr, int fg, int bg);

void textattr(int attr);
void textfg(int fg);
void textbg(int bg);

void textfg_256(int color);
void textbg_256(int color);

void resettext();

#endif
