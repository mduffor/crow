#ifndef ANDROIDAPP_HPP
#define ANDROIDAPP_HPP

#include <stdbool.h>
#include <android_native_app_glue.h>
#include <android/native_activity.h>

#include "Android/EglUtil.hpp"
#include "Android/BaseApp.hpp"
#include "Math/TMatrix.hpp"
#include "Android/AndroidHelper.hpp"


//------------------------------------------------------------------------------
class AndroidApp : public BaseApp
  {
  public:

    EglUtil             Egl;
    TMatrix32f          ProjectionMatrix;
    static BOOL         bJniInitialized;
    static BOOL         bShellInitialized;
//    static BOOL         bRefreshNavBar;

    static JNIEnv*      jniEnv;
    static jclass       jClassWebHelper;

    static jclass       jClassActivity;
    static jclass       jClassWindow;
    static jclass       jClassView;

  public:
         AndroidApp          (SceneDelegate *  pSceneIn);

         ~AndroidApp         ();

    void Init                (TJava                 java,
                              struct android_app *  app);

    void Shutdown            ();

    void static StoreClasses (JNIEnv*      jniEnvIn);

    void InitGraphics        (struct android_app *  app);

    void ShutdownGraphics    ();

    void HandleVrModeChanges (struct android_app *  app);

    void InitShell           (AAssetManager *  assetManager,
                              const char *     szFilePath);

    void UninitShell         (void);

    bool IsActive            (void);

    void static AutoHideNavBar      (struct android_app *  app);


    int  HandleKeyEvent      (const int  keyCode,
                              const int  action);

    int  HandleTouchEvent    (const int    action,
                              const float  x,
                              const float  y,
                              const float  pressure);

    void HandleAxisEvent     (const int    index,
                              const float  value);


    void HandleSystemEvents  (void);

    void TestOpenGL          (void);

    void Update              (void);

    void OnPause             (void);

    void OnResume            (void);

    void OpenURL             (const char *  szURLIn);

    void SetNotification     (int           iCodeIn,
                              int           iSecondsUntilShownIn,
                              const char *  szTitleIn,
                              const char *  szTextIn);

    void ClearNotification   (int           iCodeIn);

    void OpenTwitter         (const char *  szUserNameIn);

    void OpenFacebook        (const char *  szUserIdIn,
                              const char *  szUserNameIn);


  private:

    bool SceneIsCreated      (void);

    void SceneCreate         (void);

    void SceneDestroy        (void);

    void RendererRenderFrame (void);

    void RendererCreate      (void);

    void RendererDestroy     (void);

    void ClearBorderPixels   (void);

  };






#endif // ANDROIDAPP_HPP