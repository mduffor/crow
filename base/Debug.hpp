
/* -----------------------------------------------------------------
                             Debug.hpp

       Copyright 2002, 2008 Michael T. Duffy.  All Rights Reserved

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// This file is part of TEdit.
//
// TEdit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.
//
// TEdit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TEdit.  If not, see <http://www.gnu.org/licenses/>.


#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"



// Useage note:  Make sure all the files that use these routines have
//  the following two lines in them:
//
//   #include "Debug.hpp"
//   ASSERTFILE (__FILE__)
//


#define ASSERTFILE(str)  static RStr strAssertFile ((const char *) str);  ///< This macro is used to track which source file errors occur in.

// NOTE:  The DBG() macro can be used to have some code only run/present in Debug builds.
//         It is a shorthand way of using an "#ifdef DEBUG" construct.
#ifdef DEBUG
  #define DBG(debug_code)       debug_code
#else
  #define DBG(__ignore)
#endif // DEBUG

#ifdef NO_DEBUG

  #define ASSERT(__ignore)      ((void)0)
  #define DBG_MARK(...)         ((void)0)
  #define DBG_INFO(...)         ((void)0)
  #define DBG_WARNING(...)      ((void)0)
  #define DBG_ERROR(...)        ((void)0)
  #define DBG_RAWERROR(...)     ((void)0)
  #define DBG_RAW(...)          ((void)0)
  #define DBG_ESTATUS(__ignore) ((void)0)


#else


                            /** @brief If the expression is true, an assert message is thrown to the currently active DebugMessages object.
                                @param expr The expression to evaluate
                                @return None
                            */
  #define ASSERT(expr)      {if (!(expr)) if (pdbgMessages) pdbgMessages->Assert  ( (expr), (strAssertFile), (unsigned long)__LINE__);}
                            /** @brief This macro throws the given informational message to the currently active DebugMessages object.
                                @param ... Printf() style formatting parameters to write to the debug object.
                                @return None
                            */
  #define DBG_MARK(...)     {if (pdbgMessages) {strDbgBuffer.Format (__VA_ARGS__); pdbgMessages->RunMark ((strDbgBuffer.AsChar()), (strAssertFile), (unsigned long)__LINE__);};}
                            /** @brief This macro throws the given info message to the currently active DebugMessages object.
                                @param ... Printf() style formatting parameters to write to the debug object.
                                @return None
                            */
  #define DBG_INFO(...)     {if (pdbgMessages) {strDbgBuffer.Format (__VA_ARGS__); pdbgMessages->Info ((strDbgBuffer.AsChar()), (strAssertFile), (unsigned long)__LINE__);};}
                            /** @brief This macro throws the given warning message to the currently active DebugMessages object.
                                @param ... Printf() style formatting parameters to write to the debug object.
                                @return None
                            */
  #define DBG_WARNING(...)  {if (pdbgMessages) {strDbgBuffer.Format (__VA_ARGS__); pdbgMessages->Warning ((strDbgBuffer.AsChar()), (strAssertFile), (unsigned long)__LINE__);};}
                            /** @brief This macro throws the given error message to the currently active DebugMessages object.
                                @param ... Printf() style formatting parameters to write to the debug object.
                                @return None
                            */
  #define DBG_ERROR(...)    {if (pdbgMessages) {strDbgBuffer.Format (__VA_ARGS__); pdbgMessages->Error   ((strDbgBuffer.AsChar()), (strAssertFile), (unsigned long)__LINE__);};}

  #define DBG_RAWERROR(...) {if (pdbgMessages) {strDbgBuffer.Format (__VA_ARGS__); pdbgMessages->RawError (strDbgBuffer.AsChar());};}

  #define DBG_RAW(...)      {if (pdbgMessages) {strDbgBuffer.Format (__VA_ARGS__); pdbgMessages->RawError (strDbgBuffer.AsChar());};}

  #define DBG_ESTATUS(statusIn)   {if (pdbgMessages) {if (statusIn == EStatus::kFailure) DBG_ERROR (statusIn.GetDescription())};}


#endif // NO_DEBUG


                                /** @brief This macro throws the given warning message to the currently active DebugMessages object.
                                    @param pszText The null terminates string to write to the debug object.
                                    @return None
                                */
  #define RWARNING(pszText)      {if (pdbgMessages) pdbgMessages->Warning ((pszText), (strAssertFile), (unsigned long)__LINE__);}
                                /** @brief This macro throws the given error message to the currently active DebugMessages object.
                                    @param pszText The null terminates string to write to the debug object.
                                    @return None
                                */
  #define RERROR(pszText)        {if (pdbgMessages) pdbgMessages->Error   ((pszText), (strAssertFile), (unsigned long)__LINE__);}

                                /** @brief This macro throws the given error message to the currently active DebugMessages object.
                                    @param pszText The null terminates string to write to the debug object.
                                    @return None
                                */
  #define RRAWERROR(pszText)     {if (pdbgMessages) pdbgMessages->RawError   (pszText);}



extern RAVEN_EXPORT RStr    strDbgBuffer;  ///< A temporary buffer that any other module can use to build messages that will be passed to the debug message routines.


// Prototypes

class DebugMessages;
typedef DebugMessages *  PDebugMessages;


extern RAVEN_EXPORT PDebugMessages  pdbgMessages;  ///< The pointer to the singleton

/**

  The DebugMessages class is designed to record and display messages generated
  for debugging purposes.  This includes tracking program flow, asserts, and
  flat out error messages.  This class is designed to be a singleton, with only
  one object used for the entire program.  By inheriting from this class you can
  set up a debug message system that it tailored to the current OS or execution
  environment.

  */
//------------------------------------------------------------------------
class RAVEN_EXPORT DebugMessages
  {
  public:

  enum ELogLevel
    {
    kInfo = 0,
    kDebug = 1,
    kWarning = 2,
    kError = 3
    };

    ELogLevel             kMinLogLevel;

  public:
                                   /** @brief Constructor
                                       @return None
                                   */
                   DebugMessages  ();

                                   /** @brief Destructor
                                       @return None
                                   */
    virtual        ~DebugMessages ();

                                   /** @brief Display an assert message to the user display and the log if the condition evaluates to false.
                                       @param uStatusIn The test condition.
                                       @param strFileIn The source code filename of the line generating this message.
                                       @param uLineIn The souce code line of the command generating this message.
                                       @return None
                                   */
    virtual void   Assert          (UINT32         uStatusIn,
                                    const RStr &   strFileIn,
                                    UINT32         uLineIn);

                                   /** @brief Display a message to the log.  Used for tracking program flow.
                                       @param pszTextIn Message to display to the log.
                                       @param strFileIn The source code filename of the line generating this message.
                                       @param uLineIn The souce code line of the command generating this message.
                                       @return None
                                   */
    virtual void   RunMark         (const char *  pszTextIn,
                                    const RStr &  strFileIn,
                                    UINT32        uLineIn);

                                   /** @brief Displays a message to the log prefaced with an "Info" tag.
                                       @param pszTextIn Message to display to the log.
                                       @param strFileIn The source code filename of the line generating this message.
                                       @param uLineIn The souce code line of the command generating this message.
                                       @return None
                                   */
    virtual void   Info            (const char *  pszTextIn,
                                    const RStr &  strFileIn,
                                    UINT32        uLineIn);

                                   /** @brief Displays a message to the log prefaced with a "Warning" tag.
                                       @param pszTextIn Message to display to the log.
                                       @param strFileIn The source code filename of the line generating this message.
                                       @param uLineIn The souce code line of the command generating this message.
                                       @return None
                                   */
    virtual void   Warning         (const char *  pszTextIn,
                                    const RStr &  strFileIn,
                                    UINT32        uLineIn);

                                   /** @brief Displays a message to the user display and the log prefaced with an "Error" tag.
                                       @param pszTextIn Message to display to the log.
                                       @param strFileIn The source code filename of the line generating this message.
                                       @param uLineIn The souce code line of the command generating this message.
                                       @return None
                                   */
    virtual void   Error           (const char *  pszTextIn,
                                    const RStr &  strFileIn,
                                    UINT32        uLineIn);

                                   /** @brief Displays a message to the user display and the log prefaced with an "Error" tag.  Raw errors do not display file or line info.
                                       @param pszTextIn Message to display to the log.
                                       @return None
                                   */
    virtual void   RawError        (const char *  pszTextIn);



                                   /** @brief Display a message to the log.  Used for tracking program flow.
                                       @param strTextIn Message to display to the log.
                                       @param strFileIn The source code filename of the line generating this message.
                                       @param uLineIn The souce code line of the command generating this message.
                                       @return None
                                   */
    virtual void   RunMark         (const RStr &  strTextIn,
                                    const RStr &  strFileIn,
                                    UINT32        uLineIn);

                                   /** @brief Displays a message to the log prefaced with a "Warning" tag.
                                       @param strTextIn Message to display to the log.
                                       @param strFileIn The source code filename of the line generating this message.
                                       @param uLineIn The souce code line of the command generating this message.
                                       @return None
                                   */
    virtual void   Warning         (const RStr &  strTextIn,
                                    const RStr &  strFileIn,
                                    UINT32        uLineIn);

                                   /** @brief Displays a message to the user display and the log prefaced with an "Error" tag.
                                       @param strTextIn Message to display to the log.
                                       @param strFileIn The source code filename of the line generating this message.
                                       @param uLineIn The souce code line of the command generating this message.
                                       @return None
                                   */
    virtual void   Error           (const RStr &  strTextIn,
                                    const RStr &  strFileIn,
                                    UINT32        uLineIn);


                                   /** @brief Displays a message to the user display and the log prefaced with an "Error" tag.  Raw errors do not display file or line info.
                                       @param strTextIn Message to display to the log.
                                       @return None
                                   */
    virtual void   RawError        (const RStr &  strTextIn);


                                   /** @brief Displays a message to the user display.
                                       @param pszDialogTitleIn The text to display in the title bar of any dialog window that pops up.
                                       @param pszTextIn Message to display to the log.
                                       @return None
                                   */
    virtual void   RawUserDisp     (const char *  pszDialogTitleIn,
                                    const char *  pszTextIn);

                                   /** @brief Displays a message to the log.
                                       @param pszTextIn Message to display to the log.
                                       @return None
                                   */
    virtual void   RawWriteLog     (const char *  pszTextIn,
                                    ELogLevel     kLevelIn);

    virtual void   SetMinLogLevel  (ELogLevel     kLevelIn)  {kMinLogLevel = kLevelIn;};




  };


/**

  The DebugMessagesFactory class is used to create the DebugMessages singleton.
    There should only be one instance of this class, and it is used to initialize
    the debug message system.  It should be subclassed in order to create new
    types of debug message handlers.

  */
//------------------------------------------------------------------------
class RAVEN_EXPORT DebugMessagesFactory
  {
  public:

                             /** @brief Constructor
                                 @return None
                             */
                             DebugMessagesFactory  ();

                             /** @brief Destructor
                                 @return None
                             */
    virtual                  ~DebugMessagesFactory ();

                             /** @brief Initialize creates an instance of the DebugMessages class and stores it in the singleton pointer for this module.  This should be called once per program execution at the very beginning of the program.
                                 @return Pointer to the created instance.
                             */
    static PDebugMessages  Initialize             (VOID);

                             /** @brief Destroys the singleton for this module.  This should be called once per program execution at the very end of the program.
                                 @return None
                             */
    static void             Uninitialize           (VOID);

                             /** @brief Sets the pointer to the DebugMessages singleton
                                 @param pdbgMessagesIn Pointer to the new singleton.
                                 @return None
                             */
    static void                     SetSingleton           (PDebugMessages  pdbgMessagesIn);

  };

//------------------------------------------------------------------------
BOOL IsDebugBuild (VOID);



#endif // DEBUG_HPP
