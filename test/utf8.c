#ifndef TEST_UTF8_H
#define TEST_UTF8_H

#include "../src/utf8.h"

struct Utf8Map {
  rune r;
  char * str;
  usize s;
};

struct Utf8Map utf8map[] = {
  {0x0000, "\x00\x00", 1},
  {0x0001, "\x01\x00", 1},
  {0x007e, "\x7e\x00", 1},
  {0x007f, "\x7f\x00", 1},
  {0x0080, "\xc2\x80\x00", 2},
  {0x0081, "\xc2\x81\x00", 2},
  {0x00bf, "\xc2\xbf\x00", 2},
  {0x00c0, "\xc3\x80\x00", 2},
  {0x00c1, "\xc3\x81\x00", 2},
  {0x00c8, "\xc3\x88\x00", 2},
  {0x00d0, "\xc3\x90\x00", 2},
  {0x00e0, "\xc3\xa0\x00", 2},
  {0x00f0, "\xc3\xb0\x00", 2},
  {0x00f8, "\xc3\xb8\x00", 2},
  {0x00ff, "\xc3\xbf\x00", 2},
  {0x0100, "\xc4\x80\x00", 2},
  {0x07ff, "\xdf\xbf\x00", 2},
  {0x0400, "\xd0\x80\x00", 2},
  {0x0800, "\xe0\xa0\x80\x00", 3},
  {0x0801, "\xe0\xa0\x81\x00", 3},
  {0x1000, "\xe1\x80\x80\x00", 3},
  {0xd000, "\xed\x80\x80\x00", 3},
  {0xd7ff, "\xed\x9f\xbf\x00", 3}, // last code point before surrogate half.
  {0xe000, "\xee\x80\x80\x00", 3}, // first code point after surrogate half.
  {0xfffe, "\xef\xbf\xbe\x00", 3},
  {0xffff, "\xef\xbf\xbf\x00", 3},
  {0x10000, "\xf0\x90\x80\x80\x00", 4},
  {0x10001, "\xf0\x90\x80\x81\x00", 4},
  {0x40000, "\xf1\x80\x80\x80\x00", 4},
  {0x10fffe, "\xf4\x8f\xbf\xbe\x00", 4},
  {0x10ffff, "\xf4\x8f\xbf\xbf\x00", 4},
  {0xFFFD, "\xef\xbf\xbd\x00", 3},
};

struct Utf8Map surrogateMap[] = {
  {0xd800, "\xed\xa0\x80", 3}, // surrogate min decodes to (RuneError, 1)
  {0xdfff, "\xed\xbf\xbf", 3}, // surrogate max decodes to (RuneError, 1)
};

const char * testStrings[] = {
  "",
  "abcd",
  "☺☻☹",
  "日a本b語ç日ð本Ê語þ日¥本¼語i日©",
  "日a本b語ç日ð本Ê語þ日¥本¼語i日©日a本b語ç日ð本Ê語þ日¥本¼語i日©日a本b語ç日ð本Ê語þ日¥本¼語i日©",
  "\x80\x80\x80\x80",
};
#endif
