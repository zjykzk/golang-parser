#ifndef UTF8_H
#define UTF8_H

#include <stdbool.h>

#include "def.h"

bool isFullRune(u8 p[], usize n);
void decodeRune(u8 p[], usize n, rune * r, int * size);

#define RuneError 0xFFFD
#define RuneSelf  0x80
#define MaxRune   0x0010FFFF
#define UTFMax    4

#endif // UTF8_H
