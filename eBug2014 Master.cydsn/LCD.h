#ifndef LCD_H
#define LCD_H

#include "cytypes.h"
void print_LCD(uint8 row,const char *fmt, ...);
#define print_top(...) print_LCD(0,__VA_ARGS__)
#define print_bot(...) print_LCD(1,__VA_ARGS__)
#define print print_bot

#endif
