/* trace.h */

#include <stdio.h>

#define TERM_RED   "\x1B[31m"
#define TERM_GRN   "\x1B[32m"
#define TERM_YEL   "\x1B[33m"
#define TERM_BLU   "\x1B[34m"
#define TERM_MAG   "\x1B[35m"
#define TERM_CYN   "\x1B[36m"
#define TERM_WHT   "\x1B[37m"
#define TERM_RESET "\x1B[0m"

#define CSOL_INFO(x, ...)       do{fprintf(stdout, TERM_GRN "INFO: %s: " x "\n" TERM_RESET, __func__, ##__VA_ARGS__);}while(0)
#ifdef _DEBUG
#define CSOL_WARNING(x, ...)    do{fprintf(stderr, TERM_YEL "WARNING: %s: " x "\n" TERM_RESET, __func__, ##__VA_ARGS__);}while(0)
#define CSOL_ERROR(x, ...)      do{fprintf(stderr, TERM_RED "ERROR: %s: " x "\n" TERM_RESET, __func__, ##__VA_ARGS__);}while(0)
#else
#define CSOL_WARNING(x, ...)
#define CSOL_ERROR(x, ...)
#endif
