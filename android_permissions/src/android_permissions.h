#if defined(DM_PLATFORM_ANDROID)

#include <dmsdk/sdk.h>

static int AndroidPermissions_PermissionGranted;
static int AndroidPermissions_PermissionDenied;

void AndroidPermissions_Initialize();
bool AndroidPermissions_IsPermissionGranted(const char* permission);
void AndroidPermissions_RequestPermission(const char** permissions, const size_t& permissionsCount, lua_State* L, const int& callbackRef);
bool AndroidPermissions_ShouldShowRequestPermissionRationale(const char* permission);

#endif