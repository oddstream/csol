/* util.c */

#include <math.h>
#include <raylib.h>

#include "util.h"

// Formula is the square root of (x2 - x1)^2 + (y2 - y1)^2
float UtilDistance(Vector2 a, Vector2 b)
{
    double first = pow(b.x - a.x, 2);
    double second = pow(b.y - a.y, 2);
    return sqrt(first + second);
}

// Lerp see https://en.wikipedia.org/wiki/Linear_interpolation
float UtilLerp(float v0, float v1, float t)
{
    return (1-t)*v0 + t*v1;
}

// Smoothstep see http://sol.gfxile.net/interpolation/
float UtilSmoothstep(float A, float B, float v)
{
    v = (v) * (v) * (3.0f - 2.0f * (v));
    return (B * v) + (A * (1.0f - v));
}

// Smootherstep see http://sol.gfxile.net/interpolation/
float UtilSmootherstep(float A, float B, float v)
{
    v = (v) * (v) * (v) * ((v)*((v) * 6.0f - 15.0f) + 10.0f);
    return (B * v) + (A * (1.0f - v));
}

// Normalize is the opposite of lerp. Instead of a range and a factor, we give a range and a value to find out the factor.
float UtilNormalize(float start, float finish, float value)
{
    return (value - start) / (finish - start);
}

// MapValue converts a value from the scale [fromMin, fromMax] to a value from the scale [toMin, toMax].
// It’s just the normalize and lerp functions working together.
float UtilMapValue(float value, float fromMin, float fromMax, float toMin, float toMax)
{
    return UtilLerp(toMin, toMax, UtilNormalize(fromMin, fromMax, value));
}

// OverlapArea returns the intersection area of two rectangles
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

/*
    djb2
    this algorithm (k=33) was first reported by dan bernstein many years ago in comp.lang.c. 
    another version of this algorithm (now favored by bernstein) uses xor: hash(i) = hash(i - 1) * 33 ^ str[i]; 
    the magic of number 33 (why it works better than many other constants, prime or not) has never been adequately explained.
*/
uint32_t UtilHash(char *str)
{
    uint32_t hash = 5381;
    while ( *str ) {
        // hash = hash * 33 + c;
        hash = ((hash << 5) + hash) + *str++;
    }
    return hash;
}

bool UtilRectangleWithinRectangle(Rectangle inner, Rectangle outer)
{
    if (inner.x < outer.x || inner.y < outer.y) {
        return false;
    }
    float innerRight = inner.x + inner.width;
    float outerRight = outer.x + outer.width;
    if (innerRight > outerRight) {
        return false;
    }
    float innerBottom = inner.y + inner.height;
    float outerBottom = outer.y + outer.height;
    if (innerBottom > outerBottom) {
        return false;
    }
    return true;
}