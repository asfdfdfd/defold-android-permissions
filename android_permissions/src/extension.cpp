#include <dmsdk/sdk.h>

#include "android_permissions.h"

#define MODULE_NAME "android_permissions"

#if defined(DM_PLATFORM_ANDROID)

static int Lua_IsPermissionGranted(lua_State* L) {
    const char* permission = lua_tostring(L, 1);
    bool isPermissionGranted = AndroidPermissions_IsPermissionGranted(permission);
    lua_pushboolean(L, isPermissionGranted);
    return 1;    
}

static int Lua_RequestPermission(lua_State* L) {    
    int permissionsCount = lua_objlen(L, 1);    
        
    const char** permissionsArray = (const char**)malloc(sizeof(const char*) * permissionsCount);
    
    for (int permissionIndex = 0; permissionIndex < permissionsCount; permissionIndex++) {
        lua_rawgeti(L, 1, permissionIndex + 1);
        permissionsArray[permissionIndex] = lua_tostring(L, -1);
        lua_pop(L, 1);
    }
    
    int callbackRef = luaL_ref(L, LUA_REGISTRYINDEX); 
    // TODO: Clear memory?
    AndroidPermissions_RequestPermission(permissionsArray, permissionsCount, L, callbackRef);
    
    free(permissionsArray);
            
    return 0;
}

static int Lua_ShouldShowRequestPermissionRationale(lua_State* L) {
    const char* permission = lua_tostring(L, 1);
    bool shouldShowRequestPermissionRationale = AndroidPermissions_ShouldShowRequestPermissionRationale(permission);
    lua_pushboolean(L, shouldShowRequestPermissionRationale);
    return 1;
}

static const luaL_reg AndroidPermissionsLuaMethods[] = {
    {"is_permission_granted", Lua_IsPermissionGranted},
    {"request_permissions", Lua_RequestPermission},
    {"should_show_request_permission_rationale", Lua_ShouldShowRequestPermissionRationale},
    {0, 0}
};

static void LuaInit(lua_State* L) {
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, AndroidPermissionsLuaMethods);

    #define SETCONSTANT(name, val) \
            lua_pushnumber(L, (lua_Number) val); \
            lua_setfield(L, -2, #name);
        
    SETCONSTANT(PERMISSION_GRANTED, AndroidPermissions_PermissionGranted);
    SETCONSTANT(PERMISSION_DENIED, AndroidPermissions_PermissionDenied);
    
    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result InitializeAndroidPermissionsExtension(dmExtension::Params* params) {
    AndroidPermissions_Initialize();

    LuaInit(params->m_L);

    printf("Registered %s Extension\n", MODULE_NAME);

    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeAndroidPermissionsExtension(dmExtension::Params* params) {
    // AndroidPermissionsFinalize();  
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(AndroidPermissions, "AndroidPermissions", NULL, NULL, InitializeAndroidPermissionsExtension, NULL, NULL, FinalizeAndroidPermissionsExtension)

#else 

dmExtension::Result InitializeAndroidPermissionsExtension(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeAndroidPermissionsExtension(dmExtension::Params* params) {
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(AndroidPermissions, "AndroidPermissions", NULL, NULL, InitializeAndroidPermissionsExtension, NULL, NULL, FinalizeAndroidPermissionsExtension)

#endif