/* -----------------------------------------------------------------
                             File Path

     This module implements cross-platform routines for dealing with
     file and directory paths

                            Michael T. Duffy
   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2004-2014, Michael T. Duffy II.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
// Redistributions of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGE.

#ifndef FILEPATH_HPP
#define FILEPATH_HPP


#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RegEx.hpp"

/**
  @addtogroup base
  @{
*/

#ifdef ANDROID_NDK
  #include <jni.h>
  #include <sys/types.h>
  #include <android/asset_manager.h>
  #include <android/asset_manager_jni.h>

  // global variables needed for interfacing with the java native interface
  //   under Android.
  extern AAssetManager*  filePath_assetManager;
  extern RStr            filePath_localStoragePath;

#endif // ANDROID_NDK

#ifdef LINUX
  extern RStr            filePath_localStoragePath;
#endif

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

#define DIRECTORY_SEPARATOR '/'
#define DIRECTORY_SEPARATOR_STR "/"
#define WIN32_DIRECTORY_SEPARATOR '\\'

extern const char *  URI_PREFIX_RESOURCE;
extern const INT     URI_PREFIX_RESOURCE_LENGTH;

extern const char *  URI_PREFIX_FILE;
extern const INT     URI_PREFIX_FILE_LENGTH;

//------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------



//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class FilePath
  {
  private:

    RStr   strCurrPath;  ///< The internal path is set with a cd() command.


  public:


                  FilePath      ();

                  ~FilePath     ();



                                 /** @brief List Files Only.
                                 */
    RStrArray     lsf            (const char *  pszFullPathSpec = NULL,
                                  BOOL          bFullPaths  = true,
                                  EStatus *     statusOut   = NULL);

                                 /** @brief List Directories Only.
                                 */
    RStrArray     lsd            (const char *  pszFullPathSpec = NULL,
                                  BOOL          bFullPaths  = true,
                                  EStatus *     statusOut   = NULL);


                                 /** @brief Change Directory.
                                 */
    EStatus       cd             (const char *  pszDirIn,
                                  BOOL          bChangeShellDir = FALSE);



    static VOID   DirTreeSearch  (const char *   pszStartingPath,
                                  RegEx &        rexSearchOne,
                                  RegEx &        rexSearchTwo,
                                  RStrArray &    arrayAllPathsOut);

    static BOOL   DirTreeMatch   (const char *   pszPathIn,
                                  RegEx &        rexSearchOne,
                                  RegEx &        rexSearchTwo);



    operator      RStr           () const   {return strCurrPath;};

    operator      const char *   () const   {return strCurrPath.AsChar ();};

                                 /** @brief Assignment operator.  Copies the contents of the given string to this string.
                                     @param strIn The string that will be copied.
                                     @return A reference to this string.
                                 */
    const RStr    operator =     (const RStr & strIn);

                                 /** @brief Assignment operator.  Copies the contents of the given zero terminated character array to this string.
                                     @param pszIn The character array that will be copied.
                                     @return A reference to this string.
                                 */
    const RStr    operator =     (const char *  pszIn);

    static UINT32  SplitPath     (const char *  pszPathIn,
                                  UINT32        uSeparatorIn,
                                  RStrArray &   arrayOut);

    static const char *  GetFilenameFromPath       (const char *  szPathIn);

    static const char *  GetFilenameNoExtFromPath  (const char *  szPathIn);

    static const char *  GetPathNoExt              (const char *  szPathIn);

    static const char *  GetPathNoFilename         (const char *  szPathIn);

    static const char *  GetExtension              (const char *  szPathIn);

    static const char *  Combine                   (const char *  szBasePathIn,
                                                    const char *  szAddPathAIn,
                                                    const char *  szAddPathBIn = NULL,
                                                    const char *  szAddPathCIn = NULL,
                                                    const char *  szAddPathDIn = NULL);

    // OS-specific calls

                                 /** @brief List Directories and Files.
                                 */
    RStrArray     ls            (const char *  pszFullPathSpec = NULL,
                                 BOOL          bShowFiles  = true,
                                 BOOL          bShowDirs   = true,
                                 BOOL          bFullPaths  = true,
                                 EStatus *     statusOut   = NULL);

    static bool   DirExists      (const char *  pszPathIn);

    static bool   FileExists     (const char *  pszPathIn);

    static UINT   GetFileSize    (const char *  pszPathIn);

    static VOID   MakeDir        (const char *  pszPathIn);

    static const char *  GetCwd  (VOID);

    static UINT32        Separator (VOID);

    static const char *  ExpandPathURI  (const char *  szPathIn);

    static const char *  GetFileInPaths (const char *       szFilenameIn,
                                         const char *       szExtensionIn, // can be NULL
                                         const RStrArray &  arrayPaths);

    static EStatus  ReadFromFile (const char *     pszFilenameIn,
                                  INT              iStartIndexIn,
                                  INT &            iBufferSizeInOut,
                                  unsigned char *  pbyBufferOut);

    static EStatus  WriteToFile  (const char *     pszFilenameIn,
                                  BOOL             bAppend,
                                  INT              iBytesToWriteIn,
                                  unsigned char *  pbyBufferIn);
  };
/** @} */ // end of base group

#endif // FILEPATH_HPP

