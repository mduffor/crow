
#include <sys/time.h>
#include <time.h>
#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "GLFW/GLTestSceneDelegate.hpp"
#include "GLFW/TestOpenGLES2.hpp"

static long lPrevTimeMs = 0;
static long lTotalTimeMs = 0;

//-----------------------------------------------------------------------------
GLTestSceneDelegate::GLTestSceneDelegate ()  : CrowSceneDelegate ()
  {
  bIsCreated = FALSE;

  };


//-----------------------------------------------------------------------------
GLTestSceneDelegate::~GLTestSceneDelegate ()
  {
  };

//-----------------------------------------------------------------------------
bool  GLTestSceneDelegate::IsCreated  (void)
  {
  return (bIsCreated);
  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::Create  (void)
  {
  bIsCreated = TRUE;

  TestOpenGLES2::Init ();
  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::Destroy  (void)
  {
  bIsCreated = FALSE;
  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::CreateVAOs  (void)
  {
  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::DestroyVAOs  (void)
  {
  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::Advance  (double predictedDisplayTime)
  {
  // calculate the elapsed time since last update
  struct timeval  now;

  // Note: The accuracy of gettimeofday may be horrible, and need to be replaced with glfwGetTime()
  //double time = glfwGetTime();

  gettimeofday(&now, NULL);
  long lCurrTimeMs = now.tv_sec*1000 + now.tv_usec/1000;
  if (lPrevTimeMs == 0)
    {
    // initialize prev time
    lPrevTimeMs = lCurrTimeMs;
    }
  long lDeltaTimeMs = lCurrTimeMs - lPrevTimeMs;
  lPrevTimeMs = lCurrTimeMs;

  lTotalTimeMs += lDeltaTimeMs;

  CrowSceneDelegate::Advance (predictedDisplayTime);

  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::RenderStart  (void)
  {
  // update the scene here, and build the VBOs
  //ALOGV ("GLTestSceneDelegate RenderStart");

  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::RenderEye  (const TMatrix32f &  matEyeViewIn,
                                       const TMatrix32f &  matProjectionIn)
  {

  // Just fill the screen with a color.
  float fValue = (float(lTotalTimeMs) - (floorf(float(lTotalTimeMs / 5000.0f)) * 5000.0f)) / 5000.0f;
  INT iValue = 255 * fValue;
  DBG_INFO ("Shade %d", iValue);
  glClearColor(fValue, fValue, fValue, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  TestOpenGLES2::Draw (FALSE);
  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::SetViewXForm    (const TMatrix32f &  matTransformIn)
  {
  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::GetViewXForm    (TMatrix32f &  matTransformOut)
  {
  };

//-----------------------------------------------------------------------------
void  GLTestSceneDelegate::HandleAxisEvent  (int    iIndexIn,
                                             float  fValueIn)
  {
  };




