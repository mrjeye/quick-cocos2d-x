/*
** Lua binding: FilterSprite_luabinding
** Generated automatically by tolua++-1.0.92 on Tue Mar 11 11:34:16 2014.
*/

#include "FilterSprite_luabinding.h"
#include "CCLuaEngine.h"

using namespace cocos2d;




#include "FilterSprite.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"FilterSprite"); toluafix_add_type_mapping(typeid(FilterSprite).hash_code(), "FilterSprite");
}

/* method: create of class  FilterSprite */
#ifndef TOLUA_DISABLE_tolua_FilterSprite_luabinding_FilterSprite_create00
static int tolua_FilterSprite_luabinding_FilterSprite_create00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"FilterSprite",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* pszFileName = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   FilterSprite* tolua_ret = (FilterSprite*)  FilterSprite::create(pszFileName);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"FilterSprite");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'create'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_FilterSprite_luabinding_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"FilterSprite","FilterSprite","",NULL);
  tolua_beginmodule(tolua_S,"FilterSprite");
   tolua_function(tolua_S,"create",tolua_FilterSprite_luabinding_FilterSprite_create00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_FilterSprite_luabinding (lua_State* tolua_S) {
 return tolua_FilterSprite_luabinding_open(tolua_S);
};
#endif

