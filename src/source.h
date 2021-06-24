#ifndef SOURCE_H
#define SOURCE_H

#include "def.h"

typedef struct source source;

void nextCh(source * source);
source * newSource(str filename, void (*errh)(uint line, uint col, str msg));

#endif // SOURCE_H
