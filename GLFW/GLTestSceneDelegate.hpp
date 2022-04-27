#ifndef GLTESTSCENEDELEGATE_HPP
#define GLTESTSCENEDELEGATE_HPP

#include "Scene/CrowSceneDelegate.hpp"


//------------------------------------------------------------------------------
class GLTestSceneDelegate : public CrowSceneDelegate
  {
  public:
    BOOL         bIsCreated;

  public:

                   GLTestSceneDelegate  ();

    virtual        ~GLTestSceneDelegate ();

    virtual bool   IsCreated       (void);

    virtual void   Create          (void);

    virtual void   Destroy         (void);

    virtual void   CreateVAOs      (void);

    virtual void   DestroyVAOs     (void);

    virtual void   Advance         (double predictedDisplayTime);

    virtual void   RenderStart     (void);

    virtual void   RenderEye       (const TMatrix32f &  matEyeViewMatrix,
                                    const TMatrix32f &  matProjectionMatrix);

    virtual void   SetViewXForm    (const TMatrix32f &  matTransformIn);

    virtual void   GetViewXForm    (TMatrix32f &  matTransformOut);

    virtual void   HandleAxisEvent (int    iIndexIn,
                                    float  fValueIn);

  };


#endif // GLTESTSCENEDELEGATE_HPP