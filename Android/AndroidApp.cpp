
// This module serves as the link between Android and the rest of the game.
//  Ideally as much of the OS access should be contained in here, and the game
//  code safely kept unaware inside of the SceneDelegate.  nativeActivity is the
//  layer that handles communication with the Android system, and thus OS level access
//  should be quarantined in that file as much as possible.

//#define IRONSOURCE
//#define ADMOB

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Gfx/GLUtil.hpp"

#include "Sys/FilePath.hpp"
#include "Scene/CrowSceneDelegate.hpp"
#include "Sys/Shell.hpp"
#include "Android/AndroidApp.hpp"
#include "Android/GLTestSceneDelegate.hpp"
#include "Net/Analytics.hpp"
#include "Android/AnalyticsFirebase.hpp"
#include "Net/AnalyticsLog.hpp"
#include "AdSystem/AdSystem.hpp"
#ifdef IRONSOURCE
  #include "IronSource/AdSysIronSource.hpp"
#endif
#ifdef ADMOB
  #include "AdMob/AdSysAdMob.hpp"
#endif
#include "GoogleIAB/ShopGoogleIAB.hpp"

#include "Render/Texture.hpp"
#include "Render/Shader.hpp"
#include "Render/DisplayMesh.hpp"


//#define TEST_OPENGLES2

#ifdef TEST_OPENGLES2
  #include "Android/TestOpenGLES2.hpp"
#endif

//#include "Android/.hpp"

BOOL     AndroidApp::bJniInitialized = FALSE;
BOOL     AndroidApp::bShellInitialized = FALSE;
//BOOL     AndroidApp::bRefreshNavBar = FALSE;

JNIEnv*  AndroidApp::jniEnv;
jclass   AndroidApp::jClassWebHelper;
jclass   AndroidApp::jClassActivity;
jclass   AndroidApp::jClassWindow;
jclass   AndroidApp::jClassView;


//================================================================================
//
// AndroidApp
//
//================================================================================


//-----------------------------------------------------------------------------
AndroidApp::AndroidApp (SceneDelegate *  pSceneIn) : BaseApp (pSceneIn)
  {
  //  Renderer.Clear ();

  // when SceneDelegate throws a signal in response to the signal thrown by
  //  Shell when Shell::OpenURL () is called, handle the request with our
  //  OpenURL call here at the OS level.  This allows an OS-level request
  //  to be called from a deep game-level function without the game level
  //  knowing about the OS level.
  SceneDelegate::sigOnOpenURL.Connect (this, &AndroidApp::OpenURL);
  SceneDelegate::sigOnSetNotification.Connect (this, &AndroidApp::SetNotification);
  SceneDelegate::sigOnClearNotification.Connect (this, &AndroidApp::ClearNotification);
  SceneDelegate::sigOnOpenTwitter.Connect (this, &AndroidApp::OpenTwitter);
  SceneDelegate::sigOnOpenFacebook.Connect (this, &AndroidApp::OpenFacebook);
  }

//-----------------------------------------------------------------------------
AndroidApp::~AndroidApp ()
  {
  SceneDelegate::sigOnOpenURL.Disconnect (this, &AndroidApp::OpenURL);
  SceneDelegate::sigOnSetNotification.Disconnect (this, &AndroidApp::SetNotification);
  SceneDelegate::sigOnClearNotification.Disconnect (this, &AndroidApp::ClearNotification);
  SceneDelegate::sigOnOpenTwitter.Disconnect (this, &AndroidApp::OpenTwitter);
  SceneDelegate::sigOnOpenFacebook.Disconnect (this, &AndroidApp::OpenFacebook);
  }


//-----------------------------------------------------------------------------
void AndroidApp::Init (TJava                 java,
                       struct android_app *  app)
  {
  Java = java;

  InitShell (app->activity->assetManager, app->activity->internalDataPath);
  AndroidHelper::Init (java, app);

  #ifdef IRONSOURCE
    AdSysIronSource::StoreClasses (java.Env);
  #endif
  #ifdef ADMOB
    AdSysAdMob::StoreClasses (java.Env);
  #endif
  ShopGoogleIAB::StoreClasses (java.Env);
  AndroidApp::StoreClasses (java.Env);

  AnalyticsFirebase *  pAdapter = new AnalyticsFirebase;
  pAdapter->Init (java.Env, java.ActivityObject);
  Analytics::RegisterAdapter (pAdapter);

  #ifdef IRONSOURCE
    // TODO: Need to time how long it takes for IronSource to init.  We may need
    //  to initialize this later in the startup sequence, but before it is first
    //  called.
    //AdSystem::Instance()->RegisterAdapter (AdSysIronSource::Instance ());
  #endif
  #ifdef ADMOB
    AdSystem::Instance()->RegisterAdapter (AdSysAdMob::Instance ());
  #endif
  }

//-----------------------------------------------------------------------------
void AndroidApp::StoreClasses (JNIEnv*      jniEnvIn)
  {
  // store the classes for our local lookups
  AndroidApp::jniEnv = jniEnvIn;
  AndroidApp::jClassWebHelper = AndroidHelper::RetrieveClass (jniEnv, "com/muckyrobot/crow/WebHelper");


  AndroidApp::jClassActivity = AndroidHelper::RetrieveClass (jniEnv, "android/app/NativeActivity");
  AndroidApp::jClassWindow   = AndroidHelper::RetrieveClass (jniEnv, "android/view/Window");
  AndroidApp::jClassView     = AndroidHelper::RetrieveClass (jniEnv, "android/view/View");


  }

//-----------------------------------------------------------------------------
void AndroidApp::InitGraphics (struct android_app *  app)
  {
  DBG_INFO ("-------======= AndroidApp::InitGraphics () =======-------");
  if (app->window != NULL)
    {
    Egl.CreateContext (NULL, app);  // will only perform on the first call.
    Egl.CreateSurface (NativeWindow);
    RendererCreate ();

    // Reset any graphical elements left from previous contexts
    Shader::OnContextLost (GLCallProxyLive::Instance());
    Texture::OnContextLost ();
    DisplayMesh::OnContextLost ();
    };
  }

//-----------------------------------------------------------------------------
void AndroidApp::Shutdown ()
  {
  SceneDestroy ();
  UninitShell ();
  }

//-----------------------------------------------------------------------------
void AndroidApp::ShutdownGraphics ()
  {
  RendererDestroy ();
  Egl.DestroyContext ();
  }

//-----------------------------------------------------------------------------
bool AndroidApp::IsActive            (void)
  {
  return (bActive && (NativeWindow != NULL));
  };

//-----------------------------------------------------------------------------
void AndroidApp::HandleVrModeChanges (struct android_app *  app)
  {
  // we destroy our surface on pause, and recreate it on resume.  We likewise
  //  exit vr mode on pause, and re-enter it on resume.

  if (NativeWindow != NULL && !Egl.HasMainSurface ())
    {
    Egl.CreateSurface (NativeWindow);
    }

  if ( NativeWindow == NULL && Egl.HasMainSurface () )
    {
    Egl.DestroySurface ();
    }

//  if (bRefreshNavBar)
//    {
//    bRefreshNavBar = FALSE;
//    AutoHideNavBar (app);
//    }
  }


//-----------------------------------------------------------------------------
int AndroidApp::HandleKeyEvent (const int  keyCode,
                                const int  action)
  {
  // Convert key mappings from Android to Shell style, so the shell doesn't have
  //  to know about Android.
  int  iVKey = AndroidHelper::TranslateVKey (keyCode);
  if (action == AKEY_EVENT_ACTION_DOWN)
    {
    Shell::OnKeyDown (iVKey);
    }
  else if (action == AKEY_EVENT_ACTION_UP)
    {
    Shell::OnKeyUp (iVKey);
    };

  return 0;
  };

//-----------------------------------------------------------------------------
int AndroidApp::HandleTouchEvent (const int    action,
                                  const float  x,
                                  const float  y,
                                  const float  pressure)
  {
  // Handle GearVR touch pad, or the device touchscreen.

  // Note: the NDK also has gestureDetector.cpp/hpp which may be helpful

  // Convert mouse/touch mappings from Android to Shell encoding.
  if (action == AMOTION_EVENT_ACTION_UP)
    {
    Shell::OnPointerUp (VKEY_TOUCH_0, x, y, pressure);
    }
  else if (action == AMOTION_EVENT_ACTION_DOWN)
    {
    Shell::OnPointerDown (VKEY_TOUCH_0, x, y, pressure);
    }
  else if (action == AMOTION_EVENT_ACTION_MOVE)
    {
    Shell::OnPointerMove (VKEY_TOUCH_0, x, y, pressure);
    };

  return 1;
  };

//-----------------------------------------------------------------------------
void AndroidApp::HandleAxisEvent (const int    index,
                                  const float  value)
  {
  pSceneDelegate->HandleAxisEvent (index, value);
  };


//-----------------------------------------------------------------------------
void AndroidApp::HandleSystemEvents (void)
  {
  }

//-----------------------------------------------------------------------------
void AndroidApp::InitShell (AAssetManager *  assetManagerIn,
                            const char *     szFilePathIn)
  {
  filePath_assetManager = assetManagerIn;
  filePath_localStoragePath.AppendString (szFilePathIn);

  if (!bShellInitialized)
    {
    // Set shell constants for use by the input system.
    Shell::SetNumAxis (AMOTION_EVENT_AXIS_GENERIC_16);

    Shell::SetAxisIndexLeftStickX (AMOTION_EVENT_AXIS_X);
    Shell::SetAxisIndexLeftStickY (AMOTION_EVENT_AXIS_Y);

    Shell::SetAxisIndexRightStickX (AMOTION_EVENT_AXIS_Z);
    Shell::SetAxisIndexRightStickY (AMOTION_EVENT_AXIS_RZ);

    Shell::SetAxisIndexLeftTrigger (AMOTION_EVENT_AXIS_BRAKE);
    Shell::SetAxisIndexRightTrigger (AMOTION_EVENT_AXIS_GAS);

    bShellInitialized = TRUE;
    };
  };

//-----------------------------------------------------------------------------
void AndroidApp::UninitShell (void)
  {
  /*
  // NOTE:  We shouldn't ever destroy the scene delegate because it is created
  // and owned by android_main or the other _main.cpp files.
  if (pSceneDelegate != NULL)
    {
    delete pSceneDelegate;
    pSceneDelegate = NULL;
    };
  */
  };

//-----------------------------------------------------------------------------
bool AndroidApp::SceneIsCreated (void)
  {
  if (pSceneDelegate != NULL)
    {
    return (pSceneDelegate->IsCreated());
    }
  return (false);
  };

//-----------------------------------------------------------------------------
void AndroidApp::SceneCreate (void)
  {
  if (pSceneDelegate != NULL)
    {
    pSceneDelegate->Create ();
    };
  };

//-----------------------------------------------------------------------------
void AndroidApp::SceneDestroy (void)
  {
  if (pSceneDelegate != NULL)
    {
    pSceneDelegate->Destroy ();
    };
  };

//-----------------------------------------------------------------------------
void AndroidApp::OnPause (void)
  {
  if (pSceneDelegate != NULL)
    {
    pSceneDelegate->OnPause ();
    };
  };

//-----------------------------------------------------------------------------
void AndroidApp::OnResume (void)
  {
  if (pSceneDelegate != NULL)
    {
    pSceneDelegate->OnResume ();
    };
  };

//-----------------------------------------------------------------------------
void AndroidApp::TestOpenGL (void)
  {
  #ifdef TEST_OPENGLES2
    // DEBUGGING:  Test OpenGL ES2
    static bool test_init = false;
    if (!test_init)
      {
      test_init = true;
      TestOpenGLES2::Init();
      };

    GL( glDepthMask( GL_TRUE ) );
    GL( glEnable( GL_DEPTH_TEST ) );
    GL( glDepthFunc( GL_LEQUAL ) );
    GL( glViewport( 0, 0, Egl.GetWidth(), Egl.GetHeight()) );

    TestOpenGLES2::Draw(TRUE);
    Egl.SwapBuffers();
  #endif
  };

//-----------------------------------------------------------------------------
void AndroidApp::Update  (void)
  {
  // NOTE:  This function is called as often as possible from the main loop.

  if (pSceneDelegate == NULL) return;

  #ifdef TEST_OPENGLES2
    TestOpenGL ();
  #else

    if (!IsActive ())
      {
      // This condition prevents the scene from being created if we are not in VR mode / Active
      return;
      }

    // Create the scene if not yet created.
    // The scene is created here to be able to show a loading icon.
    if  (!SceneIsCreated () )
      {
      // Create the scene.
      SceneCreate ();
      }

    // This is the only place the frame index is incremented, right before
    // calling vrapi_GetPredictedDisplayTime().
    FrameIndex++;

    // If we need to drive the display with sensors, do it here.
    // NOTE:  We could predict displayed time based on the FrameIndex, but currently it isn't used.
    const double predictedDisplayTime = 0;

    // Advance the simulation based on the predicted display time.
    // NOTE: Could change this to a delta time, and track overall time within ovrScene.  Currently not used in CrowSceneDelegate

    //DBG_INFO ("Advance frames %d", Shell::FramesToRender (FALSE));
    // The Advance () call will increment the Shell time state, so we can query it.
    pSceneDelegate->Advance (predictedDisplayTime);

    if (Shell::FramesToRender (TRUE) > 0)
      {
      Shell::IncFrameCount ();

      RendererRenderFrame ();
      };
  #endif
  };

//-----------------------------------------------------------------------------
void AndroidApp::RendererRenderFrame (void)
  {
  // TODO: Need to figure out the Non-VR version of this.


  if (pSceneDelegate == NULL) return;

  // Calculate the center view matrix for any HMD.
  TMatrix32f  omatViewerLocation = TMatrix32f_Identity ();
  pSceneDelegate->GetViewXForm (omatViewerLocation);

  TMatrix32f centerEyeViewMatrix = omatViewerLocation;

  pSceneDelegate->SetViewXForm (centerEyeViewMatrix);

  // setup overall rendering state.

  GL( glEnable( GL_SCISSOR_TEST ) );
  GL( glDepthMask( GL_TRUE ) );
  GL( glEnable( GL_DEPTH_TEST ) );
  GL( glDepthFunc( GL_LEQUAL ) );
  GL( glViewport( 0, 0, Egl.GetWidth(), Egl.GetHeight()) );
  GL( glScissor( 0, 0, Egl.GetWidth(), Egl.GetHeight()) );

  //DBG_INFO ("glViewport of %d x %d", Egl.GetWidth(), Egl.GetHeight());

  Shell::SetCanasSize (Egl.GetWidth (), Egl.GetHeight ());

  pSceneDelegate->RenderStart ();

  // start per-eye rendering, as needed

  // TODO: Pass center matrix to shell
  // Modify view direction by interpupilary distance
  const TMatrix32f eyeViewMatrix = centerEyeViewMatrix;

  // set current render buffer here, as needed.

  // render the scene to the current eye
  pSceneDelegate->RenderEye (eyeViewMatrix, ProjectionMatrix);

  ClearBorderPixels ();

  //  frameBuffer->Resolve ();
  glFlush(); // NOTE: SwapBuffers is supposed to do an implicit flush

  // Note:  Call frame rendering here, once app is separated out.
  Egl.SwapBuffers();
  };

//-----------------------------------------------------------------------------
void AndroidApp::ClearBorderPixels (void)
  {
  // NOTE:  The below probably isn't needed for non-VR.
  // Explicitly clear the border texels to black because OpenGL-ES does not support GL_CLAMP_TO_BORDER.
  // Clear to fully opaque black.
  GL( glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
  // bottom
  GL( glScissor( 0, 0, Egl.GetWidth (), 1 ) );
  GL( glClear( GL_COLOR_BUFFER_BIT ) );
  // top
  GL( glScissor( 0, Egl.GetHeight () - 1, Egl.GetWidth (), 1 ) );
  GL( glClear( GL_COLOR_BUFFER_BIT ) );
  // left
  GL( glScissor( 0, 0, 1, Egl.GetHeight ()) );
  GL( glClear( GL_COLOR_BUFFER_BIT ) );
  // right
  GL( glScissor( Egl.GetWidth () - 1, 0, 1, Egl.GetHeight ()) );
  GL( glClear( GL_COLOR_BUFFER_BIT ) );
  };

//-----------------------------------------------------------------------------
void AndroidApp::RendererCreate (void)
  {
  float  fFovX = 80.0f; // This needs to be read from a setting somewhere, or run by the scene camera.
  float  fFovY = fFovX * (float (Egl.GetHeight ()) / float(Egl.GetWidth ()));

  // Setup the projection matrix.
  ProjectionMatrix = TMatrix32f_CreateProjectionFov (fFovX, // fovX
                                                     fFovY, // fovY
                                                     0.0f, 0.0f, 0.10f, 0.0f ); // offsetX, offsetY, nearZ, farZ.  farZ of 0 makes far plane at infinity.
  //Renderer.Create (&Java);
  };

//-----------------------------------------------------------------------------
void AndroidApp::RendererDestroy (void)
  {
  //Renderer.Destroy ();
  };


//-----------------------------------------------------------------------------
void AndroidApp::OpenURL (const char *  szURLIn)
  {
  //DBG_INFO ("Detected call to OpenURL (%s)", szURLIn);

  // Calls the OpenURL Java method in MainActivity.java

  jstring              jstr = Java.Env->NewStringUTF (szURLIn);

  //jclass     jActivityClass = Java.Env->GetObjectClass (Java.ActivityObject);
  //jmethodID openURLFunction = Java.Env->GetMethodID  (jActivityClass, "OpenURL", "(Ljava/lang/String;)V");
  //Java.Env->CallVoidMethod (Java.ActivityObject, openURLFunction, jstr);

  jmethodID fnOpenURL = jniEnv->GetStaticMethodID  (AndroidApp::jClassWebHelper, "OpenURL", "(Ljava/lang/String;)V");
  AndroidApp::jniEnv->CallStaticVoidMethod (AndroidApp::jClassWebHelper, fnOpenURL, jstr);


  /*
  // code to call an activity
  jclass     activityClass = Java.Env->FindClass ("android/app/NativeActivity");
  jmethodID getClassLoader = Java.Env->GetMethodID (activityClass,"getClassLoader", "()Ljava/lang/ClassLoader;");
  jobject cls = Java.Env->CallObjectMethod (Java.ActivityObject, getClassLoader);
  jclass classLoader = Java.Env->FindClass ("java/lang/ClassLoader");
  jmethodID findClass = Java.Env->GetMethodID (classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
  jmethodID contextMethod = Java.Env->GetMethodID (activityClass, "getApplicationContext", "()Landroid/content/Context;");
  jobject contextObj = Java.Env->CallObjectMethod (Java.ActivityObject, contextMethod);
  */
  };


//-----------------------------------------------------------------------------
void AndroidApp::SetNotification (int           iCodeIn,
                                  int           iSecondsUntilShownIn,
                                  const char *  szTitleIn,
                                  const char *  szTextIn)
  {
  // Calls the SetNotification Java method in WebHelper.java

  jint               jiCode = iCodeIn;
  jint  jiSecondsUntilShown = iSecondsUntilShownIn;
  jstring         jstrTitle = Java.Env->NewStringUTF (szTitleIn);
  jstring          jstrText = Java.Env->NewStringUTF (szTextIn);

  //jclass     jActivityClass = Java.Env->GetObjectClass (Java.ActivityObject);
  //jmethodID      idFunction = Java.Env->GetMethodID  (jActivityClass, "SetNotification", "(IILjava/lang/String;Ljava/lang/String;)V");
  //Java.Env->CallVoidMethod (Java.ActivityObject,
  //                          idFunction,
  //                          jiCode,
  //                          jiSecondsUntilShown,
  //                          jstrTitle,
  //                          jstrText);

  jmethodID fnSetNotification = jniEnv->GetStaticMethodID  (AndroidApp::jClassWebHelper, "SetNotification", "(IILjava/lang/String;Ljava/lang/String;)V");
  AndroidApp::jniEnv->CallStaticVoidMethod (AndroidApp::jClassWebHelper,
                                            fnSetNotification,
                                            jiCode,
                                            jiSecondsUntilShown,
                                            jstrTitle,
                                            jstrText);


  /*
  // code to call an activity
  jclass     activityClass = Java.Env->FindClass ("android/app/NativeActivity");
  jmethodID getClassLoader = Java.Env->GetMethodID (activityClass,"getClassLoader", "()Ljava/lang/ClassLoader;");
  jobject cls = Java.Env->CallObjectMethod (Java.ActivityObject, getClassLoader);
  jclass classLoader = Java.Env->FindClass ("java/lang/ClassLoader");
  jmethodID findClass = Java.Env->GetMethodID (classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
  jmethodID contextMethod = Java.Env->GetMethodID (activityClass, "getApplicationContext", "()Landroid/content/Context;");
  jobject contextObj = Java.Env->CallObjectMethod (Java.ActivityObject, contextMethod);
  */
  };

//-----------------------------------------------------------------------------
void AndroidApp::ClearNotification (int           iCodeIn)
  {
  // Calls the ClearNotification Java method in WebHelper.java
  jint               jiCode = iCodeIn;

  jmethodID fnClearNotification = jniEnv->GetStaticMethodID  (AndroidApp::jClassWebHelper, "ClearNotification", "(I)V");
  AndroidApp::jniEnv->CallStaticVoidMethod (AndroidApp::jClassWebHelper,
                                            fnClearNotification,
                                            jiCode);
  };


//-----------------------------------------------------------------------------
void AndroidApp::OpenTwitter (const char *  szUserNameIn)
  {
  // Calls the OpenTwitter Java method in MainActivity.java

  jstring  jstrUserName = Java.Env->NewStringUTF (szUserNameIn);

  //jclass     jActivityClass = Java.Env->GetObjectClass (Java.ActivityObject);
  //jmethodID      idFunction = Java.Env->GetMethodID  (jActivityClass, "OpenTwitter", "(Ljava/lang/String;)V");
  //Java.Env->CallVoidMethod (Java.ActivityObject,
  //                          idFunction,
  //                          jstrUserName);

  jmethodID fnOpenTwitter = jniEnv->GetStaticMethodID  (AndroidApp::jClassWebHelper, "OpenTwitter", "(Ljava/lang/String;)V");
  AndroidApp::jniEnv->CallStaticVoidMethod (AndroidApp::jClassWebHelper, fnOpenTwitter, jstrUserName);

  };

//-----------------------------------------------------------------------------
void AndroidApp::OpenFacebook (const char *  szUserIdIn,
                               const char *  szUserNameIn)
  {
  // Calls the OpenFacebook Java method in MainActivity.java

  jstring    jstrUserId = Java.Env->NewStringUTF (szUserIdIn);
  jstring  jstrUserName = Java.Env->NewStringUTF (szUserNameIn);

  //jclass     jActivityClass = Java.Env->GetObjectClass (Java.ActivityObject);
  //jmethodID      idFunction = Java.Env->GetMethodID  (jActivityClass, "OpenFacebook", "(Ljava/lang/String;Ljava/lang/String;)V");
  //Java.Env->CallVoidMethod (Java.ActivityObject,
  //                          idFunction,
  //                          jstrUserId,
  //                          jstrUserName);

  jmethodID fnOpenFacebook = jniEnv->GetStaticMethodID  (AndroidApp::jClassWebHelper, "OpenFacebook", "(Ljava/lang/String;Ljava/lang/String;)V");
  AndroidApp::jniEnv->CallStaticVoidMethod (AndroidApp::jClassWebHelper, fnOpenFacebook, jstrUserId, jstrUserName);
  };




//-----------------------------------------------------------------------------
void AndroidApp::AutoHideNavBar (struct android_app* app)
  {
  // This code is from :
  //  https://www.gamedev.net/forums/topic/674511-auto-hide-nav-bar-on-android-app-ndk/

  // http://www.androiddocs.com/training/system-ui/immersive.html

  // NOTE: This has to be called from the UI thread.

  DBG_INFO ("Calling AutoHideNavBar")

  jmethodID getWindow             = jniEnv->GetMethodID (AndroidApp::jClassActivity, "getWindow", "()Landroid/view/Window;");
  jmethodID getDecorView          = jniEnv->GetMethodID (AndroidApp::jClassWindow, "getDecorView", "()Landroid/view/View;");
  jmethodID setSystemUiVisibility = jniEnv->GetMethodID (AndroidApp::jClassView, "setSystemUiVisibility", "(I)V");

  jobject window    = jniEnv->CallObjectMethod (app->activity->clazz, getWindow);
  jobject decorView = jniEnv->CallObjectMethod (window, getDecorView);

  jfieldID flagFullscreenID      = jniEnv->GetStaticFieldID (AndroidApp::jClassView, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
  jfieldID flagHideNavigationID  = jniEnv->GetStaticFieldID (AndroidApp::jClassView, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
  jfieldID flagImmersiveStickyID = jniEnv->GetStaticFieldID (AndroidApp::jClassView, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");
  jfieldID flagHideLayoutNavID   = jniEnv->GetStaticFieldID (AndroidApp::jClassView, "SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION", "I");
  jfieldID flagLayoutStableID    = jniEnv->GetStaticFieldID (AndroidApp::jClassView, "SYSTEM_UI_FLAG_LAYOUT_STABLE", "I");


  int flagFullscreen      = jniEnv->GetStaticIntField (AndroidApp::jClassView, flagFullscreenID);
  int flagHideNavigation  = jniEnv->GetStaticIntField (AndroidApp::jClassView, flagHideNavigationID);
  int flagImmersiveSticky = jniEnv->GetStaticIntField (AndroidApp::jClassView, flagImmersiveStickyID);
  int flagHideLayoutNav   = jniEnv->GetStaticIntField (AndroidApp::jClassView, flagHideLayoutNavID);
  int flagLayoutStable    = jniEnv->GetStaticIntField (AndroidApp::jClassView, flagLayoutStableID);

  int flag = flagFullscreen | flagHideNavigation | flagImmersiveSticky | flagHideLayoutNav | flagLayoutStable;

  jniEnv->CallVoidMethod (decorView, setSystemUiVisibility, flag);
  }



















/*
typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;


// NOTE:  This is kept for reference because I got it working, but
//   needed to go a different direction for registering classes.
//-----------------------------------------------------------------------------
extern "C" jint JNI_OnLoad (JavaVM* vm, void* ) // void* is reserved
  {
  AndroidApp::bJniInitialized = TRUE;

  UnionJNIEnvToVoid uenv;
  uenv.venv     = NULL;
  JNIEnv *  env = NULL;
  jint result = -1;

  if (vm->GetEnv (&uenv.venv, JNI_VERSION_1_6) != JNI_OK)
    {
    DBG_ERROR("GetEnv failed");
    return result;
    }
  env = uenv.env;

  //AdSysIronSource::StoreClasses (env);


  //  if (registerNatives(env) != JNI_TRUE) {
  //    ALOGE("ERROR: registerNatives failed");
  //  return result;
  //}

  result = JNI_VERSION_1_6;
  return result;
  }
*/
