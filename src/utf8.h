#ifndef UTF8_H
#define UTF8_H

#include <stdbool.h>

#include "def.h"

bool isFullRune(u8 p[], usize n);
void decodeRune(u8 p[], usize n, rune * r, usize * size);

#endif // UTF8_H
