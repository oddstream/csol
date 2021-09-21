/* settings.c */

#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include <raylib.h>

#include "moon.h"

extern int windowWidth, windowHeight;
extern float cardScale;
extern Color baizeColor;
extern char variant[64];

void LoadSettings(void)
{
    lua_State *L = luaL_newstate();

    if ( luaL_loadfile(L, "csol.settings.lua") || lua_pcall(L, 0, 0, 0) ) {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    } else {
        windowWidth = MoonGetGlobalInt(L, "WindowWidth", 640);
        windowHeight = MoonGetGlobalInt(L, "WindowHeight", 480);
        cardScale = MoonGetGlobalFloat(L, "CardScale", 1.0f);
        strncpy(variant, MoonGetGlobalString(L, "Variant", "Klondike"), sizeof(variant)-1);
        int typ = lua_getglobal(L, "BaizeColor");
        if ( typ == LUA_TTABLE ) {
            float r, g, b, a;
            r = MoonGetFieldNumber(L, "red", 1);
            g = MoonGetFieldNumber(L, "green", 1);
            b = MoonGetFieldNumber(L, "blue", 1);
            a = MoonGetFieldNumber(L, "alpha", 1);
            baizeColor = (Color){.r=r*255, .g=g*255, .b=b*255, .a=a*255};
        } else {
            baizeColor = DARKGREEN;
        }
        lua_pop(L, 1);  // remove global (table)
    }

    lua_close(L);
}

void SaveSettings(void)
{

}
