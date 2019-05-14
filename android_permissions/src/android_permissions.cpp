#if defined(DM_PLATFORM_ANDROID)

#include <dmsdk/sdk.h>

#include "djni.h"

namespace {
    struct PermissionRequestContext {
        lua_State* m_LuaState;
        int m_CallbackRef;        
        PermissionRequestContext(lua_State* L, int callbackRef) : m_LuaState(L), m_CallbackRef(callbackRef) {
        }
    };
}

static int AndroidPermissions_PermissionGranted = 0;
static int AndroidPermissions_PermissionDenied = 0;

bool AndroidPermissions_IsPermissionGranted(const char* permission) {
    JNIEnv* env = djni::env();
    jclass jclass_ContextCompat = djni::GetClass(env, "android.support.v4.content.ContextCompat");
    jmethodID jmethodID_ContextCompat_checkSelfPermission = env->GetStaticMethodID(jclass_ContextCompat, "checkSelfPermission", "(Landroid/content/Context;Ljava/lang/String;)I");
    jobject jobject_NativeActivity = dmGraphics::GetNativeAndroidActivity();
    jstring jstring_Permission = env->NewStringUTF(permission);
    jint grantResult =  env->CallStaticIntMethod(jclass_ContextCompat, jmethodID_ContextCompat_checkSelfPermission, jobject_NativeActivity, jstring_Permission);    
    env->DeleteLocalRef(jstring_Permission);
    env->DeleteLocalRef(jclass_ContextCompat);
    return grantResult == AndroidPermissions_PermissionGranted;
}

void AndroidPermissions_RequestPermission(const char** permissions, const size_t& permissionsCount, lua_State* L, const int& callbackRef) {
    JNIEnv* env = djni::env();
    jclass jclass_PermissionRequestsManager = djni::GetClass(env, "com.asfdfdfd.defold.android.permissions.PermissionRequestsManager");
    jmethodID permissionRequestManagerGetInstanceMethod = env->GetStaticMethodID(jclass_PermissionRequestsManager, "getInstance", "()Lcom/asfdfdfd/defold/android/permissions/PermissionRequestsManager;");
    jmethodID jmethodID_PermissionRequestsManager_requestPermissions = env->GetMethodID(jclass_PermissionRequestsManager, "requestPermissions", "(Landroid/app/Activity;[Ljava/lang/String;J)V");      
    jclass jclass_String = djni::GetClass(env, "java.lang.String");    
    jobjectArray jobjectArray_Permissions = env->NewObjectArray(permissionsCount, jclass_String, NULL); 
    env->DeleteLocalRef(jclass_String);
    
    for(size_t permissionIndex = 0; permissionIndex < permissionsCount; permissionIndex++) {
        jstring jstring_Permission = env->NewStringUTF(permissions[permissionIndex]);
        env->SetObjectArrayElement(jobjectArray_Permissions, permissionIndex, jstring_Permission);
        env->DeleteLocalRef(jstring_Permission);            
    }
    
    jobject jobject_NativeActivity = dmGraphics::GetNativeAndroidActivity();

    PermissionRequestContext* permissionRequestContext = new PermissionRequestContext(L, callbackRef);
    jlong permissionRequestContextPtr = (jlong)permissionRequestContext;
    
    jobject jobject_PermissionRequestsManager = env->CallStaticObjectMethod(jclass_PermissionRequestsManager, permissionRequestManagerGetInstanceMethod);
    env->CallVoidMethod(jobject_PermissionRequestsManager, jmethodID_PermissionRequestsManager_requestPermissions, jobject_NativeActivity, jobjectArray_Permissions, permissionRequestContextPtr);
    env->DeleteLocalRef(jobject_PermissionRequestsManager);
    env->DeleteLocalRef(jclass_PermissionRequestsManager);
    env->DeleteLocalRef(jobjectArray_Permissions);
}

// TODO: Probably threading problem.
void AndroidPermissions_onRequestPermissionsResult(JNIEnv* env, jobject, jobjectArray jobjectArray_Permissions, jintArray jintArray_GrantResults, jlong permissionRequestContextPtr) {    
    PermissionRequestContext* permissionRequestContext = (PermissionRequestContext*)permissionRequestContextPtr; 
    lua_State* L = permissionRequestContext->m_LuaState;
    int callbackRef = permissionRequestContext->m_CallbackRef;
    delete permissionRequestContext;

    jsize resultsCount = env->GetArrayLength(jobjectArray_Permissions);
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
    luaL_unref(L, LUA_REGISTRYINDEX, callbackRef);

    jint* grantResults = env->GetIntArrayElements(jintArray_GrantResults, NULL);
    
    lua_createtable(L, 0, resultsCount);    
    for (int permissionIndex = 0; permissionIndex < resultsCount; permissionIndex++) {
        jstring jstring_Permission = (jstring)env->GetObjectArrayElement(jobjectArray_Permissions, permissionIndex);
        const char* permission = env->GetStringUTFChars(jstring_Permission, NULL);

        lua_pushstring(L, permission);
        lua_pushinteger(L, grantResults[permissionIndex]);               
        lua_settable(L, -3);
                
        env->ReleaseStringUTFChars(jstring_Permission, permission);
        env->DeleteLocalRef(jstring_Permission);        
    }

    env->ReleaseIntArrayElements(jintArray_GrantResults, grantResults, JNI_ABORT);
    
    lua_pcall(L, 1, 0, 0);
}

bool AndroidPermissions_ShouldShowRequestPermissionRationale(const char* permission) {
    JNIEnv* env = djni::env();  
    jclass jclass_ActivityCompat = djni::GetClass(env, "android.support.v4.app.ActivityCompat");
    jmethodID jmethodID_ActivityCompat_shouldShowRequestPermissionRationale = env->GetStaticMethodID(jclass_ActivityCompat, "shouldShowRequestPermissionRationale", "(Landroid/app/Activity;Ljava/lang/String;)Z");
    jobject jobject_NativeActivity = dmGraphics::GetNativeAndroidActivity();
    jstring jstring_Permission = env->NewStringUTF(permission);
    bool shouldShowRequestPermissionRationale = env->CallStaticBooleanMethod(jclass_ActivityCompat, jmethodID_ActivityCompat_shouldShowRequestPermissionRationale, jobject_NativeActivity, jstring_Permission);
    env->DeleteLocalRef(jstring_Permission);
    env->DeleteLocalRef(jclass_ActivityCompat);
    return shouldShowRequestPermissionRationale;
}

void AndroidPermissions_RegisterPermissionRequestsManagerNatives(JNIEnv* env) {
    JNINativeMethod permissionRequestsManagerMethods[] = {
        {"onRequestPermissionsResult", "([Ljava/lang/String;[IJ)V", (void*)&AndroidPermissions_onRequestPermissionsResult}
    };

    jclass jclass_PermissionRequestsManager = djni::GetClass(env, "com.asfdfdfd.defold.android.permissions.PermissionRequestsManager");
    env->RegisterNatives(jclass_PermissionRequestsManager, permissionRequestsManagerMethods, sizeof(permissionRequestsManagerMethods) / sizeof(permissionRequestsManagerMethods[0]));    
    env->DeleteLocalRef(jclass_PermissionRequestsManager);
}

void AndroidPermissions_AddPermissionsFragment(JNIEnv* env) {
    jclass jclass_PermissionsFragment = djni::GetClass(env, "com.asfdfdfd.defold.android.permissions.PermissionsFragment");
    jmethodID jmethodID_PermissionsFragment_initialize = env->GetStaticMethodID(jclass_PermissionsFragment, "initialize", "(Landroid/app/Activity;)V");
    jobject jobject_NativeActivity = dmGraphics::GetNativeAndroidActivity();
    env->CallStaticObjectMethod(jclass_PermissionsFragment, jmethodID_PermissionsFragment_initialize, jobject_NativeActivity);        
    env->DeleteLocalRef(jclass_PermissionsFragment);
}

void AndroidPermissions_InitializeConstants(JNIEnv* env) {
    jclass jclass_PackageManager = djni::GetClass(env, "android.content.pm.PackageManager");
    jfieldID jfieldID_PackageManager_PERMISSION_GRANTED = env->GetStaticFieldID(jclass_PackageManager, "PERMISSION_GRANTED", "I");
    jfieldID jfieldID_PackageManager_PERMISSION_DENIED = env->GetStaticFieldID(jclass_PackageManager, "PERMISSION_DENIED", "I");
    AndroidPermissions_PermissionGranted = env->GetStaticIntField(jclass_PackageManager, jfieldID_PackageManager_PERMISSION_GRANTED);
    AndroidPermissions_PermissionDenied = env->GetStaticIntField(jclass_PackageManager, jfieldID_PackageManager_PERMISSION_DENIED);
    env->DeleteLocalRef(jclass_PackageManager);
}

void AndroidPermissions_Initialize() {
    JNIEnv* env = djni::env();

    AndroidPermissions_InitializeConstants(env);
    AndroidPermissions_RegisterPermissionRequestsManagerNatives(env);
    AndroidPermissions_AddPermissionsFragment(env);
}

#endif