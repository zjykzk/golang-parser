#ifndef UNICODE_GROUPS_H
#define UNICODE_GROUPS_H

#include "def.h"

typedef struct Range16 {
  u16 lo;
  u16 lu;
  u16 stride;
} Range16;

typedef struct Range32 {
  u32 lo;
  u32 lu;
  u32 stride;
} Range32;

typedef struct RangeTable {
  const Range16 *r16;
  int r16Len;
  int latinOffset;

  const Range32 *r32;
  int r32Len;
} RangeTable;

extern const RangeTable *letterTable;

#endif // UNICODE_GROUPS_H
