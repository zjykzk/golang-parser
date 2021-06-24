#include <string.h>
#include <stdio.h> // printf

#include "utils.h"
#include "utf8.c"

#define RuneError 0xFFFD

str invalidSequenceTests[] = {
  "\xed\xa0\x80\x80", // surrogate min
  "\xed\xbf\xbf\x80", // surrogate max

  // xx
  "\x91\x80\x80\x80",

  // s1
  "\xC2\x7F\x80\x80",
  "\xC2\xC0\x80\x80",
  "\xDF\x7F\x80\x80",
  "\xDF\xC0\x80\x80",

  // s2
  "\xE0\x9F\xBF\x80",
  "\xE0\xA0\x7F\x80",
  "\xE0\xBF\xC0\x80",
  "\xE0\xC0\x80\x80",

  // s3
  "\xE1\x7F\xBF\x80",
  "\xE1\x80\x7F\x80",
  "\xE1\xBF\xC0\x80",
  "\xE1\xC0\x80\x80",

  //s4
  "\xED\x7F\xBF\x80",
  "\xED\x80\x7F\x80",
  "\xED\x9F\xC0\x80",
  "\xED\xA0\x80\x80",

  // s5
  "\xF0\x8F\xBF\xBF",
  "\xF0\x90\x7F\xBF",
  "\xF0\x90\x80\x7F",
  "\xF0\xBF\xBF\xC0",
  "\xF0\xBF\xC0\x80",
  "\xF0\xC0\x80\x80",

  // s6
  "\xF1\x7F\xBF\xBF",
  "\xF1\x80\x7F\xBF",
  "\xF1\x80\x80\x7F",
  "\xF1\xBF\xBF\xC0",
  "\xF1\xBF\xC0\x80",
  "\xF1\xC0\x80\x80",

  // s7
  "\xF4\x7F\xBF\xBF",
  "\xF4\x80\x7F\xBF",
  "\xF4\x80\x80\x7F",
  "\xF4\x8F\xBF\xC0",
  "\xF4\x8F\xC0\x80",
  "\xF4\x90\x80\x80",
};

testCase(testDecodeRune) {
  forEach(struct Utf8Map, utf8map, m) {
    u8 *b = (u8 *)m->str;
    rune r;
    usize size;

    decodeRune(b, m->s, &r, &size);
    zz_assertf(r == m->r && size == m->s,
        "decodeRune(%s) = %#04x, %d want %#04x, %d",
        m->str, r, (int)size, m->r, (int)m->s);

    // there's an extra byte that bytes left behind - make sure trailing byte works
    decodeRune(b, m->s+1, &r, &size);
    zz_assertf(r == m->r && size == m->s,
        "decodeRune(%s) = %#04x, %d want %#04x, %d",
        m->str, r, (int)size, m->r, (int)m->s);
    
    // make sure missing bytes fail
    usize wantsize = 1;
    if (wantsize >= m->s) {
      wantsize = 0;
    }
    decodeRune(b, m->s-1, &r, &size);
    zz_assertf(r == RuneError && size == wantsize,
        "decodeRune(%s) = %#04x, %d want %#04x, %d",
        m->str, r, (int)size, RuneError, (int)wantsize);

    u8 tmpb[8];
    memcpy(tmpb, b, m->s);
    // make sure bad sequences fail
    if (m->s == 1) {
      tmpb[0] = 0x80;
    } else {
      tmpb[m->s-1] = 0x7F;
    }
    decodeRune(tmpb, m->s, &r, &size);
    zz_assertf(r == RuneError && size == 1,
        "decodeRune(%s) = %#04x, %d want %#04x, %d", tmpb, r, (int)size, RuneError, 1);
  }

  // surrogate rune
  forEach(struct Utf8Map, surrogateMap, m) {
    u8 *b = (u8 *)m->str;
    rune r;
    usize size;

    decodeRune(b, m->s, &r, &size);
    zz_assertf(r == RuneError && size == 1,
        "decodeRune(%s) = %#04x, %d want %#04x, %d", b, r, (int)size, RuneError, 1);
  }

  // decode invalid sequence
  forEach(str, invalidSequenceTests, s) {
    u8 *b = (u8 *)*s;
    rune r;
    usize size;

    decodeRune(b, strlen(*s), &r, &size);
    zz_assertf(r == RuneError,
        "decodeRune(%s) = %#04x, want %#04x", *s, r, RuneError);
  }
}
