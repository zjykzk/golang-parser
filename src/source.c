#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "def.h"
#include "error.h"
#include "utf8.h"

// The source buffer is accessed using three indices b (begin),
// r (read), and e (end):
//
// - If b >= 0, it points to the beginning of a segment of most
//   recently read characters (typically a Go literal).
//
// - r points to the byte immediately following the most recently
//   read character ch, which starts at r-chw.
//
// - e points to the byte immediately following the last byte that
//   was read into the buffer.
//
// The buffer content is terminated at buf[e] with the sentinel
// character utf8.RuneSelf. This makes it possible to test for
// the common case of ASCII characters with a single 'if' (see
// nextch method).
//
//                +------ content in use -------+
//                v                             v
// buf [...read...|...segment...|ch|...unread...|s|...free...]
//                ^             ^  ^            ^
//                |             |  |            |
//                b         r-chw  r            e
//
// Invariant: -1 <= b < r <= e < len(buf) && buf[e] == sentinel
typedef struct source {
  FILE *in;
  void (*errh)(uint line, uint col, const char *msg);

  u8 *buf;         // source buffer
  int len;

  error *ioerr;    // pending I/O error, or nil
  int b, r, e;     // buffer indices (see comment above)
  uint line, col;  // source position of ch (0-based)
  rune ch;         // most recently read character
  int chw;         // width of ch
} source;


#define sentinel 0x80 // ascii character

static void err(source *s, const char *msg) {
  s->errh(s->line + 1, s->col + 1, msg);
}

static void fill(source *s);

void nextCh(source *s) {
redo:
  s->r += s->chw;
  if (s->ch == '\n') {
    s->line++;
    s->col = 0;
  }

  // ASCII
  if (s->buf[s->r] < sentinel) {
    s->r++;
    s->chw = 1;
    if (s->buf[s->r] == 0) {
      err(s, "invalid NUL character");
      goto redo;
    }
    return;
  }

  // utf8
  // not enough bytes for utf8
  while (s->ioerr == NULL && (s->e - s->r) < UTFMax && !isFullRune(s->buf + s->r, s->e - s->r)) {
    // read utf8
    fill(s);
  }

  if (s->r == s->e) {
    if (s->ioerr != errEOF) {
      err(s, "I/O error");
    }

    // why -1 && 0?
    s->ch = -1;
    s->chw = 0;
    return;
  }

  decodeRune(s->buf, s->e - s->r, &s->ch, &s->chw);
  s->r += s->chw;

  if (s->ch == RuneError && s->chw == 1) {
    err(s, "invalid utf8 encoding");
    goto redo;
  }

  // BOM's are only allowed as the first character in a file
  if (s->ch == 0xfeff) {
    if (s->line > 0 || s->col > 0) {
      err(s, "invalid BOM in the middle of the file");
    }
    goto redo;
  }
}

static void fill(source *s) {
  int b = s->r;
  if (s->b >= 0) {
    b = s->b;
    s->b = 0;
  }

#define maxSize 4<<10
#define minSize 1<<20
#define nextSize(size) ((size)<(minSize)?(minSize):((size)<=(maxSize)?((size)<<1):((size)+(maxSize))))

  int len = s->e - s->r;
  if (len * 2 > s->len) {
    s->len = nextSize(len);
    s->buf = realloc(s->buf, s->len);
  } else {
    memcpy(s->buf, s->buf + s->r, s->e - s->r);
  }
  s->r -= len;
  s->e -= len;

  for (int i = 0; i < 10; i++) {
    size_t r = fread(s->buf + s->e, sizeof *s->buf, s->len - s->e, s->in);
    if (ferror(s->in)) {
      printf("read file failed, err:%d\n", ferror(s->in));
      exit(-1);
      return;
    }

    if (r == 0) {
      continue;
    }

    s->e += r;
    s->buf[s->e] = sentinel;
    if (feof(s->in)) {
      s->ioerr = errEOF;
    }
    return;
  }

  s->buf[s->e] = sentinel;
  s->ioerr = errNoProgress;
}
