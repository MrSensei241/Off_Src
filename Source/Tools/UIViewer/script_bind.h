/*
** Lua binding: script
** Generated automatically by tolua++-1.0.2 on 10/05/11 15:49:52.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int tolua_script_open (lua_State* tolua_S);


/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_BOOL (lua_State* tolua_S)
{
 BOOL* self = (BOOL*) tolua_tousertype(tolua_S,1,0);
 delete self;
 return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"BOOL");
}

/* function: Open */
static int tolua_script_Open00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isstring(tolua_S,1,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,1,0));
  int argNum = ((int)  tolua_tonumber(tolua_S,2,-1));
 {
  Open(name,argNum);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Open'.",&tolua_err);
 return 0;
#endif
}

/* function: SetBasePath */
static int tolua_script_SetBasePath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isstring(tolua_S,1,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const char* path = ((const char*)  tolua_tostring(tolua_S,1,0));
 {
  SetBasePath(path);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetBasePath'.",&tolua_err);
 return 0;
#endif
}

/* function: GetBasePath */
static int tolua_script_GetBasePath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  const char* tolua_ret = (const char*)  GetBasePath();
 tolua_pushstring(tolua_S,(const char*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBasePath'.",&tolua_err);
 return 0;
#endif
}

/* function: GetFrameXMLCount */
static int tolua_script_GetFrameXMLCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  int tolua_ret = (int)  GetFrameXMLCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFrameXMLCount'.",&tolua_err);
 return 0;
#endif
}

/* function: GetFrameXMLName */
static int tolua_script_GetFrameXMLName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  int index = ((int)  tolua_tonumber(tolua_S,1,0));
 {
  const char* tolua_ret = (const char*)  GetFrameXMLName(index);
 tolua_pushstring(tolua_S,(const char*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFrameXMLName'.",&tolua_err);
 return 0;
#endif
}

/* function: CheckChildDirectory */
static int tolua_script_CheckChildDirectory00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  BOOL tolua_ret = (BOOL)  CheckChildDirectory();
 {
#ifdef __cplusplus
 void* tolua_obj = new BOOL(tolua_ret);
 tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"BOOL");
#else
 void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(BOOL));
 tolua_pushusertype_and_takeownership(tolua_S,tolua_obj,"BOOL");
#endif
 }
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CheckChildDirectory'.",&tolua_err);
 return 0;
#endif
}

/* Open function */
TOLUA_API int tolua_script_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_function(tolua_S,"Open",tolua_script_Open00);
 tolua_function(tolua_S,"SetBasePath",tolua_script_SetBasePath00);
 tolua_function(tolua_S,"GetBasePath",tolua_script_GetBasePath00);
 tolua_function(tolua_S,"GetFrameXMLCount",tolua_script_GetFrameXMLCount00);
 tolua_function(tolua_S,"GetFrameXMLName",tolua_script_GetFrameXMLName00);
 tolua_function(tolua_S,"CheckChildDirectory",tolua_script_CheckChildDirectory00);
 tolua_endmodule(tolua_S);
 return 1;
}
