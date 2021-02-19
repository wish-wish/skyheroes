#include "scripting/lua-bindings/auto/lua_sky_rad_auto.hpp"
#include "SpriteWithHue.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"

int lua_sky_rad_SpriteWithHue_initWithTexture(lua_State* tolua_S)
{
    int argc = 0;
    SpriteWithHue* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (SpriteWithHue*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_sky_rad_SpriteWithHue_initWithTexture'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            cocos2d::Texture2D* arg0;
            ok &= luaval_to_object<cocos2d::Texture2D>(tolua_S, 2, "cc.Texture2D",&arg0, "SpriteWithHue:initWithTexture");

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "SpriteWithHue:initWithTexture");

            if (!ok) { break; }
            bool ret = cobj->initWithTexture(arg0, arg1);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            cocos2d::Texture2D* arg0;
            ok &= luaval_to_object<cocos2d::Texture2D>(tolua_S, 2, "cc.Texture2D",&arg0, "SpriteWithHue:initWithTexture");

            if (!ok) { break; }
            bool ret = cobj->initWithTexture(arg0);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 3) {
            cocos2d::Texture2D* arg0;
            ok &= luaval_to_object<cocos2d::Texture2D>(tolua_S, 2, "cc.Texture2D",&arg0, "SpriteWithHue:initWithTexture");

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "SpriteWithHue:initWithTexture");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "SpriteWithHue:initWithTexture");

            if (!ok) { break; }
            bool ret = cobj->initWithTexture(arg0, arg1, arg2);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "SpriteWithHue:initWithTexture",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_initWithTexture'.",&tolua_err);
#endif

    return 0;
}
int lua_sky_rad_SpriteWithHue_setupDefaultSettings(lua_State* tolua_S)
{
    int argc = 0;
    SpriteWithHue* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (SpriteWithHue*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_sky_rad_SpriteWithHue_setupDefaultSettings'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_setupDefaultSettings'", nullptr);
            return 0;
        }
        cobj->setupDefaultSettings();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "SpriteWithHue:setupDefaultSettings",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_setupDefaultSettings'.",&tolua_err);
#endif

    return 0;
}
int lua_sky_rad_SpriteWithHue_getHue(lua_State* tolua_S)
{
    int argc = 0;
    SpriteWithHue* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (SpriteWithHue*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_sky_rad_SpriteWithHue_getHue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_getHue'", nullptr);
            return 0;
        }
        double ret = cobj->getHue();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "SpriteWithHue:getHue",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_getHue'.",&tolua_err);
#endif

    return 0;
}
int lua_sky_rad_SpriteWithHue_initWithSpriteFrame(lua_State* tolua_S)
{
    int argc = 0;
    SpriteWithHue* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (SpriteWithHue*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_sky_rad_SpriteWithHue_initWithSpriteFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::SpriteFrame* arg0;

        ok &= luaval_to_object<cocos2d::SpriteFrame>(tolua_S, 2, "cc.SpriteFrame",&arg0, "SpriteWithHue:initWithSpriteFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_initWithSpriteFrame'", nullptr);
            return 0;
        }
        bool ret = cobj->initWithSpriteFrame(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "SpriteWithHue:initWithSpriteFrame",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_initWithSpriteFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_sky_rad_SpriteWithHue_shaderBody(lua_State* tolua_S)
{
    int argc = 0;
    SpriteWithHue* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (SpriteWithHue*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_sky_rad_SpriteWithHue_shaderBody'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_shaderBody'", nullptr);
            return 0;
        }
        const char* ret = cobj->shaderBody();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "SpriteWithHue:shaderBody",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_shaderBody'.",&tolua_err);
#endif

    return 0;
}
int lua_sky_rad_SpriteWithHue_initShader(lua_State* tolua_S)
{
    int argc = 0;
    SpriteWithHue* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (SpriteWithHue*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_sky_rad_SpriteWithHue_initShader'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_initShader'", nullptr);
            return 0;
        }
        cobj->initShader();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "SpriteWithHue:initShader",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_initShader'.",&tolua_err);
#endif

    return 0;
}
int lua_sky_rad_SpriteWithHue_getAlpha(lua_State* tolua_S)
{
    int argc = 0;
    SpriteWithHue* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (SpriteWithHue*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_sky_rad_SpriteWithHue_getAlpha'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_getAlpha'", nullptr);
            return 0;
        }
        double ret = cobj->getAlpha();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "SpriteWithHue:getAlpha",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_getAlpha'.",&tolua_err);
#endif

    return 0;
}
int lua_sky_rad_SpriteWithHue_setHue(lua_State* tolua_S)
{
    int argc = 0;
    SpriteWithHue* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (SpriteWithHue*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_sky_rad_SpriteWithHue_setHue'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "SpriteWithHue:setHue");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_setHue'", nullptr);
            return 0;
        }
        cobj->setHue(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "SpriteWithHue:setHue",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_setHue'.",&tolua_err);
#endif

    return 0;
}
int lua_sky_rad_SpriteWithHue_hueUniformCallback(lua_State* tolua_S)
{
    int argc = 0;
    SpriteWithHue* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (SpriteWithHue*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_sky_rad_SpriteWithHue_hueUniformCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        cocos2d::GLProgram* arg0;
        cocos2d::Uniform* arg1;

        ok &= luaval_to_object<cocos2d::GLProgram>(tolua_S, 2, "cc.GLProgram",&arg0, "SpriteWithHue:hueUniformCallback");

        #pragma warning NO CONVERSION TO NATIVE FOR Uniform*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_hueUniformCallback'", nullptr);
            return 0;
        }
        cobj->hueUniformCallback(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "SpriteWithHue:hueUniformCallback",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_hueUniformCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_sky_rad_SpriteWithHue_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 2)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "SpriteWithHue:create");
            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "SpriteWithHue:create");
            if (!ok) { break; }
            SpriteWithHue* ret = SpriteWithHue::create(arg0, arg1);
            object_to_luaval<SpriteWithHue>(tolua_S, "SpriteWithHue",(SpriteWithHue*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "SpriteWithHue:create");
            if (!ok) { break; }
            SpriteWithHue* ret = SpriteWithHue::create(arg0);
            object_to_luaval<SpriteWithHue>(tolua_S, "SpriteWithHue",(SpriteWithHue*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "SpriteWithHue:create",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_create'.",&tolua_err);
#endif
    return 0;
}
int lua_sky_rad_SpriteWithHue_createWithTexture(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 2)
        {
            cocos2d::Texture2D* arg0;
            ok &= luaval_to_object<cocos2d::Texture2D>(tolua_S, 2, "cc.Texture2D",&arg0, "SpriteWithHue:createWithTexture");
            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "SpriteWithHue:createWithTexture");
            if (!ok) { break; }
            SpriteWithHue* ret = SpriteWithHue::createWithTexture(arg0, arg1);
            object_to_luaval<SpriteWithHue>(tolua_S, "SpriteWithHue",(SpriteWithHue*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 3)
        {
            cocos2d::Texture2D* arg0;
            ok &= luaval_to_object<cocos2d::Texture2D>(tolua_S, 2, "cc.Texture2D",&arg0, "SpriteWithHue:createWithTexture");
            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "SpriteWithHue:createWithTexture");
            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "SpriteWithHue:createWithTexture");
            if (!ok) { break; }
            SpriteWithHue* ret = SpriteWithHue::createWithTexture(arg0, arg1, arg2);
            object_to_luaval<SpriteWithHue>(tolua_S, "SpriteWithHue",(SpriteWithHue*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            cocos2d::Texture2D* arg0;
            ok &= luaval_to_object<cocos2d::Texture2D>(tolua_S, 2, "cc.Texture2D",&arg0, "SpriteWithHue:createWithTexture");
            if (!ok) { break; }
            SpriteWithHue* ret = SpriteWithHue::createWithTexture(arg0);
            object_to_luaval<SpriteWithHue>(tolua_S, "SpriteWithHue",(SpriteWithHue*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "SpriteWithHue:createWithTexture",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_createWithTexture'.",&tolua_err);
#endif
    return 0;
}
int lua_sky_rad_SpriteWithHue_createWithSpriteFrameName(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "SpriteWithHue:createWithSpriteFrameName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_createWithSpriteFrameName'", nullptr);
            return 0;
        }
        SpriteWithHue* ret = SpriteWithHue::createWithSpriteFrameName(arg0);
        object_to_luaval<SpriteWithHue>(tolua_S, "SpriteWithHue",(SpriteWithHue*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "SpriteWithHue:createWithSpriteFrameName",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_createWithSpriteFrameName'.",&tolua_err);
#endif
    return 0;
}
int lua_sky_rad_SpriteWithHue_createWithSpriteFrame(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"SpriteWithHue",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        cocos2d::SpriteFrame* arg0;
        ok &= luaval_to_object<cocos2d::SpriteFrame>(tolua_S, 2, "cc.SpriteFrame",&arg0, "SpriteWithHue:createWithSpriteFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_sky_rad_SpriteWithHue_createWithSpriteFrame'", nullptr);
            return 0;
        }
        SpriteWithHue* ret = SpriteWithHue::createWithSpriteFrame(arg0);
        object_to_luaval<SpriteWithHue>(tolua_S, "SpriteWithHue",(SpriteWithHue*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "SpriteWithHue:createWithSpriteFrame",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_sky_rad_SpriteWithHue_createWithSpriteFrame'.",&tolua_err);
#endif
    return 0;
}
static int lua_sky_rad_SpriteWithHue_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (SpriteWithHue)");
    return 0;
}

int lua_register_sky_rad_SpriteWithHue(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"SpriteWithHue");
    tolua_cclass(tolua_S,"SpriteWithHue","SpriteWithHue","cc.Sprite",nullptr);

    tolua_beginmodule(tolua_S,"SpriteWithHue");
        tolua_function(tolua_S,"initWithTexture",lua_sky_rad_SpriteWithHue_initWithTexture);
        tolua_function(tolua_S,"setupDefaultSettings",lua_sky_rad_SpriteWithHue_setupDefaultSettings);
        tolua_function(tolua_S,"getHue",lua_sky_rad_SpriteWithHue_getHue);
        tolua_function(tolua_S,"initWithSpriteFrame",lua_sky_rad_SpriteWithHue_initWithSpriteFrame);
        tolua_function(tolua_S,"shaderBody",lua_sky_rad_SpriteWithHue_shaderBody);
        tolua_function(tolua_S,"initShader",lua_sky_rad_SpriteWithHue_initShader);
        tolua_function(tolua_S,"getAlpha",lua_sky_rad_SpriteWithHue_getAlpha);
        tolua_function(tolua_S,"setHue",lua_sky_rad_SpriteWithHue_setHue);
        tolua_function(tolua_S,"hueUniformCallback",lua_sky_rad_SpriteWithHue_hueUniformCallback);
        tolua_function(tolua_S,"create", lua_sky_rad_SpriteWithHue_create);
        tolua_function(tolua_S,"createWithTexture", lua_sky_rad_SpriteWithHue_createWithTexture);
        tolua_function(tolua_S,"createWithSpriteFrameName", lua_sky_rad_SpriteWithHue_createWithSpriteFrameName);
        tolua_function(tolua_S,"createWithSpriteFrame", lua_sky_rad_SpriteWithHue_createWithSpriteFrame);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(SpriteWithHue).name();
    g_luaType[typeName] = "SpriteWithHue";
    g_typeCast["SpriteWithHue"] = "SpriteWithHue";
    return 1;
}
TOLUA_API int register_all_sky_rad(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"rad",0);
	tolua_beginmodule(tolua_S,"rad");

	lua_register_sky_rad_SpriteWithHue(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

