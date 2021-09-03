#include "utf8.h"

// The default lowest and highest continuation byte.
#define locb 0b10000000
#define hicb 0b10111111

#define t1 0b00000000
#define tx 0b10000000
#define t2 0b11000000
#define t3 0b11100000
#define t4 0b11110000
#define t5 0b11111000

#define maskx 0b00111111
#define mask2 0b00011111
#define mask3 0b00001111
#define mask4 0b00000111

#define rune1Max (1<<7 - 1)
#define rune2Max (1<<11 - 1)
#define rune3Max (1<<16 - 1)

// The default lowest and highest continuation byte.
#define locb 0b10000000
#define hicb 0b10111111

// These names of these constants are chosen to give nice alignment in the
// table below. The first nibble is an index into acceptRanges or F for
// special one-byte cases. The second nibble is the Rune length or the
// Status for the special one-byte case.
#define xx 0xF1 // invalid: size 1
#define as 0xF0 // ASCII: size 1
#define s1 0x02 // accept 0, size 2
#define s2 0x13 // accept 1, size 3
#define s3 0x03 // accept 0, size 3
#define s4 0x23 // accept 2, size 3
#define s5 0x34 // accept 3, size 4
#define s6 0x04 // accept 0, size 4
#define s7 0x44 // accept 4, size 4

// first is information about the first byte in a UTF-8 sequence.
static u8 first[256] = {
  //   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
  as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, // 0x00-0x0F
  as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, // 0x10-0x1F
  as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, // 0x20-0x2F
  as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, // 0x30-0x3F
  as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, // 0x40-0x4F
  as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, // 0x50-0x5F
  as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, // 0x60-0x6F
  as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, as, // 0x70-0x7F
  //   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
  xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, // 0x80-0x8F
  xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, // 0x90-0x9F
  xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, // 0xA0-0xAF
  xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, // 0xB0-0xBF
  xx, xx, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, // 0xC0-0xCF
  s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, s1, // 0xD0-0xDF
  s2, s3, s3, s3, s3, s3, s3, s3, s3, s3, s3, s3, s3, s4, s3, s3, // 0xE0-0xEF
  s5, s6, s6, s6, s7, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, // 0xF0-0xFF
};

// acceptRange gives the range of valid values for the second byte in a UTF-8
// sequence.
struct acceptRange {
  u8 lo; // lowest value for second byte.
  u8 hi; // highest value for second byte.
};

// acceptRanges has size 16 to avoid bounds checks in the code that uses it.
static struct acceptRange acceptRanges[16] = {
  [0] = {locb, hicb},
  [1] = {0xA0, hicb},
  [2] = {locb, 0x9F},
  [3] = {0x90, hicb},
  [4] = {locb, 0x8F},
};

// isFullRune reports whether the bytes in p begin with a full UTF-8 encoding of a rune.
// An invalid encoding is considered a full Rune since it will convert as a width-1 error rune.
bool isFullRune(u8 p[], usize n) {
  if (n == 0) {
    return false;
  }
  u8 x = first[p[0]];
  if (n >= (x&7)) {
    return true; // ASCII, invalid or valid.
  }

  // Must be short or invalid.
  struct acceptRange * accept = &acceptRanges[x>>4];
  if (n > 1 && (p[1] < accept->lo || accept->hi < p[1])) {
    return true;
  }

  if (n > 2 && (p[2] < locb || hicb < p[2])) {
    return true;
  }

  return false;
}

// decodeRune unpacks the first UTF-8 encoding in p and returns the rune and
// its width in bytes. If p is empty it returns (RuneError, 0). Otherwise, if
// the encoding is invalid, it returns (RuneError, 1). Both are impossible
// results for correct, non-empty UTF-8.
//
// An encoding is invalid if it is incorrect UTF-8, encodes a rune that is
// out of range, or is not the shortest possible UTF-8 encoding for the
// value. No other validation is performed.
void decodeRune(u8 p[], usize n, rune * r, int * size) {
  if (n < 1) {
    *r = RuneError;
    *size = 0;
    return;
  }

  u8 p0 = p[0];
  u8 x = first[p0];
  if (x >= as) {
    // The following code simulates an additional check for x == xx and
    // handling the ASCII and invalid cases accordingly. This mask-and-or
    // approach prevents an additional branch.
    rune mask = ((rune) x << 31) >> 31; // Create 0x0000 or 0xFFFF.
    *r = (((rune)p[0])&(~mask)) | (RuneError&mask);
    *size = 1;
    return;
  }

  usize sz = (usize)(x & 7);
  struct acceptRange * accept = &acceptRanges[x>>4];
  if (n < sz) {
    *r = RuneError;
    *size = 1;
    return;
  }

  u8 b1 = p[1];
  if (b1 < accept->lo || accept->hi < b1) {
    *r = RuneError;
    *size = 1;
    return;
  }
  
  if (sz <= 2) { // <= instead of == to help the compiler eliminate some bounds checks
    *r =  ((rune)(p0&mask2))<<6 | (rune) (b1&maskx);
    *size = 2;
    return;
  }

  u8 b2 = p[2];
  if (b2 < locb || hicb < b2) {
     *r = RuneError;
     *size = 1;
     return;
  }

  if (sz <= 3) {
    *r = ((rune)(p0&mask3))<<12 | ((rune)(b1&maskx))<<6 | (rune)(b2&maskx);
    *size = 3;
    return;
  }

  u8 b3 = p[3];
  if (b3 < locb || hicb < b3) {
    *r = RuneError;
    *size = 1;
    return;
  }

  *r = ((rune)(p0&mask4))<<18 | ((rune)(b1&maskx))<<12 | ((rune)(b2&maskx))<<6 | (rune)(b3&maskx);
  *size = 4;
  return;
}
