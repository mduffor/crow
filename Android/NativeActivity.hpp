#ifndef NATIVEACTIVITY_H
#define NATIVEACTIVITY_H


#if defined( ANDROID )

#include <android/native_window_jni.h>  // for native window JNI
#include <android_native_app_glue.h>

#include "Android/BaseApp.hpp"
#include "Scene/SceneDelegate.hpp"

//#define MULTI_THREADED       0

//------------------------------------------------------------------------------
class NativeActivity
  {
  public:
    NativeActivity  (struct android_app *  pApp,
                     BaseApp *             pBaseApp);

    ~NativeActivity () {};
  };

#endif // ANDROID
#endif // NATIVEACTIVITY_H