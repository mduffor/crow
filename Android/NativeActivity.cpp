
// NOTE: This file handles all access with the Android OS.  Control is then passed
//  as needed to the BaseApp module, where Oculus SDK related code will be implemented,
//  before passing control down to the game layer in SceneDelegate.

#include <stdlib.h>
#include <time.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#if defined( ANDROID )

#include "Sys/Shell.hpp"

#include "Android/NativeActivity.hpp"
#include "Android/EglUtil.hpp"
#include "Android/BaseApp.hpp"
#include "Android/AndroidApp.hpp"

//static const int CPU_LEVEL      = 2;
//static const int GPU_LEVEL      = 3;

// Globals for the native activity, since it is not a class


//================================================================================
//  Native Activity
//================================================================================


// prototypes for static functions
static void    AppCommandHandler   (struct android_app * app,
                                    int32_t              cmd);

static int32_t AppInputHandler (struct android_app * app,
                                AInputEvent *        event);


/**
 * Process the next main command.
 */
//-----------------------------------------------------------------------------
static void AppCommandHandler (struct android_app * app, int32_t cmd )
  {
  // NOTE:  This function is called from the main thread.

  BaseApp * appState = (BaseApp *)app->userData;

  switch (cmd) // NOTE: These commands are defined in android_native_app_glue.h
    {
    // There is no APP_CMD_CREATE. The ANativeActivity creates the
    // application thread from onCreate(). The application thread
    // then calls android_main().
    case APP_CMD_START:
      {
      //DBG_INFO ("onStart()");
      DBG_INFO ("    APP_CMD_START");
      break;
      }
    case APP_CMD_RESUME:
      {
      //DBG_INFO ("onResume()");
      DBG_INFO ("    APP_CMD_RESUME");
      appState->Resumed = true;
      appState->OnResume ();
      //AndroidApp::AutoHideNavBar (app);
      break;
      }
    case APP_CMD_PAUSE:
      {
      //DBG_INFO ("onPause()");
      DBG_INFO ("    APP_CMD_PAUSE");
      appState->Resumed = false;
      appState->OnPause ();
      break;
      }
    case APP_CMD_STOP:
      {
      //DBG_INFO ("onStop()");
      DBG_INFO ("    APP_CMD_STOP");
      break;
      }
    case APP_CMD_DESTROY:
      {
      //DBG_INFO ("onDestroy()");
      DBG_INFO ("    APP_CMD_DESTROY");
      appState->NativeWindow = NULL;
      break;
      }
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
      //AndroidApp::AutoHideNavBar (app);
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

    case APP_CMD_SAVE_STATE:
      {
      // The system has asked us to save our current state.  Do so.
      // TODO:
      //engine->app->savedState = malloc(sizeof(struct saved_state));
      //*((struct saved_state*)engine->app->savedState) = engine->state;
      //engine->app->savedStateSize = sizeof(struct saved_state);
      break;
      }

    case APP_CMD_GAINED_FOCUS:
      {
      // if using the accelerometer, enable it here.
      appState->SetActive (true);
      //AndroidApp::AutoHideNavBar (app);
      //AndroidApp::bRefreshNavBar = TRUE;
      break;
      }

    case APP_CMD_LOST_FOCUS:
      {
      // if using the accelerometer, disable it here.
      appState->SetActive (false);
      break;
      }

    default:
      break;

    }
  }

// Process the next input event.
//-----------------------------------------------------------------------------
static int32_t AppInputHandler (struct android_app *  app,
                                AInputEvent *         event)
{
  BaseApp * appState = (BaseApp *)app->userData;
  const int type = AInputEvent_getType (event);
  if  (type == AINPUT_EVENT_TYPE_KEY )
  {
    //DBG_INFO("Input Event Type Key");
    const int keyCode = AKeyEvent_getKeyCode (event);
    const int action = AKeyEvent_getAction (event);
    return appState->HandleKeyEvent (keyCode, action);
  }
  else if  (type == AINPUT_EVENT_TYPE_MOTION )
  {
    const int source = AInputEvent_getSource (event);
    //DBG_INFO("Input Event Type Motion source %x", source);
    // Events with source == AINPUT_SOURCE_TOUCHSCREEN come from the phone's builtin touch screen.
    // Events with source == AINPUT_SOURCE_MOUSE come from the trackpad on the right side of the GearVR.
    if  (source == AINPUT_SOURCE_TOUCHSCREEN || source == AINPUT_SOURCE_MOUSE )
    {
      // Note: We use the RawX and RawY here because we want to use the entire touchpad, and
      //  we don't restrict it to a relative position of any screen window.

      // TODO: Add multi-touch support here.  The below code doesn't distinguish between
      //  first touch, second touch, etc.  See old onTouchEvent code in VRActivity.java
      //  for reference.

      const int action = AKeyEvent_getAction (event ) & AMOTION_EVENT_ACTION_MASK;
      const float x = AMotionEvent_getRawX (event, 0);
      const float y = AMotionEvent_getRawY (event, 0);
      const float pressure = AMotionEvent_getPressure (event, 0);
      return appState->HandleTouchEvent (action, x, y, pressure);
    }
    if (source == AINPUT_SOURCE_JOYSTICK )
      {
      // joystick input.
      const int action = AMotionEvent_getAction (event ) & AMOTION_EVENT_ACTION_MASK;
      const int pointer_index = AMotionEvent_getAction (event ) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK;
      //const float x = AMotionEvent_getRawX (event, 0);
      //const float y = AMotionEvent_getRawY (event, 0);
      //const float pressure = AMotionEvent_getPressure (event, 0);

      if (action == AMOTION_EVENT_ACTION_MOVE)
        {
        // http://developer.android.com/training/game-controllers/controller-input.html#joystick

        //int  axis_index = AMotionEvent_getPointerId (event, pointer_index);
        //appState->HandleAxisEvent (axis_index, AMotionEvent_getAxisValue (event, axis_index, pointer_index));

        //NOTE:  I wish I could figure out the axis index from the motion event, but no one seems to
        //  have posted code on how to do this yet.  I may need to reverse-engineer the message.
        appState->HandleAxisEvent (AMOTION_EVENT_AXIS_X, AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_X, pointer_index));
        appState->HandleAxisEvent (AMOTION_EVENT_AXIS_Y, AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_Y, pointer_index));
        appState->HandleAxisEvent (AMOTION_EVENT_AXIS_Z, AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_Z, pointer_index));
        appState->HandleAxisEvent (AMOTION_EVENT_AXIS_RZ, AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_RZ, pointer_index));
        appState->HandleAxisEvent (AMOTION_EVENT_AXIS_HAT_X, AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_HAT_X, pointer_index));
        appState->HandleAxisEvent (AMOTION_EVENT_AXIS_HAT_Y, AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_HAT_Y, pointer_index));
        appState->HandleAxisEvent (AMOTION_EVENT_AXIS_GAS, AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_GAS, pointer_index));
        appState->HandleAxisEvent (AMOTION_EVENT_AXIS_BRAKE, AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_BRAKE, pointer_index));

        /*
        // left joystick  AMOTION_EVENT_AXIS_X = 0, AMOTION_EVENT_AXIS_Y

        float lx = AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_X, pointer_index);
        float ly = AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_Y, pointer_index);
        float rx = AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_Z, pointer_index);
        float ry = AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_RZ, pointer_index);
        float hx = AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_HAT_X, pointer_index);
        float hy = AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_HAT_Y, pointer_index);

        float rt = AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_GAS, pointer_index);
        float lt = AMotionEvent_getAxisValue (event, AMOTION_EVENT_AXIS_BRAKE, pointer_index);

        // right joystick  AMOTION_EVENT_AXIS_Z = 0, AMOTION_EVENT_AXIS_RZ

        // clamp for a flat area in the center
        float THRESHOLD = 0.01f;

        if ((lx >= -THRESHOLD) && (lx <= THRESHOLD)) lx = 0.0f;
        if ((ly >= -THRESHOLD) && (ly <= THRESHOLD)) ly = 0.0f;
        if ((rx >= -THRESHOLD) && (rx <= THRESHOLD)) rx = 0.0f;
        if ((ry >= -THRESHOLD) && (ry <= THRESHOLD)) ry = 0.0f;
        if ((hx >= -THRESHOLD) && (hx <= THRESHOLD)) hx = 0.0f;
        if ((hy >= -THRESHOLD) && (hy <= THRESHOLD)) hy = 0.0f;
        if ((lt >= -THRESHOLD) && (lt <= THRESHOLD)) lt = 0.0f;
        if ((rt >= -THRESHOLD) && (rt <= THRESHOLD)) rt = 0.0f;

        DBG_INFO("  Joystick event left x %0.04g y %0.04g  right x %0.04g y %0.04g  trigger l %0.04g r %0.04g", lx, ly, rx, ry, lt, rt);

        char szOut [96];
        memset (szOut, ' ', sizeof (szOut));
        szOut [95] = '\0';

        for (int i = 0; i <= 47; ++i)
          {
          float  fValue = AMotionEvent_getAxisValue (event, i, pointer_index);
          if (fValue >= THRESHOLD) szOut[i*2] = 'U';
          else if (fValue <= -THRESHOLD) szOut[i*2] = 'D';
          else  szOut[i*2] = '-';
          };
        DBG_INFO ("%s", szOut);

        // L Joy - y neg/up, pos/down, x neg/left, pos/right
        // R Joy - y neg/up, pos/down, x neg/left, pos/right
        // Gamepad hatx neg/left, haty neg/up, pos/down
        // Trigger L BRAKE, R GAS
        */
        };
      }
  }
  return 0;
}


/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */

//-----------------------------------------------------------------------------
NativeActivity::NativeActivity (struct android_app *  pApp,
                                BaseApp *             pBaseApp)
  {
  // NOTE: This function runs on its own thread, separate from the Android
  //  UI thread.

  // NOTE:  This can be called multiple times if the activity is destroyed
  //  and then recreated, such as when paging out from the game.  In that case,
  //  the same pBaseApp should be passed by the caller each time.
  DebugMessagesFactory::Initialize ();

  DBG_INFO ("------====== Running app version %s ======------", Shell::GetVersion ());

  srand((unsigned int)time(NULL));

  DBG_INFO ("----------------------------------------------------------------");
  DBG_INFO ("android_app_entry()");
  DBG_INFO ("    android_main()");

  // We create the java object at this level because we need the android_app
  //  to do so.  We then pass it down to the BaseApp class, where it will be used.
  TJava java;
  java.Vm = pApp->activity->vm;
  (*java.Vm).AttachCurrentThread (&java.Env, NULL);
  java.ActivityObject = pApp->activity->clazz;

  // TODO: This is where the type of AndroidApp is created.  It needs to be
  //   a configuration option of some sort, and I'm not sure how best to approach
  //   that considering this file is now in a Crow library.  Another option might
  //   be to move android_main into the app itself, and have the current function
  //   called from that, with a BaseApp instance passed in.

  //ovrApp  appState;  // Creation of App
  //AndroidApp appState;

  pApp->userData = pBaseApp;
  pApp->onAppCmd = AppCommandHandler;
  pApp->onInputEvent = AppInputHandler;


  if (pApp->savedState != NULL)
    {
    // We are starting with a previous saved state; restore from it.
    // TODO
    //engine.state = *(struct saved_state*)state->savedState;
    // NOTE:  Be sure to deallocate saved state once you have read from it.
    }

  // NOTE:  When you go back to working with GearVR, you may need to change the order
  //         of Init and InitGraphics back to how it was.
  pBaseApp->InitGraphics (pApp);
  pBaseApp->Init (java, pApp);
  pBaseApp->SetActive (true);
  //AndroidApp::AutoHideNavBar (pApp);

  while ((pApp->destroyRequested == 0) && (!Shell::AppIsClosing()))

    {
    // Read all pending events.
    for  (; ; )
      {
      int events;
      struct android_poll_source * source;
      // if not Active and not being destroyed, block until looper event
      const int timeoutMilliseconds =  (!pBaseApp->IsActive () && pApp->destroyRequested == 0) ? -1 : 0;
      if  (ALooper_pollAll (timeoutMilliseconds, NULL, &events, (void **)&source) < 0)
        {
        break;
        }

      // Process this event.
      if  (source != NULL)
        {
        source->process (pApp, source);
        }

      pBaseApp->HandleVrModeChanges (pApp);
      } // end read all pending events

    // NOTE: Slight chance we need to exit here if destroyRequested is set.  Occulus code doesn't do it, but
    //  native activity sample code does exit.  Make determination after testing.

    pBaseApp->Update ();
    } // while not app exiting

  // Shutdown and cleanup

  pBaseApp->ShutdownGraphics ();
  pBaseApp->Shutdown ();

  (*pBaseApp->Java.Vm).DetachCurrentThread ();
  }

#endif // ANDROID