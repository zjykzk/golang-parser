#include "unicode.h"
#include "unicode_groups.h"

const int maxLatin1 = 0xff;

// Bit masks for each code point under U+0100, for fast lookup.
enum {
  pC     = 1,        // a control character.
  pP     = 2,        // a punctuation character.
  pN     = 4,        // a numeral.
  pS     = 8,        // a symbolic character.
  pZ     = 16,       // a spacing character.
  pLu    = 32,       // an upper-case letter.
  pLl    = 64,       // a lower-case letter.
  pp     = 128,      // a printable character according to Go's definition.
  pg     = pp | pZ,  // a graphical character according to the Unicode definition.
  pLo    = pLl | pLu,// a letter that is neither upper nor lower case.
  pLmask = pLo
};

static u8 properties[] = {
  [0x00] = pC,       // '\x00'
  [0x01] = pC,       // '\x01'
  [0x02] = pC,       // '\x02'
  [0x03] = pC,       // '\x03'
  [0x04] = pC,       // '\x04'
  [0x05] = pC,       // '\x05'
  [0x06] = pC,       // '\x06'
  [0x07] = pC,       // '\a'
  [0x08] = pC,       // '\b'
  [0x09] = pC,       // '\t'
  [0x0A] = pC,       // '\n'
  [0x0B] = pC,       // '\v'
  [0x0C] = pC,       // '\f'
  [0x0D] = pC,       // '\r'
  [0x0E] = pC,       // '\x0e'
  [0x0F] = pC,       // '\x0f'
  [0x10] = pC,       // '\x10'
  [0x11] = pC,       // '\x11'
  [0x12] = pC,       // '\x12'
  [0x13] = pC,       // '\x13'
  [0x14] = pC,       // '\x14'
  [0x15] = pC,       // '\x15'
  [0x16] = pC,       // '\x16'
  [0x17] = pC,       // '\x17'
  [0x18] = pC,       // '\x18'
  [0x19] = pC,       // '\x19'
  [0x1A] = pC,       // '\x1a'
  [0x1B] = pC,       // '\x1b'
  [0x1C] = pC,       // '\x1c'
  [0x1D] = pC,       // '\x1d'
  [0x1E] = pC,       // '\x1e'
  [0x1F] = pC,       // '\x1f'
  [0x20] = pZ | pp,  // ' '
  [0x21] = pP | pp,  // '!'
  [0x22] = pP | pp,  // '"'
  [0x23] = pP | pp,  // '#'
  [0x24] = pS | pp,  // '$'
  [0x25] = pP | pp,  // '%'
  [0x26] = pP | pp,  // '&'
  [0x27] = pP | pp,  // '\''
  [0x28] = pP | pp,  // '('
  [0x29] = pP | pp,  // ')'
  [0x2A] = pP | pp,  // '*'
  [0x2B] = pS | pp,  // '+'
  [0x2C] = pP | pp,  // ','
  [0x2D] = pP | pp,  // '-'
  [0x2E] = pP | pp,  // '.'
  [0x2F] = pP | pp,  // '/'
  [0x30] = pN | pp,  // '0'
  [0x31] = pN | pp,  // '1'
  [0x32] = pN | pp,  // '2'
  [0x33] = pN | pp,  // '3'
  [0x34] = pN | pp,  // '4'
  [0x35] = pN | pp,  // '5'
  [0x36] = pN | pp,  // '6'
  [0x37] = pN | pp,  // '7'
  [0x38] = pN | pp,  // '8'
  [0x39] = pN | pp,  // '9'
  [0x3A] = pP | pp,  // ':'
  [0x3B] = pP | pp,  // ';'
  [0x3C] = pS | pp,  // '<'
  [0x3D] = pS | pp,  // '='
  [0x3E] = pS | pp,  // '>'
  [0x3F] = pP | pp,  // '?'
  [0x40] = pP | pp,  // '@'
  [0x41] = pLu | pp, // 'A'
  [0x42] = pLu | pp, // 'B'
  [0x43] = pLu | pp, // 'C'
  [0x44] = pLu | pp, // 'D'
  [0x45] = pLu | pp, // 'E'
  [0x46] = pLu | pp, // 'F'
  [0x47] = pLu | pp, // 'G'
  [0x48] = pLu | pp, // 'H'
  [0x49] = pLu | pp, // 'I'
  [0x4A] = pLu | pp, // 'J'
  [0x4B] = pLu | pp, // 'K'
  [0x4C] = pLu | pp, // 'L'
  [0x4D] = pLu | pp, // 'M'
  [0x4E] = pLu | pp, // 'N'
  [0x4F] = pLu | pp, // 'O'
  [0x50] = pLu | pp, // 'P'
  [0x51] = pLu | pp, // 'Q'
  [0x52] = pLu | pp, // 'R'
  [0x53] = pLu | pp, // 'S'
  [0x54] = pLu | pp, // 'T'
  [0x55] = pLu | pp, // 'U'
  [0x56] = pLu | pp, // 'V'
  [0x57] = pLu | pp, // 'W'
  [0x58] = pLu | pp, // 'X'
  [0x59] = pLu | pp, // 'Y'
  [0x5A] = pLu | pp, // 'Z'
  [0x5B] = pP | pp,  // '['
  [0x5C] = pP | pp,  // '\\'
  [0x5D] = pP | pp,  // ']'
  [0x5E] = pS | pp,  // '^'
  [0x5F] = pP | pp,  // '_'
  [0x60] = pS | pp,  // '`'
  [0x61] = pLl | pp, // 'a'
  [0x62] = pLl | pp, // 'b'
  [0x63] = pLl | pp, // 'c'
  [0x64] = pLl | pp, // 'd'
  [0x65] = pLl | pp, // 'e'
  [0x66] = pLl | pp, // 'f'
  [0x67] = pLl | pp, // 'g'
  [0x68] = pLl | pp, // 'h'
  [0x69] = pLl | pp, // 'i'
  [0x6A] = pLl | pp, // 'j'
  [0x6B] = pLl | pp, // 'k'
  [0x6C] = pLl | pp, // 'l'
  [0x6D] = pLl | pp, // 'm'
  [0x6E] = pLl | pp, // 'n'
  [0x6F] = pLl | pp, // 'o'
  [0x70] = pLl | pp, // 'p'
  [0x71] = pLl | pp, // 'q'
  [0x72] = pLl | pp, // 'r'
  [0x73] = pLl | pp, // 's'
  [0x74] = pLl | pp, // 't'
  [0x75] = pLl | pp, // 'u'
  [0x76] = pLl | pp, // 'v'
  [0x77] = pLl | pp, // 'w'
  [0x78] = pLl | pp, // 'x'
  [0x79] = pLl | pp, // 'y'
  [0x7A] = pLl | pp, // 'z'
  [0x7B] = pP | pp,  // '{'
  [0x7C] = pS | pp,  // '|'
  [0x7D] = pP | pp,  // '}'
  [0x7E] = pS | pp,  // '~'
  [0x7F] = pC,       // '\u007f'
  [0x80] = pC,       // '\u0080'
  [0x81] = pC,       // '\u0081'
  [0x82] = pC,       // '\u0082'
  [0x83] = pC,       // '\u0083'
  [0x84] = pC,       // '\u0084'
  [0x85] = pC,       // '\u0085'
  [0x86] = pC,       // '\u0086'
  [0x87] = pC,       // '\u0087'
  [0x88] = pC,       // '\u0088'
  [0x89] = pC,       // '\u0089'
  [0x8A] = pC,       // '\u008a'
  [0x8B] = pC,       // '\u008b'
  [0x8C] = pC,       // '\u008c'
  [0x8D] = pC,       // '\u008d'
  [0x8E] = pC,       // '\u008e'
  [0x8F] = pC,       // '\u008f'
  [0x90] = pC,       // '\u0090'
  [0x91] = pC,       // '\u0091'
  [0x92] = pC,       // '\u0092'
  [0x93] = pC,       // '\u0093'
  [0x94] = pC,       // '\u0094'
  [0x95] = pC,       // '\u0095'
  [0x96] = pC,       // '\u0096'
  [0x97] = pC,       // '\u0097'
  [0x98] = pC,       // '\u0098'
  [0x99] = pC,       // '\u0099'
  [0x9A] = pC,       // '\u009a'
  [0x9B] = pC,       // '\u009b'
  [0x9C] = pC,       // '\u009c'
  [0x9D] = pC,       // '\u009d'
  [0x9E] = pC,       // '\u009e'
  [0x9F] = pC,       // '\u009f'
  [0xA0] = pZ,       // '\u00a0'
  [0xA1] = pP | pp,  // '¡'
  [0xA2] = pS | pp,  // '¢'
  [0xA3] = pS | pp,  // '£'
  [0xA4] = pS | pp,  // '¤'
  [0xA5] = pS | pp,  // '¥'
  [0xA6] = pS | pp,  // '¦'
  [0xA7] = pP | pp,  // '§'
  [0xA8] = pS | pp,  // '¨'
  [0xA9] = pS | pp,  // '©'
  [0xAA] = pLo | pp, // 'ª'
  [0xAB] = pP | pp,  // '«'
  [0xAC] = pS | pp,  // '¬'
  [0xAD] = 0,        // '\u00ad'
  [0xAE] = pS | pp,  // '®'
  [0xAF] = pS | pp,  // '¯'
  [0xB0] = pS | pp,  // '°'
  [0xB1] = pS | pp,  // '±'
  [0xB2] = pN | pp,  // '²'
  [0xB3] = pN | pp,  // '³'
  [0xB4] = pS | pp,  // '´'
  [0xB5] = pLl | pp, // 'µ'
  [0xB6] = pP | pp,  // '¶'
  [0xB7] = pP | pp,  // '·'
  [0xB8] = pS | pp,  // '¸'
  [0xB9] = pN | pp,  // '¹'
  [0xBA] = pLo | pp, // 'º'
  [0xBB] = pP | pp,  // '»'
  [0xBC] = pN | pp,  // '¼'
  [0xBD] = pN | pp,  // '½'
  [0xBE] = pN | pp,  // '¾'
  [0xBF] = pP | pp,  // '¿'
  [0xC0] = pLu | pp, // 'À'
  [0xC1] = pLu | pp, // 'Á'
  [0xC2] = pLu | pp, // 'Â'
  [0xC3] = pLu | pp, // 'Ã'
  [0xC4] = pLu | pp, // 'Ä'
  [0xC5] = pLu | pp, // 'Å'
  [0xC6] = pLu | pp, // 'Æ'
  [0xC7] = pLu | pp, // 'Ç'
  [0xC8] = pLu | pp, // 'È'
  [0xC9] = pLu | pp, // 'É'
  [0xCA] = pLu | pp, // 'Ê'
  [0xCB] = pLu | pp, // 'Ë'
  [0xCC] = pLu | pp, // 'Ì'
  [0xCD] = pLu | pp, // 'Í'
  [0xCE] = pLu | pp, // 'Î'
  [0xCF] = pLu | pp, // 'Ï'
  [0xD0] = pLu | pp, // 'Ð'
  [0xD1] = pLu | pp, // 'Ñ'
  [0xD2] = pLu | pp, // 'Ò'
  [0xD3] = pLu | pp, // 'Ó'
  [0xD4] = pLu | pp, // 'Ô'
  [0xD5] = pLu | pp, // 'Õ'
  [0xD6] = pLu | pp, // 'Ö'
  [0xD7] = pS | pp,  // '×'
  [0xD8] = pLu | pp, // 'Ø'
  [0xD9] = pLu | pp, // 'Ù'
  [0xDA] = pLu | pp, // 'Ú'
  [0xDB] = pLu | pp, // 'Û'
  [0xDC] = pLu | pp, // 'Ü'
  [0xDD] = pLu | pp, // 'Ý'
  [0xDE] = pLu | pp, // 'Þ'
  [0xDF] = pLl | pp, // 'ß'
  [0xE0] = pLl | pp, // 'à'
  [0xE1] = pLl | pp, // 'á'
  [0xE2] = pLl | pp, // 'â'
  [0xE3] = pLl | pp, // 'ã'
  [0xE4] = pLl | pp, // 'ä'
  [0xE5] = pLl | pp, // 'å'
  [0xE6] = pLl | pp, // 'æ'
  [0xE7] = pLl | pp, // 'ç'
  [0xE8] = pLl | pp, // 'è'
  [0xE9] = pLl | pp, // 'é'
  [0xEA] = pLl | pp, // 'ê'
  [0xEB] = pLl | pp, // 'ë'
  [0xEC] = pLl | pp, // 'ì'
  [0xED] = pLl | pp, // 'í'
  [0xEE] = pLl | pp, // 'î'
  [0xEF] = pLl | pp, // 'ï'
  [0xF0] = pLl | pp, // 'ð'
  [0xF1] = pLl | pp, // 'ñ'
  [0xF2] = pLl | pp, // 'ò'
  [0xF3] = pLl | pp, // 'ó'
  [0xF4] = pLl | pp, // 'ô'
  [0xF5] = pLl | pp, // 'õ'
  [0xF6] = pLl | pp, // 'ö'
  [0xF7] = pS | pp,  // '÷'
  [0xF8] = pLl | pp, // 'ø'
  [0xF9] = pLl | pp, // 'ù'
  [0xFA] = pLl | pp, // 'ú'
  [0xFB] = pLl | pp, // 'û'
  [0xFC] = pLl | pp, // 'ü'
  [0xFD] = pLl | pp, // 'ý'
  [0xFE] = pLl | pp, // 'þ'
  [0xFF] = pLl | pp, // 'ÿ'
};

#define isSize(size)                                                           \
static int is##size(const Range##size *tab, size_t len, u##size r) {           \
  int s = 0, e = (int) len;                                                    \
  while (s < e) {                                                              \
    int m = s + (e - s) / 2;                                                   \
    u##size lo = tab[m].lo, lu = tab[m].lu;                                    \
    if (r >= lo && r <= lu) {                                                  \
      return tab[m].stride == 1 || (r - lo) % tab[m].stride == 0;              \
    }                                                                          \
                                                                               \
    if (r < lo) {                                                              \
      e = m;                                                                   \
      continue;                                                                \
    }                                                                          \
                                                                               \
    if (r > lu) {                                                              \
      s = m + 1;                                                               \
    }                                                                          \
  }                                                                            \
                                                                               \
  return 0;                                                                    \
}

isSize(16)
isSize(32)

static int isExcludingLatin(const RangeTable *tab, rune r) {
  if (tab->r16Len > 0 && tab->r16[tab->r16Len-1].lu >= r) {
    return is16(tab->r16, tab->r16Len, r);
  }

  return is32(tab->r32, tab->r32Len, r);
}

int isLetter(rune r) {
  if (r <= maxLatin1) {
    return (properties[r]&pLmask) != 0;
  }

  return isExcludingLatin(letterTable, r);
}
