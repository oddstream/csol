/* util.c */

#include <math.h>
#include <raylib.h>

#include "util.h"

float UtilDistance(Vector2 a, Vector2 b)
{
    double first = pow(b.x - a.x, 2);
    double second = pow(b.y - a.y, 2);
    return sqrt(first + second);
}

float UtilSmootherstep(float A, float B, float v)
{
    v = (v) * (v) * (v) * ((v)*((v) * 6.0f - 15.0f) + 10.0f);
    return (B * v) + (A * (1.0f - v));
}

float UtilOverlapArea(Rectangle a, Rectangle b)
{
    float x = fmaxf(0, fminf(a.x + a.width, b.x + b.width) - fmaxf(a.x, b.x));
    float y = fmaxf(0, fminf(a.y + a.height, b.y + b.height) - fmaxf(a.y, b.y));
    return x * y;
}

char* UtilOrdToShortString(int ord)
{
    static char* ords[14] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    return ords[ord];
}

char* UtilOrdToLongString(int ord)
{
    static char* ords[14] = {"", "Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"};
    return ords[ord];
}

char* UtilSuitToShortString(int suit)
{
    static char* suits[4] = {"C", "D", "H", "S"};
    return suits[suit];
}

char* UtilSuitToLongString(int suit)
{
    static char* suits[4] = {"Club", "Diamond", "Heart", "Spade"};
    return suits[suit];
}