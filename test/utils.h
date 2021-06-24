#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdint.h>

/* assert */
#define zz_assert(expr)                                                       \
    if (!(expr)) {                                                            \
        printf("Assertion failed: %s (%s: %d)\n", #expr, __FILE__, __LINE__); \
        abort();                                                              \
    }

#define zz_assertf(expr, ...)                                                 \
    if (!(expr)) {                                                            \
        printf("Assertion failed: %s (%s: %d)\n", #expr, __FILE__, __LINE__); \
        printf(__VA_ARGS__);                                                  \
        printf("\n");                                                         \
        abort();                                                              \
    }

#define testCase(name)                      \
  void name(void);                          \
  int main(int argc, const char * argv[]) { \
    name();                                 \
    return 0;                               \
  }                                         \
  void name(void)

#define forEach(type, arr, v)                                     \
  for (type *v = &arr[0], *end = &arr[0] + sizeof(arr)/sizeof(arr[0]); v < end; v++)

#endif // UTILS_H
