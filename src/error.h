#ifndef ERROR_H
#define ERROR_H

typedef struct error error;

const char *errorMsg();

extern error *errEOF;
extern error *errNoProgress;

#endif // ERROR_H
