
// This module serves as the link between GLFW and the rest of the game.
//  Ideally as much of the OS access should be contained in here, and the game
//  code safely kept unaware inside of the SceneDelegate.  nativeActivity is the
//  layer that handles communication with the GLFW system, and thus OS level access
//  should be quarantined in that file as much as possible.

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Gfx/GLUtil.hpp"

#include "Sys/FilePath.hpp"
#include "Scene/CrowSceneDelegate.hpp"
#include "Sys/Shell.hpp"
#include "Sys/InputManager.hpp"
#include "GLFW/GLFWApp.hpp"
#include "GLFW/GLTestSceneDelegate.hpp"

//#define TEST_OPENGLES2

//#ifdef TEST_OPENGLES2
  #include "GLFW/TestOpenGLES2.hpp"
//#endif

BOOL     GLFWApp::bShellInitialized = FALSE;

//================================================================================
//
// GLFWApp
//
//================================================================================


//-----------------------------------------------------------------------------
GLFWApp::GLFWApp (SceneDelegate *  pSceneIn) : BaseApp (pSceneIn)
  {
  //  Renderer.Clear ();

  // when SceneDelegate throws a signal in response to the signal thrown by
  //  Shell when Shell::OpenURL () is called, handle the request with our
  //  OpenURL call here at the OS level.  This allows an OS-level request
  //  to be called from a deep game-level function without the game level
  //  knowing about the OS level.
  SceneDelegate::sigOnOpenURL.Connect (this, &GLFWApp::OpenURL);
  SceneDelegate::sigOnSetNotification.Connect (this, &GLFWApp::SetNotification);
  SceneDelegate::sigOnClearNotification.Connect (this, &GLFWApp::ClearNotification);
  SceneDelegate::sigOnOpenTwitter.Connect (this, &GLFWApp::OpenTwitter);
  SceneDelegate::sigOnOpenFacebook.Connect (this, &GLFWApp::OpenFacebook);
  }

//-----------------------------------------------------------------------------
GLFWApp::~GLFWApp ()
  {
  SceneDelegate::sigOnOpenURL.Disconnect (this, &GLFWApp::OpenURL);
  SceneDelegate::sigOnSetNotification.Disconnect (this, &GLFWApp::SetNotification);
  SceneDelegate::sigOnClearNotification.Disconnect (this, &GLFWApp::ClearNotification);
  SceneDelegate::sigOnOpenTwitter.Disconnect (this, &GLFWApp::OpenTwitter);
  SceneDelegate::sigOnOpenFacebook.Disconnect (this, &GLFWApp::OpenFacebook);
  }


//-----------------------------------------------------------------------------
void GLFWApp::Init ()
  {
  // NOTE:  The following path needs to be configured to match the build system.
  //         It used to be "./assets/" but now Android Studio stores assets at
  //         "./app/src/main/assets/".  Since I'm only using Linux for testing
  //         at the moment, I can set it to this path.  If I were to release on
  //         Linux or build a utility for running on Linux, this should be
  //         configured more logically.
  InitShell ("./app/src/main/assets/");
  }

//-----------------------------------------------------------------------------
GLFWwindow *  GLFWApp::InitGraphics ()
  {
  // NOTE:  Ordering is important here.  The call to Egl.CreateContext() will
  //   create our window, and our OpenGL rendering context.  Only after
  //   that is done, can we call Shell::InitOpenGL () which will call
  //   glewInit().
  // REFACTOR: I probably need to move all the glew stuff out of Shell
  //   and into EglUtil since I have restructured the code overall such that
  //   Shell's reponsibilities no longer cover graphics initialization, but
  //   that is a larger refactor for another time.


  pWindow = Egl.CreateContext (NULL, Shell::GetCanvasWidth(), Shell::GetCanvasHeight());

  Shell::InitOpenGL ();

  RendererCreate ();
  return (pWindow);
  }

//-----------------------------------------------------------------------------
void GLFWApp::Shutdown ()
  {
  SceneDestroy ();
  UninitShell ();
  }

//-----------------------------------------------------------------------------
void GLFWApp::ShutdownGraphics ()
  {
  RendererDestroy ();
  Egl.DestroyContext ();
  }

//-----------------------------------------------------------------------------
bool GLFWApp::IsActive            (void)
  {
  return (bActive && (pWindow != NULL));
  };

//-----------------------------------------------------------------------------
void GLFWApp::HandleVrModeChanges (void)
  {
  // we destroy our surface on pause, and recreate it on resume.  We likewise
  //  exit vr mode on pause, and re-enter it on resume.

  // NOTE:  For the GLFW app, this isn't currently needed.  Left for reference.

  if (pWindow != NULL && !Egl.HasMainSurface ())
    {
    Egl.CreateSurface ();
    }

  if ( pWindow == NULL && Egl.HasMainSurface () )
    {
    Egl.DestroySurface ();
    }
  }


//-----------------------------------------------------------------------------
void GLFWApp::HandleKeyEvent (const int  keyCode,
                              const int  action)
  {
  // Convert key mappings from GLFW to Shell style, so the shell doesn't have
  //  to know about GLFW.

  //DBG_INFO ("HandleKeyEvent");

  int  iVKey = TranslateVKey (keyCode);
  if (action == GLFW_PRESS)
    {
    Shell::OnKeyDown (iVKey);
    }
  else if (action == GLFW_RELEASE)
    {
    Shell::OnKeyUp (iVKey);
    }
  else if (action == GLFW_REPEAT)
    {
    Shell::OnKeyUp (iVKey);
    Shell::OnKeyDown (iVKey);
    };
  };

//-----------------------------------------------------------------------------
void GLFWApp::HandleTouchEvent (const int    action,
                                const float  x,
                                const float  y,
                                const float  pressure)
  {
  // Handle GearVR touch pad, or the device touchscreen.
  //DBG_INFO ("HandleTouchEvent");

  // Note: the NDK also has gestureDetector.cpp/hpp which may be helpful

  // Convert mouse/touch mappings from GLFW to Shell encoding.
  if (action == GLFW_RELEASE)
    {
    Shell::OnPointerUp (VKEY_TOUCH_0, x, y, pressure);
    }
  else if (action == GLFW_PRESS)
    {
    Shell::OnPointerDown (VKEY_TOUCH_0, x, y, pressure);
    DBG_INFO ("OnPointerDown");
    }
  else
    {
    Shell::OnPointerMove (VKEY_TOUCH_0, x, y, pressure);
    };
  };


//-----------------------------------------------------------------------------
void GLFWApp::HandleMouseKeyEvent (int iButton, int iAction, int iMods)
  {
  // action  One of GLFW_PRESS or GLFW_RELEASE.
  // button The mouse button

  //DBG_INFO ("HandleMouseKeyEvent");

  int  iKeyOut = GAMEPAD_BUTTON_1;
  switch (iButton)
    {
    case GLFW_MOUSE_BUTTON_1:    iKeyOut = GAMEPAD_BUTTON_1; break;
    case GLFW_MOUSE_BUTTON_2:    iKeyOut = GAMEPAD_BUTTON_2; break;
    case GLFW_MOUSE_BUTTON_3:    iKeyOut = GAMEPAD_BUTTON_3; break;
    case GLFW_MOUSE_BUTTON_4:    iKeyOut = GAMEPAD_BUTTON_4; break;
    case GLFW_MOUSE_BUTTON_5:    iKeyOut = GAMEPAD_BUTTON_5; break;
    case GLFW_MOUSE_BUTTON_6:    iKeyOut = GAMEPAD_BUTTON_6; break;
    case GLFW_MOUSE_BUTTON_7:    iKeyOut = GAMEPAD_BUTTON_7; break;
    case GLFW_MOUSE_BUTTON_8:    iKeyOut = GAMEPAD_BUTTON_8; break;
    default:                break;
    };

  // NOTE: We are simulating the mouse as a Touch0 interface, but probably need
  //  to rethink or better abstract this out.  At the very least mouse buttons
  //  0, 1, and 2 should be transformed into touch 0, 1, and 2.  Also consider
  //  how a gamepad and HMD will map into this.  Perhaps "touch" and all the rest
  //  need to map to a more abstract "pointer" concept.

  float  fPressure = 1.0f;
  if (iAction == GLFW_PRESS)
    {
    Shell::OnKeyDown (iKeyOut);
    if (iKeyOut == GAMEPAD_BUTTON_1)
      {
      Shell::OnPointerDown (VKEY_TOUCH_0, fLastMouseX, fLastMouseY, fPressure);
      }
    }
  else if (iAction == GLFW_RELEASE)
    {
    Shell::OnKeyUp (iKeyOut);
    if (iKeyOut == GAMEPAD_BUTTON_1)
      {
      Shell::OnPointerUp (VKEY_TOUCH_0, fLastMouseX, fLastMouseY, fPressure);
      }
    }
  };


//-----------------------------------------------------------------------------
void GLFWApp::HandleMouseMoveEvent (float  fXIn,
                                    float  fYIn)
  {
  // store mouse position.
  fLastMouseX = fXIn;
  fLastMouseY = fYIn;

  float  fPressure = 1.0f;
  if (Shell::IsKeyDown (GAMEPAD_BUTTON_1))
    {
    Shell::OnPointerMove (VKEY_TOUCH_0, fLastMouseX, fLastMouseY, fPressure);
    };
  };


//-----------------------------------------------------------------------------
void GLFWApp::HandleAxisEvent (const int    index,
                               const float  value)
  {
  pSceneDelegate->HandleAxisEvent (index, value);
  };


//-----------------------------------------------------------------------------
void GLFWApp::HandleSystemEvents (void)
  {
  }

//-----------------------------------------------------------------------------
void GLFWApp::InitShell (const char *     szFilePathIn)
  {
  DebugMessagesFactory::Initialize ();
  filePath_localStoragePath.AppendString (szFilePathIn);

  if (!bShellInitialized)
    {
    // TODO:  Figure out with GLFW what axis the sticks come through as.

    // Set shell constants for use by the input system.
    Shell::SetNumAxis (16);

    Shell::SetAxisIndexLeftStickX (0);
    Shell::SetAxisIndexLeftStickY (1);

    Shell::SetAxisIndexRightStickX (2);
    Shell::SetAxisIndexRightStickY (3);

    Shell::SetAxisIndexLeftTrigger  (4);
    Shell::SetAxisIndexRightTrigger (5);

    bShellInitialized = TRUE;
    };
  };

//-----------------------------------------------------------------------------
void GLFWApp::UninitShell (void)
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
bool GLFWApp::SceneIsCreated (void)
  {
  if (pSceneDelegate != NULL)
    {
    return (pSceneDelegate->IsCreated());
    }
  return (false);
  };

//-----------------------------------------------------------------------------
void GLFWApp::SceneCreate (void)
  {
  if (pSceneDelegate != NULL)
    {
    pSceneDelegate->Create ();
    };
  };

//-----------------------------------------------------------------------------
void GLFWApp::SceneDestroy (void)
  {
  if (pSceneDelegate != NULL)
    {
    pSceneDelegate->Destroy ();
    };
  };

//-----------------------------------------------------------------------------
void GLFWApp::OnPause (void)
  {
  if (pSceneDelegate != NULL)
    {
    pSceneDelegate->OnPause ();
    };
  };

//-----------------------------------------------------------------------------
void GLFWApp::OnResume (void)
  {
  if (pSceneDelegate != NULL)
    {
    pSceneDelegate->OnResume ();
    };
  };

//-----------------------------------------------------------------------------
void GLFWApp::TestOpenGL (void)
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

    TestOpenGLES2::Draw(FALSE);
    Egl.SwapBuffers();
  #endif
  };

//-----------------------------------------------------------------------------
void GLFWApp::Update  (void)
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
void GLFWApp::RendererRenderFrame (void)
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
void GLFWApp::ClearBorderPixels (void)
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
void GLFWApp::RendererCreate (void)
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
void GLFWApp::RendererDestroy (void)
  {
  //Renderer.Destroy ();
  };

//-----------------------------------------------------------------------------
VOID GLFWApp::PollForJoysticks (VOID)
  {
  if (glfwJoystickPresent (GLFW_JOYSTICK_1))
    {
    int iAxisCount;
    const float * afAxes = glfwGetJoystickAxes (GLFW_JOYSTICK_1, &iAxisCount);

    // NOTE:  Probably need to do some testing and mapping of which axis index goes where.

    if (iAxisCount > 0) HandleAxisEvent (0, afAxes[0]);
    if (iAxisCount > 1) HandleAxisEvent (1, afAxes[1]);
    if (iAxisCount > 2) HandleAxisEvent (2, afAxes[2]);
    if (iAxisCount > 3) HandleAxisEvent (3, afAxes[3]);
    if (iAxisCount > 4) HandleAxisEvent (4, afAxes[4]);
    if (iAxisCount > 5) HandleAxisEvent (5, afAxes[5]);
    if (iAxisCount > 6) HandleAxisEvent (6, afAxes[6]);
    if (iAxisCount > 7) HandleAxisEvent (7, afAxes[7]);

    int iButtonCount;
    const unsigned char* auButtons = glfwGetJoystickButtons (GLFW_JOYSTICK_1, &iButtonCount);
    // Each element in the returned array is either GLFW_PRESS or GLFW_RELEASE.
    iButtonCount = RMin (iButtonCount, 16);

    // Only send a key event if the button state has changed since last we processed it.
    for (int  iIndex = 0; iIndex < iButtonCount; ++iIndex)
      {
      if (Shell::IsKeyDown (GAMEPAD_BUTTON_1 + iIndex))
        {
        if (auButtons[iIndex] == GLFW_RELEASE)
          {
          HandleKeyEvent (GAMEPAD_BUTTON_1 + iIndex, GLFW_RELEASE);
          }
        }
      else
        {
        if (auButtons[iIndex] == GLFW_PRESS)
          {
          HandleKeyEvent (GAMEPAD_BUTTON_1 + iIndex, GLFW_PRESS);
          }
        }
      };
    }; // if joystick present
  };

//-----------------------------------------------------------------------------
void GLFWApp::OpenURL (const char *  szURLIn)
  {
  // Not implemented in Linux
  DBG_INFO ("Detected call to OpenURL (%s)", szURLIn);
  };

//-----------------------------------------------------------------------------
void GLFWApp::SetNotification  (int           iCodeIn,
                                int           iSecondsUntilShownIn,
                                const char *  szTitleIn,
                                const char *  szTextIn)
  {
  // Not implemented in Linux
  DBG_INFO ("Detected call to SetNotification (%d - %s : %s)", iCodeIn, szTitleIn, szTextIn);
  };

//-----------------------------------------------------------------------------
void GLFWApp::ClearNotification  (int           iCodeIn)
  {
  // Not implemented in Linux
  DBG_INFO ("Detected call to ClearNotification (%d)", iCodeIn);
  };

//-----------------------------------------------------------------------------
void GLFWApp::OpenTwitter  (const char *  szUserNameIn)
  {
  // Not implemented in Linux
  DBG_INFO ("Detected call to OpenTwitter (%s)", szUserNameIn);
  };

//-----------------------------------------------------------------------------
void GLFWApp::OpenFacebook  (const char *  szUserIdIn,
                             const char *  szUserNameIn)
  {
  // Not implemented in Linux
  DBG_INFO ("Detected call to OpenFacebook (%s : %s)", szUserIdIn, szUserNameIn);
  };

//-------------------------------------------------------------------------
INT  GLFWApp::TranslateVKey (INT  iKeyCodeIn)
  {
  if ((iKeyCodeIn >= GLFW_KEY_A) && (iKeyCodeIn <= GLFW_KEY_Z))
    {
    return ('A' + iKeyCodeIn - GLFW_KEY_A);
    }
  else if ((iKeyCodeIn >= GLFW_KEY_0) && (iKeyCodeIn <= GLFW_KEY_A))
    {
    return ('0' + iKeyCodeIn - GLFW_KEY_0);
    }
  else
    {
    switch (iKeyCodeIn)
      {
      case GLFW_KEY_LEFT_ALT:         return (VKEY_ALT);
      case GLFW_KEY_RIGHT_ALT:        return (VKEY_ALT);

      case GLFW_KEY_APOSTROPHE:       return ('\'');
      case '@':                       return ('@');
      //case GLFW_KEY_BACK:             return (VKEY_ESC); // hardware back button
      case GLFW_KEY_BACKSLASH:        return ('\\');
      case GLFW_KEY_PAUSE:            return (VKEY_PAUSE);


      /*
      case GLFW_KEY_BUTTON_1:         return (GAMEPAD_BUTTON_1);
      case GLFW_KEY_BUTTON_2:         return (GAMEPAD_BUTTON_2);
      case GLFW_KEY_BUTTON_3:         return (GAMEPAD_BUTTON_3);
      case GLFW_KEY_BUTTON_4:         return (GAMEPAD_BUTTON_4);
      case GLFW_KEY_BUTTON_5:         return (GAMEPAD_BUTTON_5);
      case GLFW_KEY_BUTTON_6:         return (GAMEPAD_BUTTON_6);
      case GLFW_KEY_BUTTON_7:         return (GAMEPAD_BUTTON_7);
      case GLFW_KEY_BUTTON_8:         return (GAMEPAD_BUTTON_8);
      case GLFW_KEY_BUTTON_9:         return (GAMEPAD_BUTTON_9);
      case GLFW_KEY_BUTTON_10:        return (GAMEPAD_BUTTON_10);
      case GLFW_KEY_BUTTON_11:        return (GAMEPAD_BUTTON_11);
      case GLFW_KEY_BUTTON_12:        return (GAMEPAD_BUTTON_12);
      case GLFW_KEY_BUTTON_13:        return (GAMEPAD_BUTTON_13);
      case GLFW_KEY_BUTTON_14:        return (GAMEPAD_BUTTON_14);
      case GLFW_KEY_BUTTON_15:        return (GAMEPAD_BUTTON_15);
      case GLFW_KEY_BUTTON_16:        return (GAMEPAD_BUTTON_16);

      case GLFW_KEY_BUTTON_A:         return (BUTTON_A);
      case GLFW_KEY_BUTTON_B:         return (BUTTON_B);
      case GLFW_KEY_BUTTON_C:         return (BUTTON_C);
      case GLFW_KEY_BUTTON_X:         return (BUTTON_X);
      case GLFW_KEY_BUTTON_Y:         return (BUTTON_Y);
      case GLFW_KEY_BUTTON_Z:         return (BUTTON_Z);
      case GLFW_KEY_BUTTON_L1:        return (BUTTON_L1);
      case GLFW_KEY_BUTTON_L2:        return (BUTTON_L2);
      case GLFW_KEY_BUTTON_R1:        return (BUTTON_R1);
      case GLFW_KEY_BUTTON_R2:        return (BUTTON_R2);
      case GLFW_KEY_BUTTON_THUMBL:    return (BUTTON_THUMBL);
      case GLFW_KEY_BUTTON_THUMBR:    return (BUTTON_THUMBR);
      case GLFW_KEY_BUTTON_MODE:      return (BUTTON_MODE);
      case GLFW_KEY_BUTTON_SELECT:    return (BUTTON_SELECT);
      case GLFW_KEY_BUTTON_START:     return (BUTTON_START);

      case GLFW_KEY_DPAD_CENTER:      return (DPAD_CENTER);
      case GLFW_KEY_DPAD_DOWN:        return (DPAD_DOWN);
      case GLFW_KEY_DPAD_LEFT:        return (DPAD_LEFT);
      case GLFW_KEY_DPAD_RIGHT:       return (DPAD_RIGHT);
      case GLFW_KEY_DPAD_UP:          return (DPAD_UP);
      */

      case GLFW_KEY_COMMA:            return (',');
      case GLFW_KEY_EQUAL:            return ('=');
      case GLFW_KEY_GRAVE_ACCENT:     return ('`');
      case GLFW_KEY_LEFT_BRACKET:     return ('[');
      case GLFW_KEY_RIGHT_BRACKET:    return (']');
      case GLFW_KEY_MINUS:            return ('-');
      case '+':                       return ('+');
      case GLFW_KEY_PERIOD:           return ('.');
      case '#':                       return ('#');
      case GLFW_KEY_SEMICOLON:        return (';');
      case GLFW_KEY_SLASH:            return ('/');
      case GLFW_KEY_SPACE:            return (VKEY_SPACE);
      case '*':                       return ('*');
      case GLFW_KEY_TAB:              return (VKEY_TAB);
      case GLFW_KEY_F1:               return (VKEY_F1);
      case GLFW_KEY_F2:               return (VKEY_F2);
      case GLFW_KEY_F3:               return (VKEY_F3);
      case GLFW_KEY_F4:               return (VKEY_F4);
      case GLFW_KEY_F5:               return (VKEY_F5);
      case GLFW_KEY_F6:               return (VKEY_F6);
      case GLFW_KEY_F7:               return (VKEY_F7);
      case GLFW_KEY_F8:               return (VKEY_F8);
      case GLFW_KEY_F9:               return (VKEY_F9);
      case GLFW_KEY_F10:              return (VKEY_F10);
      case GLFW_KEY_F11:              return (VKEY_F11);
      case GLFW_KEY_F12:              return (VKEY_F12);
      case GLFW_KEY_PAGE_UP:          return (VKEY_PAGEUP);
      case GLFW_KEY_PAGE_DOWN:        return (VKEY_PAGEDOWN);
      case GLFW_KEY_LEFT_SHIFT:       return (VKEY_SHIFT);
      case GLFW_KEY_RIGHT_SHIFT:      return (VKEY_SHIFT);
      case GLFW_KEY_DELETE:           return (VKEY_BACKSPACE);
      case GLFW_KEY_LEFT_CONTROL:     return (VKEY_CONTROL);
      case GLFW_KEY_RIGHT_CONTROL:    return (VKEY_CONTROL);
      case GLFW_KEY_ENTER:            return (VKEY_ENTER);
      case GLFW_KEY_ESCAPE:           return (VKEY_ESC);
      case GLFW_KEY_HOME:             return (VKEY_HOME);
      case GLFW_KEY_END:              return (VKEY_END);
      case GLFW_KEY_INSERT:           return (VKEY_INSERT);
      case GLFW_KEY_MENU:             return (VKEY_MENU);

      case GLFW_KEY_KP_0:             return (VKEY_NUMPAD0);
      case GLFW_KEY_KP_1:             return (VKEY_NUMPAD1);
      case GLFW_KEY_KP_2:             return (VKEY_NUMPAD2);
      case GLFW_KEY_KP_3:             return (VKEY_NUMPAD3);
      case GLFW_KEY_KP_4:             return (VKEY_NUMPAD4);
      case GLFW_KEY_KP_5:             return (VKEY_NUMPAD5);
      case GLFW_KEY_KP_6:             return (VKEY_NUMPAD6);
      case GLFW_KEY_KP_7:             return (VKEY_NUMPAD7);
      case GLFW_KEY_KP_8:             return (VKEY_NUMPAD8);
      case GLFW_KEY_KP_9:             return (VKEY_NUMPAD9);
      case GLFW_KEY_KP_ADD:           return (VKEY_NUMPAD_ADD);
      case GLFW_KEY_KP_DIVIDE:        return (VKEY_NUMPAD_DIVIDE);
      case GLFW_KEY_KP_DECIMAL:       return (VKEY_NUMPAD_DECIMAL);
      case GLFW_KEY_KP_SUBTRACT:      return (VKEY_NUMPAD_SUBTRACT);
      case GLFW_KEY_KP_MULTIPLY:      return (VKEY_NUMPAD_MULTIPLY);
      case GLFW_KEY_KP_EQUAL:         return (VKEY_NUMPAD_EQUAL);
      case GLFW_KEY_KP_ENTER:         return (VKEY_NUMPAD_ENTER);

      //case KeyEvent.:  return (VKEY_);

      default:
        return (VKEY_INVALID);
      }
    }
  }
