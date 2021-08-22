#include <stdio.h> // printf

#include "../src/unicode.h"

#include "utils.h"

const int UpperCase = 0, LowerCase = 1, TitleCase = 1, MaxCase = 1;

rune upperTest[] = {
  0x41,
  0xc0,
  0xd8,
  0x100,
  0x139,
  0x14a,
  0x178,
  0x181,
  0x376,
  0x3cf,
  0x13bd,
  0x1f2a,
  0x2102,
  0x2c00,
  0x2c10,
  0x2c20,
  0xa650,
  0xa722,
  0xff3a,
  0x10400,
  0x1d400,
  0x1d7ca,
};

rune notupperTest[] = {
  0x40,
  0x5b,
  0x61,
  0x185,
  0x1b0,
  0x377,
  0x387,
  0x2150,
  0xab7d,
  0xffff,
  0x10000,
};

rune letterTest[] = {
  0x41,
  0x61,
  0xaa,
  0xba,
  0xc8,
  0xdb,
  0xf9,
  0x2ec,
  0x535,
  0x620,
  0x6e6,
  0x93d,
  0xa15,
  0xb99,
  0xdc0,
  0xedd,
  0x1000,
  0x1200,
  0x1312,
  0x1401,
  0x2c00,
  0xa800,
  0xf900,
  0xfa30,
  0xffda,
  0xffdc,
  0x10000,
  0x10300,
  0x10400,
  0x20000,
  0x2f800,
  0x2fa1d,
};

rune notletterTest[] = {
  0x20,
  0x35,
  0x375,
  0x619,
  0x700,
  0x1885,
  0xfffe,
  0x1ffff,
  0x10ffff,
};

// Contains all the special cased Latin-1 chars.
rune spaceTest[] = {
  0x09,
  0x0a,
  0x0b,
  0x0c,
  0x0d,
  0x20,
  0x85,
  0xA0,
  0x2000,
  0x3000,
};

typedef struct caseT {
  int cas;
  rune in, out;
} caseT;

caseT caseTest[] = {
  // errors
  {-1, '\n', 0xFFFD},
  {UpperCase, -1, -1},
  {UpperCase, 1 << 30, 1 << 30},

  // ASCII (special-cased so test carefully)
  {UpperCase, '\n', '\n'},
  {UpperCase, 'a', 'A'},
  {UpperCase, 'A', 'A'},
  {UpperCase, '7', '7'},
  {LowerCase, '\n', '\n'},
  {LowerCase, 'a', 'a'},
  {LowerCase, 'A', 'a'},
  {LowerCase, '7', '7'},
  {TitleCase, '\n', '\n'},
  {TitleCase, 'a', 'A'},
  {TitleCase, 'A', 'A'},
  {TitleCase, '7', '7'},

  // Latin-1: easy to read the tests!
  {UpperCase, 0x80, 0x80},
  /*{UpperCase, 'Å', 'Å'},*/
  /*{UpperCase, 'å', 'Å'},*/
  {LowerCase, 0x80, 0x80},
  /*{LowerCase, 'Å', 'å'},*/
  /*{LowerCase, 'å', 'å'},*/
  {TitleCase, 0x80, 0x80},
  /*{TitleCase, 'Å', 'Å'},*/
  /*{TitleCase, 'å', 'Å'},*/

  // 0131;LATIN SMALL LETTER DOTLESS I;Ll;0;L;;;;;N;;;0049;;0049
  {UpperCase, 0x0131, 'I'},
  {LowerCase, 0x0131, 0x0131},
  {TitleCase, 0x0131, 'I'},

  // 0133;LATIN SMALL LIGATURE IJ;Ll;0;L;<compat> 0069 006A;;;;N;LATIN SMALL LETTER I J;;0132;;0132
  {UpperCase, 0x0133, 0x0132},
  {LowerCase, 0x0133, 0x0133},
  {TitleCase, 0x0133, 0x0132},

  // 212A;KELVIN SIGN;Lu;0;L;004B;;;;N;DEGREES KELVIN;;;006B;
  {UpperCase, 0x212A, 0x212A},
  {LowerCase, 0x212A, 'k'},
  {TitleCase, 0x212A, 0x212A},

  // From an UpperLower sequence
  // A640;CYRILLIC CAPITAL LETTER ZEMLYA;Lu;0;L;;;;;N;;;;A641;
  {UpperCase, 0xA640, 0xA640},
  {LowerCase, 0xA640, 0xA641},
  {TitleCase, 0xA640, 0xA640},
  // A641;CYRILLIC SMALL LETTER ZEMLYA;Ll;0;L;;;;;N;;;A640;;A640
  {UpperCase, 0xA641, 0xA640},
  {LowerCase, 0xA641, 0xA641},
  {TitleCase, 0xA641, 0xA640},
  // A64E;CYRILLIC CAPITAL LETTER NEUTRAL YER;Lu;0;L;;;;;N;;;;A64F;
  {UpperCase, 0xA64E, 0xA64E},
  {LowerCase, 0xA64E, 0xA64F},
  {TitleCase, 0xA64E, 0xA64E},
  // A65F;CYRILLIC SMALL LETTER YN;Ll;0;L;;;;;N;;;A65E;;A65E
  {UpperCase, 0xA65F, 0xA65E},
  {LowerCase, 0xA65F, 0xA65F},
  {TitleCase, 0xA65F, 0xA65E},

  // From another UpperLower sequence
  // 0139;LATIN CAPITAL LETTER L WITH ACUTE;Lu;0;L;004C 0301;;;;N;LATIN CAPITAL LETTER L ACUTE;;;013A;
  {UpperCase, 0x0139, 0x0139},
  {LowerCase, 0x0139, 0x013A},
  {TitleCase, 0x0139, 0x0139},
  // 013F;LATIN CAPITAL LETTER L WITH MIDDLE DOT;Lu;0;L;<compat> 004C 00B7;;;;N;;;;0140;
  {UpperCase, 0x013f, 0x013f},
  {LowerCase, 0x013f, 0x0140},
  {TitleCase, 0x013f, 0x013f},
  // 0148;LATIN SMALL LETTER N WITH CARON;Ll;0;L;006E 030C;;;;N;LATIN SMALL LETTER N HACEK;;0147;;0147
  {UpperCase, 0x0148, 0x0147},
  {LowerCase, 0x0148, 0x0148},
  {TitleCase, 0x0148, 0x0147},

  // Lowercase lower than uppercase.
  // AB78;CHEROKEE SMALL LETTER GE;Ll;0;L;;;;;N;;;13A8;;13A8
  {UpperCase, 0xab78, 0x13a8},
  {LowerCase, 0xab78, 0xab78},
  {TitleCase, 0xab78, 0x13a8},
  {UpperCase, 0x13a8, 0x13a8},
  {LowerCase, 0x13a8, 0xab78},
  {TitleCase, 0x13a8, 0x13a8},

  // Last block in the 5.1.0 table
  // 10400;DESERET CAPITAL LETTER LONG I;Lu;0;L;;;;;N;;;;10428;
  {UpperCase, 0x10400, 0x10400},
  {LowerCase, 0x10400, 0x10428},
  {TitleCase, 0x10400, 0x10400},
  // 10427;DESERET CAPITAL LETTER EW;Lu;0;L;;;;;N;;;;1044F;
  {UpperCase, 0x10427, 0x10427},
  {LowerCase, 0x10427, 0x1044F},
  {TitleCase, 0x10427, 0x10427},
  // 10428;DESERET SMALL LETTER LONG I;Ll;0;L;;;;;N;;;10400;;10400
  {UpperCase, 0x10428, 0x10400},
  {LowerCase, 0x10428, 0x10428},
  {TitleCase, 0x10428, 0x10400},
  // 1044F;DESERET SMALL LETTER EW;Ll;0;L;;;;;N;;;10427;;10427
  {UpperCase, 0x1044F, 0x10427},
  {LowerCase, 0x1044F, 0x1044F},
  {TitleCase, 0x1044F, 0x10427},

  // First one not in the 5.1.0 table
  // 10450;SHAVIAN LETTER PEEP;Lo;0;L;;;;;N;;;;;
  {UpperCase, 0x10450, 0x10450},
  {LowerCase, 0x10450, 0x10450},
  {TitleCase, 0x10450, 0x10450},

  // Non-letters with case.
  {LowerCase, 0x2161, 0x2171},
  {UpperCase, 0x0345, 0x0399},
};

testCase(testIsLetter) {
  forEach(rune, upperTest, r) {
    zz_assertf(isLetter(*r), "isLetter(U+%04X) = false, want true", *r);
  }
  forEach(rune, letterTest, r) {
    zz_assertf(isLetter(*r), "isLetter(U+%04X) = false, want true", *r);
  }
  forEach(rune, notletterTest, r) {
    zz_assertf(!isLetter(*r), "isLetter(U+%04X) = true, want false", *r);
  }
}
