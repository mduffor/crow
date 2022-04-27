#ifndef GLFWAPP_HPP
#define GLFWAPP_HPP

#include "GLFW/EglUtil.hpp"
#include "GLFW/BaseApp.hpp"
#include "Math/TMatrix.hpp"

//------------------------------------------------------------------------------
class GLFWApp : public BaseApp
  {
  public:
    float               fLastMouseX;
    float               fLastMouseY;
    EglUtil             Egl;
    TMatrix32f          ProjectionMatrix;
    static BOOL         bShellInitialized;


  public:
         GLFWApp             (SceneDelegate *  pSceneIn);

         ~GLFWApp            ();

    void Init                ();

    void Shutdown            ();

    GLFWwindow * InitGraphics        ();

    void ShutdownGraphics    ();

    void HandleVrModeChanges (void);

    void InitShell           (const char *     szFilePath);

    void UninitShell         (void);

    bool IsActive            (void);

    void HandleKeyEvent      (const int  keyCode,
                              const int  action);

    void HandleTouchEvent    (const int    action,
                              const float  x,
                              const float  y,
                              const float  pressure);

    void HandleMouseKeyEvent  (int iButton,
                               int iAction,
                               int iMods);

    void HandleMouseMoveEvent (float  fXIn,
                               float  fYIn);

    void HandleAxisEvent     (const int    index,
                              const float  value);

    void HandleSystemEvents  (void);

    void TestOpenGL          (void);

    void Update              (void);

    void OnPause             (void);

    void OnResume            (void);

    void PollForJoysticks    (void);

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

    INT  TranslateVKey       (INT  iKeyCodeIn);


  };

#endif // GLFWAPP_HPP