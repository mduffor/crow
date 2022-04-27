
// NOTE: This file handles all access with the OS.  Control is then passed
//  as needed to the BaseApp module, where Oculus SDK related code will be implemented,
//  before passing control down to the game layer in SceneDelegate.

#include <stdlib.h>
#include <time.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Sys/InputManager.hpp"
#include "Sys/Shell.hpp"

#include "GLFW/NativeActivity.hpp"
#include "GLFW/EglUtil.hpp"
#include "GLFW/BaseApp.hpp"
#include "GLFW/GLFWApp.hpp"

// Globals for the native activity, since it is not a class

//================================================================================
//  Native Activity
//================================================================================

BaseApp *  g_pBaseApp = NULL;


// NOTE: You may need to destroy and recreate the graphics context when the app is
//  iconified and restored.  To be determined later.
/*
    case APP_CMD_INIT_WINDOW:
      {
      // The window is being shown, get it ready.
      DBG_INFO ("surfaceCreated()");
      DBG_INFO ("    APP_CMD_INIT_WINDOW");
      appState->NativeWindow = app->window;
      if (appState->NativeWindow != NULL)
        {
        // recreate surfaces
        appState->InitGraphics (app);
        //engine_init_display(engine);
        //engine_draw_frame(engine);
        }
      break;
      }
    case APP_CMD_TERM_WINDOW:
      {
      // The window is being hidden or closed, clean it up.
      DBG_INFO ("surfaceDestroyed()");
      DBG_INFO ("    APP_CMD_TERM_WINDOW");
      appState->ShutdownGraphics ();
      appState->NativeWindow = NULL;
      appState->SetActive (false);
      break;
      }
*/


//-----------------------------------------------------------------------------
static void FocusCallback (GLFWwindow*  pWindow, int bReceiveFocus)
  {
  if (bReceiveFocus)
    {
    g_pBaseApp->SetActive (true);
    }
  else
    {
    g_pBaseApp->SetActive (false);
    }
  }

//-----------------------------------------------------------------------------
static void KeyCallback (GLFWwindow*  pWindow, int iKey, int iScancode, int iAction, int iMods)
  {
  // action is one of: GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE

  //  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  //      glfwSetWindowShouldClose(window, GLFW_TRUE);

  //DBG_INFO("Input Event Type Key");
  g_pBaseApp->HandleKeyEvent (iKey, iAction);
  }

//-----------------------------------------------------------------------------
void MouseButtonCallback (GLFWwindow * pWindow, int iButton, int iAction, int iMods)
  {
  //DBG_INFO ("-------------- MouseButton %d %d %d", iButton, iAction, iMods);
  g_pBaseApp->HandleMouseKeyEvent (iButton, iAction, iMods);
  }

//-----------------------------------------------------------------------------
void MousePositionCallback (GLFWwindow *  pWindow, double fXPos, double fYPos)
  {
  //DBG_INFO ("-------------- MousePosition %f %f", fXPos, fYPos);
  g_pBaseApp->HandleMouseMoveEvent (fXPos, fYPos);
  }



//-----------------------------------------------------------------------------
void ErrorCallback (int error, const char* description)
  {
  DBG_ERROR (description);
  }

/**
 * This is the entry point from the main() function that is defined in the game.
 * probably should be a static function instead of a constructor.
 */

//-----------------------------------------------------------------------------
NativeActivity::NativeActivity (BaseApp *             pBaseApp)
  {
  //DBG_MARK ("NativeActivity");
  srand(time(NULL));

  g_pBaseApp = pBaseApp;

  GLFWwindow *  pWindow;

  glfwSetErrorCallback(ErrorCallback);

  // Initialize the GLFW library
  if (!glfwInit())
    {
    glfwTerminate();
    //return -1;
    };

  // NOTE: Init () must come before InitGraphics () (for GLFW... hopefully for all)
  pBaseApp->Init ();
  pWindow = pBaseApp->InitGraphics ();
  pBaseApp->SetActive (true);


  // The callbacks need to be set up per-window
  glfwSetKeyCallback           (pWindow, KeyCallback);
  glfwSetMouseButtonCallback   (pWindow, MouseButtonCallback);
  glfwSetCursorPosCallback     (pWindow, MousePositionCallback);
  glfwSetWindowFocusCallback   (pWindow, FocusCallback);

  // TODO: Hook up a resize callback to re-query the surface size.

  // Loop until the user closes the window
  while (!glfwWindowShouldClose (pWindow) && (!Shell::AppIsClosing()))
    {
    // Poll for and process events
    glfwPollEvents ();
    pBaseApp->PollForJoysticks ();
    pBaseApp->HandleVrModeChanges ();

    // update state
    // NOTE: Slight chance we need to exit here if glfwWindowShouldClose () is set.
    pBaseApp->Update ();
    }

  // Shutdown and cleanup

  pBaseApp->ShutdownGraphics ();
  pBaseApp->Shutdown ();


  glfwTerminate();
  }



// NOTE:  The game will contain the main() file, and call something like this:
/*
//-----------------------------------------------------------------------------
int main(void)
  {
  NativeActivity activity (new GLFWApp (new HManSceneDelegate ()));
  //NativeActivity (new GLFWApp (new GLTestSceneDelegate ()));
  }
*/
