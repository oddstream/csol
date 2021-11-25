/* trace.h */

#include <stdio.h>

#define CSOL_INFO(x, ...)       do{fprintf(stdout, "INFO: %s: " x "\n", __func__, ##__VA_ARGS__);}while(0)
#ifdef _DEBUG
#define CSOL_WARNING(x, ...)    do{fprintf(stderr, "\x1B[33mWARNING: %s: " x "\n", __func__, ##__VA_ARGS__);}while(0)
#define CSOL_ERROR(x, ...)      do{fprintf(stderr, "\x1B[31mERROR: %s: " x "\n", __func__, ##__VA_ARGS__);}while(0)
#else
#define CSOL_WARNING(x, ...)
#define CSOL_ERROR(x, ...)
#endif
