/* util.h */

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#include <raylib.h>

typedef float (*EasingFunc)(float, float, float);

float UtilDistance(Vector2 a, Vector2 b);
float UtilLerp(float v0, float v1, float t);
float UtilEaseInSine(float v0, float v1, float t);
float UtilEaseInCubic(float v0, float v1, float t);
float UtilSmoothstep(float A, float B, float v);
float UtilSmootherstep(float A, float B, float v);
float UtilNormalize(float start, float finish, float value);
float UtilMapValue(float value, float fromMin, float fromMax, float toMin, float toMax);
float UtilOverlapArea(Rectangle a, Rectangle b);
char* UtilOrdToShortString(int ord);
char* UtilOrdToLongString(int ord);
char* UtilSuitToShortString(int suit);
char* UtilSuitToLongString(int suit);

uint32_t UtilHash(char *str);

_Bool UtilRectangleWithinRectangle(Rectangle inner, Rectangle outer);

#endif
