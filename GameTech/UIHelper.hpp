
#ifndef UIHELPER_HPP
#define UIHELPER_HPP

#include "Sys/Types.hpp"

#include "Composite/Node.hpp"

//#include "Command/Command.hpp"
//#include "Command/CommandParser.hpp"

//#include "Containers/BaseList.hpp"
//#include "WordEntry.hpp"
//#include "Component/ValueRegistryComponent.hpp"
//#include "Gfx/Anim.hpp"
//#include "Util/Signal.h"
//#include "Sys/Timer.hpp"

/**
  @addtogroup gametech
  @{
*/

//------------------------------------------------------------------------------
class UIHelper
  {
  private:

    ValueRegistry *           pReg;

  private:

    static UIHelper *  pInstance;

  public:
                            UIHelper      ();

    virtual                 ~UIHelper     ();

    static UIHelper *       Instance         (VOID)   {if (pInstance == NULL) {pInstance = new UIHelper();}; return pInstance;};

    static VOID             DestroyInstance  (VOID)   {if (pInstance != NULL) {delete pInstance;}; pInstance = NULL;};

    VOID                    TransitionPopScreen        (VOID);

    VOID                    PopScreen                  (VOID);

    VOID                    PushScreen                 (const char *  szScreenNameIn);  // for when you want to return to current screen after the new screen

    VOID                    QueueScreen                (const char *  szScreenNameIn);  // when setting up a list of screens to move through

    VOID                    ShowScreen                 (const char *  szScreenNameIn);  // when you are ready to replace the current screen with a new one.

    VOID                    PlayIntro       (Node *  pnodeIn);

    VOID                    PlayOutro       (Node *  pnodeIn);

    VOID                    PlayIdle        (Node *  pnodeIn);

    VOID                    PlayHidden      (Node *  pnodeIn);

    VOID                    ShowDialog      (const char *  szDialogStateKeyIn);

    VOID                    HideAllDialogs  (VOID)    ;

  };

/** @} */ // end of gametech group


#endif // UIHELPER_HPP




