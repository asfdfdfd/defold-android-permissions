#if defined(DM_PLATFORM_ANDROID)

#include <dmsdk/sdk.h>

namespace djni {
	JNIEnv* env();

	jclass GetClass(JNIEnv* env, const char* classname);
}

#endif
