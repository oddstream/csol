/* settings.c */

#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <raylib.h>

#include "luautil.h"

extern float cardScale;
extern Color baizeColor;

void LoadSettings(int *windowWidth, int *windowHeight)
{
    lua_State *L = luaL_newstate();
    if (!L) {
        fprintf(stderr, "ERROR: %s: could not create Lua state\n", __func__);
        return;
    }
    luaL_openlibs(L);

    if ( luaL_loadfile(L, "csol.settings.lua") || lua_pcall(L, 0, 0, 0) ) {
        fprintf(stderr, "ERROR: %s: col.settings.lua: %s\n", __func__, lua_tostring(L, -1));
        lua_pop(L, 1);
    } else {
        *windowWidth = LuaUtilGetGlobalInt(L, "WindowWidth", 640);
        *windowHeight = LuaUtilGetGlobalInt(L, "WindowHeight", 480);
        cardScale = LuaUtilGetGlobalFloat(L, "CardScale", 1.0f);
        int typ = lua_getglobal(L, "BaizeColor");
        if ( typ == LUA_TTABLE ) {
            float r, g, b, a;
            r = LuaUtilGetFieldFloat(L, "red", 1);
            g = LuaUtilGetFieldFloat(L, "green", 1);
            b = LuaUtilGetFieldFloat(L, "blue", 1);
            a = LuaUtilGetFieldFloat(L, "alpha", 1);
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
