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


#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Util/RegEx.hpp"
#include "Sys/FilePath.hpp"


const char *  URI_PREFIX_RESOURCE = "res://";
const INT     URI_PREFIX_RESOURCE_LENGTH = 6;

const char *  URI_PREFIX_FILE = "file://";
const INT     URI_PREFIX_FILE_LENGTH = 7;

// NOTE: The below includes won't rebuild due to edits in the includes, unless
//        you do a "make all"
#ifdef WIN32
  #include "FilePathWin32.cpp"
#endif

#ifdef ANDROID_NDK
  //#include "../../../Sys/FilePathAndroid.cpp"
  #include "base/Sys/FilePathAndroid.cpp"
#else
  #ifdef LINUX
    #include "FilePathLinux.cpp"
  #endif
#endif


//-----------------------------------------------------------------------------
//  FilePath
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------
FilePath::FilePath () : strCurrPath ("")
  {
  };


//------------------------------------------------------------------------
FilePath::~FilePath ()
  {
  };


//------------------------------------------------------------------------
RStrArray  FilePath::lsf  (const char *  pszFullPathSpec,
                            BOOL         bFullPaths,
                            EStatus *    pStatusOut)
  {
  return (ls  (pszFullPathSpec, true, false, bFullPaths, pStatusOut));
  };


//------------------------------------------------------------------------
RStrArray  FilePath::lsd  (const char *  pszFullPathSpec,
                           BOOL          bFullPaths,
                           EStatus *     pStatusOut)
  {
  return (ls  (pszFullPathSpec, false, true, bFullPaths, pStatusOut));
  };


//------------------------------------------------------------------------
const RStr  FilePath::operator =  (const RStr & strIn)
  {
  cd (strIn.AsChar());
  return strCurrPath;
  };


//------------------------------------------------------------------------
const RStr  FilePath::operator =  (const char *  pszIn)
  {
  cd (pszIn);
  return strCurrPath;
  };

//------------------------------------------------------------------------------
UINT32 FilePath::SplitPath (const char *  pszPathIn,
                             UINT32       uSeparatorIn,
                             RStrArray &  arrayOut)
  {
  UINT32     uNumStringsFound = 0;

  INT  iLength = strlen (pszPathIn);
  INT  iCurrPos = 0;

  while (iCurrPos < iLength)
    {
    // search for the separator
    INT32  iCopyStart = iCurrPos;
    while (uSeparatorIn != (unsigned char) pszPathIn [iCurrPos])
      {
      ++iCurrPos;
      if (iCurrPos >= iLength) break;
      };

    INT32  iStringLength = iCurrPos - iCopyStart;

    if (iStringLength > 0)
      {
      arrayOut.Append (RStr (""));
      RStr &  strOut = arrayOut [arrayOut.Length () - 1];

      RStr strPathIn (pszPathIn);
      strPathIn.GetMiddle ((UINT32)iCopyStart, (UINT32)iStringLength, strOut);

      ++uNumStringsFound;
      };

    // skip the separator
    ++iCurrPos;
    };

  return (uNumStringsFound);
  };

//------------------------------------------------------------------------------
const char *  FilePath::GetFilenameFromPath  (const char *  szPathIn)
  {
  static RStr        strFilenameOut;


  strFilenameOut = szPathIn;
  INT   iLastSlash = strFilenameOut.ReverseFindChar (DIRECTORY_SEPARATOR);
  strFilenameOut.TruncateLeft (iLastSlash + 1);

  return (strFilenameOut.AsChar ());
  };


//------------------------------------------------------------------------------
const char *  FilePath::GetFilenameNoExtFromPath  (const char *  szPathIn)
  {

  static RStr        strFilenameOut;


  strFilenameOut = szPathIn;
  INT   iLastSlash = strFilenameOut.ReverseFindChar (DIRECTORY_SEPARATOR);
  strFilenameOut.TruncateLeft (iLastSlash + 1);

  INT   iLastPeriod = strFilenameOut.ReverseFindChar ('.');
  if (iLastPeriod != -1)
    {
    strFilenameOut.TruncateRight (iLastPeriod - 1);
    };
  return (strFilenameOut.AsChar ());
  };

//------------------------------------------------------------------------------
const char *  FilePath::GetPathNoExt  (const char *  szPathIn)
  {
  static RStr        strFilenameOut;

  strFilenameOut = szPathIn;
  INT   iLastPeriod = strFilenameOut.ReverseFindChar ('.');
  if (iLastPeriod != -1)
    {
    strFilenameOut.TruncateRight (iLastPeriod - 1);
    };
  return (strFilenameOut.AsChar ());
  };

//------------------------------------------------------------------------------
const char *  FilePath::GetPathNoFilename  (const char *  szPathIn)
  {
  static RStr        strPathOut;


  strPathOut = szPathIn;
  INT   iLastSlash = strPathOut.ReverseFindChar (DIRECTORY_SEPARATOR);
  strPathOut.TruncateRight (iLastSlash);

  return (strPathOut.AsChar ());
  };

//------------------------------------------------------------------------------
const char *  FilePath::GetExtension (const char *  szPathIn)
  {
  static RStr        strExtOut ("");

  RStr   strFilename (szPathIn);
  INT32  iSearch = strFilename.ReverseFindChar ('.');
  if (iSearch != -1)
    {
    strFilename.GetRight (strFilename.Length () - iSearch - 1, strExtOut);
    };
  return (strExtOut.AsChar ());
  };

//------------------------------------------------------------------------------
const char * FilePath::Combine (const char *  szBasePathIn,
                                const char *  szAddPathAIn,
                                const char *  szAddPathBIn,
                                const char *  szAddPathCIn,
                                const char *  szAddPathDIn)
  {
  static RStr        strOut ("");

  strOut = szBasePathIn;
  if (!strOut.EndsWith (Separator()))  {strOut += Separator();};
  strOut += szAddPathAIn;

  if (szAddPathBIn != NULL)
    {
    if (!strOut.EndsWith (Separator()))  {strOut += Separator();};
    strOut += szAddPathBIn;
    };
  if (szAddPathCIn != NULL)
    {
    if (!strOut.EndsWith (Separator()))  {strOut += Separator();};
    strOut += szAddPathCIn;
    };
  if (szAddPathDIn != NULL)
    {
    if (!strOut.EndsWith (Separator()))  {strOut += Separator();};
    strOut += szAddPathDIn;
    };

  return (strOut.AsChar ());
  };


//------------------------------------------------------------------------------
VOID  FilePath::DirTreeSearch  (const char *   pszStartingPath,
                                RegEx &        rexSearchOne,
                                RegEx &        rexSearchTwo,
                                RStrArray &    arrayAllPathsOut)
  {
  // recursively search the directory tree, starting at the directory passed as
  //  the starting path.  If you find a directory containing both rexSearchOne
  //  and rexSearchTwo (either in the files or directories), then add this directory
  //  to arrayAllPathsOut.  We search all sub-directories, except the sub directories
  //  whose parent matched the search parameters.  So leaves cannot contain more leaves.

  RStrArray  arrayToSearch;
  INT        iSearchPos = 0;
  RStrArray  arraySubDirs;
  EStatus    statusDirs;
  INT        iIndex;
  FilePath  filePath;
  INT        iFoundCount = 0;


  //printf ("Start path %s\n", pszStartingPath);
  // initialize search parameters
  arrayToSearch.Append (pszStartingPath);

  while (arrayToSearch.Length () > iSearchPos)
    {
    RStr   strCurrPath = arrayToSearch [iSearchPos];
    ++iSearchPos;

    //printf ("Searching path %s\n", strCurrPath.AsChar ());

    if (DirTreeMatch  (strCurrPath, rexSearchOne, rexSearchTwo))
      {
      arrayAllPathsOut.Append (strCurrPath);
      ++iFoundCount;
      }
    else
      {
      // still no match.  Add the child directories to the search path.
      arraySubDirs = filePath.lsd (strCurrPath, true, &statusDirs);
      INT  iNumDirs = arraySubDirs.Length ();

      for (iIndex = 0; iIndex < iNumDirs; ++iIndex)
        {
        // make sure the directory ends in a slash
        if (arraySubDirs [iIndex].GetAt (arraySubDirs [iIndex].Length () - 1) != DIRECTORY_SEPARATOR)
          {
          arraySubDirs [iIndex] += "/";
          };

        arrayToSearch.Append (arraySubDirs [iIndex]);
        //printf ("Adding sub dir %s\n", arraySubDirs [iIndex].AsChar ());
        };
      };
    };

  // if we didn't find anything, we may be in a subdirectory of a desired directory.
  //  search directly back up the path for a matching directory, but don't consider
  //  searching down subdirectories for matches.

  if (iFoundCount == 0)
    {
    RStr  strCurrPath = pszStartingPath;

    do
      {
      INT  iSlashPos = strCurrPath.ReverseFindChar (DIRECTORY_SEPARATOR, strCurrPath.Length () - 2);

      if (iSlashPos == -1) break;

      strCurrPath.TruncateRight (iSlashPos);

      //printf ("Tail Searching path %s\n", strCurrPath.AsChar ());

      if (DirTreeMatch  (strCurrPath, rexSearchOne, rexSearchTwo))
        {
        arrayAllPathsOut.Append (strCurrPath);
        break;
        }
      } while (strCurrPath.Length () > 2);
    };
  };


//------------------------------------------------------------------------------
BOOL  FilePath::DirTreeMatch  (const char *   pszPathIn,
                               RegEx &        rexSearchOne,
                               RegEx &        rexSearchTwo)
  {
  // this routine is called by DirTreeSearch.  It checks the given path for the existance
  //  of the two search expressions in either files or subdirectories.

  RStrArray  arraySubDirs;
  RStrArray  arrayFiles;
  EStatus    statusFiles;
  EStatus    statusDirs;
  FilePath  filePath;
  INT        iIndex;
  BOOL       bMatchOne = (rexSearchOne.Pattern () == "");
  BOOL       bMatchTwo = (rexSearchTwo.Pattern () == "");

  // get the directory and file listings
  arrayFiles   = filePath.lsf (pszPathIn, true, &statusFiles);
  arraySubDirs = filePath.lsd (pszPathIn, true, &statusDirs);

  // check the files for a match
  if (statusFiles == EStatus::kSuccess)
    {
    INT  iNumFiles = arrayFiles.Length ();

    for (iIndex = 0; iIndex < iNumFiles; ++iIndex)
      {
      //printf ("Considering file %s\n", arrayFiles [iIndex].AsChar ());
      if (arrayFiles [iIndex] * rexSearchOne != "")
        {
        // match
        bMatchOne = true;
        }
      else if (arrayFiles [iIndex] * rexSearchTwo != "")
        {
        // match
        bMatchTwo = true;
        };

      if ((bMatchOne) && (bMatchTwo))
        {
        // match
        //printf ("Found file %s\n", arrayFiles [iIndex].AsChar ());
        break;
        };
      };
    };

  if ((! bMatchOne) || (! bMatchTwo))
    {
    // didn't match on the files.  Try the directories.

    // check the directories for a match
    if (statusDirs == EStatus::kSuccess)
      {
      INT  iNumDirs = arraySubDirs.Length ();

      for (iIndex = 0; iIndex < iNumDirs; ++iIndex)
        {
        // make sure the directory ends in a slash
        if (arraySubDirs [iIndex].GetAt (arraySubDirs [iIndex].Length () - 1) != DIRECTORY_SEPARATOR)
          {
          arraySubDirs [iIndex] += "/";
          };

        if (arraySubDirs [iIndex] * rexSearchOne != "")
          {
          bMatchOne = true;
          }
        else if (arraySubDirs [iIndex] * rexSearchTwo != "")
          {
          bMatchTwo = true;
          };

        if ((bMatchOne) && (bMatchTwo))
          {
          // match
          //printf ("Found dir %s %s\n", arraySubDirs [iIndex].AsChar (), pszPathIn);
          break;
          };
        };
      };
    };
  return (bMatchOne && bMatchTwo);
  };

//------------------------------------------------------------------------------
const char *  FilePath::GetFileInPaths (const char *       szFilenameIn,
                                        const char *       szExtensionIn,
                                        const RStrArray &  arrayPathsIn)
  {
  static RStr  strPath;
  for (INT  iIndex = 0; iIndex < arrayPathsIn.Length (); ++iIndex)
    {
    strPath = arrayPathsIn [iIndex];

    strPath.AppendString (szFilenameIn);
    if (szExtensionIn != NULL)
      {
      strPath.AppendString (szExtensionIn);
      };

    //DBG_INFO ("FilePath::GetFileInPaths checking for file named %s", strPath.AsChar ());

    if (FilePath::FileExists (strPath))
      {
      // file exists
      return (strPath.AsChar ());
      };
    };
  // didn't find the filename along any of the paths.
  return (NULL);
  };



















































#ifdef DEPRECATED

#ifdef ANDROID_NDK
  #define LINUX
  #include <assert.h>
  #include <jni.h>
  #include <string.h>
  #include <sys/types.h>
  #include <android/asset_manager.h>
  #include <android/asset_manager_jni.h>
  #include <android/log.h>

  // global variables needed for interfacing with the java native interface
  //   under Android.
  //JNIEnv*  filePath_jniEnv;
  //jobject  filePath_jobjAssetManager;
  AAssetManager*  filePath_assetManager;
  RStr            filePath_localStoragePath;

  const char *  URI_PREFIX_RESOURCE = "res://";
  const INT     URI_PREFIX_RESOURCE_LENGTH = 6;

  const char *  URI_PREFIX_FILE = "file://";
  const INT     URI_PREFIX_FILE_LENGTH = 7;

#endif // ANDROID_NDK

#ifdef LINUX
  #include <sys/stat.h>
  #include <unistd.h>
  #include <dirent.h>
  #include <stdio.h>
  #include <string.h>

  extern RStr            filePath_localStoragePath;
#endif // LINUX

#include "Util/RegEx.hpp"
#include "Sys/FilePath.hpp"


#define DIRECTORY_SEPARATOR '/'
#define DIRECTORY_SEPARATOR_STR "/"
#define WIN32_DIRECTORY_SEPARATOR '\\'



#ifdef LINUX



#ifdef ANDROID_NDK
//------------------------------------------------------------------------
RStrArray  FilePath::ls  (const char *  pszFullPathSpec,
                          BOOL          bShowFiles,
                          BOOL          bShowDirs,
                          BOOL          bFullPaths,
                          EStatus *     pStatusOut)
  {
  RStrArray        arrayOut;
  //struct stat      statINode;

  if (pStatusOut)  *pStatusOut = EStatus::kSuccess;

  // use the given path, or the current path, or the current directory

  RStr  strPathOut = pszFullPathSpec;
  if (strPathOut.IsEmpty ())
    {
    strPathOut = strCurrPath;
    };
  if (strPathOut.IsEmpty ())
    {
    strPathOut = ".";
    };
  // Note:  Extract any file spec for glob matching.

  RegEx  rexFileSpec ("*", RegEx::kGlob);
  INT32  iFinalSlash = strPathOut.ReverseFindChar (DIRECTORY_SEPARATOR);
  if ((iFinalSlash != -1) && (iFinalSlash != (INT32 (strPathOut.Length ()) - 1)))
    {
    RStr  strFileSpec = strPathOut.ChompRight (strPathOut.Length () - iFinalSlash - 1);

    rexFileSpec.Set (strFileSpec, RegEx::kGlob);
    };

  // make sure the directory does NOT end in a slash
  strPathOut.StripTrailingChar (DIRECTORY_SEPARATOR);

  //DBG_INFO ("Contents of directory: %s", strPathOut.AsChar ());

  AAssetDir *assetDir = AAssetManager_openDir (filePath_assetManager, strPathOut.AsChar ());

  if (assetDir == NULL)
    {
    // Error.  Couldn't open directory.
    RStr  strOut;
    strOut.Format ("FilePath::ls - Unable to open directory %s\n", strPathOut.AsChar ());
    DBG_ERROR (strOut.AsChar ());
    if (pStatusOut)  *pStatusOut = EStatus (EStatus::kFailure, strOut.AsChar ());
    }
  else
    {
    BOOL  bDone = false;
    while (!bDone)
      {
      const char * fName = AAssetDir_getNextFileName (assetDir);

      if (fName != NULL)
        {
        //DBG_INFO ("ls found file %s", fName);
        if (fName [0] != '.')
          {

          RStr  strFullPath = strPathOut + DIRECTORY_SEPARATOR_STR + fName;

          //DBG_INFO (strFullPath.AsChar ());
          if ((bShowFiles) && (rexFileSpec.Match (fName) == fName))
            {
            if (bFullPaths)
              {
              arrayOut.Append (strFullPath);
              }
            else
              {
              arrayOut.Append (fName);
              };
            };
          // Note:  android NDK does not currently allow showing of directories.

          /*
          if (stat (strFullPath.AsChar (), &statINode) != -1)
            {
            if (bShowDirs && S_ISDIR (statINode.st_mode))
              {
              if (bFullPaths)
                {
                arrayOut.Append (strFullPath);
                }
              else
                {
                arrayOut.Append (fName);
                };
              };
            if (bShowFiles && S_ISREG (statINode.st_mode))
              {
              // Perform filename glob matching.
              if (rexFileSpec.Match (fName) == fName)
                {
                // we have a match.
                if (bFullPaths)
                  {
                  arrayOut.Append (strFullPath);
                  }
                else
                  {
                  arrayOut.Append (fName);
                  };
                };
              };
            };
          */
          }
        else
          {
          /* bypass hidden files and . and .. */;
          };
        }
      else
        {
        bDone = true;
        };
      }
    };
  //DBG_INFO ("Done reading directory");
  AAssetDir_close (assetDir);
  return (arrayOut);
  };

#else
//------------------------------------------------------------------------
RStrArray  FilePath::ls  (const char *  pszFullPathSpec,
                          BOOL          bShowFiles,
                          BOOL          bShowDirs,
                          BOOL          bFullPaths,
                          EStatus *     pStatusOut)
  {
  DIR *            dirHandle;
  struct dirent *  pDirEntry;
  BOOL             bDone = false;
  RStrArray        arrayOut;
  struct stat      statINode;

  if (pStatusOut)  *pStatusOut = EStatus::kSuccess;

  // use the given path, or the current path, or the current directory

  RStr  strPathOut = pszFullPathSpec;
  if (strPathOut.IsEmpty ())
    {
    strPathOut = strCurrPath;
    };
  if (strPathOut.IsEmpty ())
    {
    strPathOut = ".";
    };
  // Note:  Extract any file spec for glob matching.

  RegEx  rexFileSpec ("*", RegEx::kGlob);
  INT32  iFinalSlash = strPathOut.ReverseFindChar (DIRECTORY_SEPARATOR);
  if ((iFinalSlash != -1) && (iFinalSlash != (INT32 (strPathOut.Length ()) - 1)))
    {
    RStr  strFileSpec = strPathOut.ChompRight (strPathOut.Length () - iFinalSlash - 1);

    rexFileSpec.Set (strFileSpec, RegEx::kGlob);
    };

  // make sure the directory ends in a slash
  if (strPathOut [strPathOut.Length () -1] != DIRECTORY_SEPARATOR)
    {
    strPathOut += DIRECTORY_SEPARATOR;
    };


  if ((dirHandle = opendir (strPathOut.AsChar ())) == NULL)
    {
    // Error.  Couldn't open directory.
    RStr  strOut;
    strOut.Format ("FilePath::ls - Unable to open directory %s\n", strPathOut.AsChar ());
    if (pStatusOut)  *pStatusOut = EStatus (EStatus::kFailure, strOut.AsChar ());
    }
  else
    {
    while (!bDone)
      {
      pDirEntry = readdir (dirHandle);
      if (pDirEntry != NULL)
        {
        if (pDirEntry->d_name [0] != '.')
          {
          RStr  strFullPath = strPathOut + pDirEntry->d_name;

          if (stat (strFullPath.AsChar (), &statINode) != -1)
            {
            if (bShowDirs && S_ISDIR (statINode.st_mode))
              {
              if (bFullPaths)
                {
                arrayOut.Append (strFullPath);
                }
              else
                {
                arrayOut.Append (pDirEntry->d_name);
                };
              };
            if (bShowFiles && S_ISREG (statINode.st_mode))
              {
              // Perform filename glob matching.
              if (rexFileSpec.Match (pDirEntry->d_name) == pDirEntry->d_name)
                {
                // we have a match.
                if (bFullPaths)
                  {
                  arrayOut.Append (strFullPath);
                  }
                else
                  {
                  arrayOut.Append (pDirEntry->d_name);
                  };
                };
              };
            };
          }
        else
          {
          /* bypass hidden files and . and .. */;
          };
        }
      else
        {
        bDone = true;
        };
      }
    };
  closedir (dirHandle);

  return (arrayOut);
  };
#endif //ANDROID_NDK

//------------------------------------------------------------------------
EStatus  FilePath::cd  (const char *  pszDirIn,
                        BOOL          bChangeShellDir)
  {
  DIR *            dirHandle;
  RStr             strNewPath;

  if (pszDirIn == NULL) {return (EStatus::kFailure);};
  if ((unsigned char) pszDirIn [0] == UINT32 (DIRECTORY_SEPARATOR))
    {
    // absolute directory
    strNewPath = pszDirIn;
    }
  else
    {
    // relative path
    strNewPath = strCurrPath + pszDirIn;
    };

  // make sure the path ends in a slash

  if (strNewPath [strNewPath.Length () - 1] != UINT32 (DIRECTORY_SEPARATOR))
    {
    strNewPath += DIRECTORY_SEPARATOR;
    };

  if ((dirHandle = opendir (strNewPath.AsChar ())) == NULL)
    {
    // Error.  Couldn't open directory.
    return (EStatus::kFailure);
    }

  // valid directory
  closedir (dirHandle);

  strCurrPath = strNewPath;

  if (bChangeShellDir)
    {
    if (chdir (strCurrPath.AsChar ()) != 0)
      {
      return (EStatus::kFailure);
      };
    };

  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------------
bool FilePath::DirExists (const char *  pszPathIn)
  {
  struct stat  statInfo;

  // Note: May want to strip out double slashes here.
  // to be implemented

  if (stat (pszPathIn, &statInfo) == 0)
    {
    if (S_ISDIR (statInfo.st_mode))
      {
      return (true);
      };
    };
  return (false);
  };


#ifdef ANDROID_NDK
//------------------------------------------------------------------------------
bool FilePath::FileExists (const char *  pszPathIn)
  {
  if (strncmp (pszPathIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    AAsset* asset = AAssetManager_open(filePath_assetManager, &pszPathIn[URI_PREFIX_RESOURCE_LENGTH], AASSET_MODE_UNKNOWN);
    if (asset == NULL)
      {
      return (false);
      }
    AAsset_close(asset);
    return (true);
    }
  else if (strncmp (pszPathIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) == 0)
    {
    RStr  strFullPath = filePath_localStoragePath;
    strFullPath += DIRECTORY_SEPARATOR_STR;
    strFullPath += &pszPathIn[URI_PREFIX_FILE_LENGTH];

    FILE* file = fopen(strFullPath.AsChar (), "rb");
    if (file == NULL)
      {
      return (false);
      }
    fclose (file);
    return (true);
    }
  return (false);
  };
#else
//------------------------------------------------------------------------------
bool FilePath::FileExists (const char *  pszPathIn)
  {
  struct stat  statInfo;

  if (stat (pszPathIn, &statInfo) == 0)
    {
    if (S_ISREG (statInfo.st_mode))
      {
      return (true);
      };
    };
  return (false);
  };
#endif

#ifdef ANDROID_NDK
//------------------------------------------------------------------------------
UINT  FilePath::GetFileSize  (const char *  pszPathIn)
  {
  UINT     ulFileSize = 0;

  // read from the asset director via the asset manager.

  //AAssetManager* mgr = AAssetManager_fromJava (filePath_jniEnv, filePath_jobjAssetManager);

  if (strncmp (pszPathIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    AAsset* asset = AAssetManager_open(filePath_assetManager, &pszPathIn[URI_PREFIX_RESOURCE_LENGTH], AASSET_MODE_UNKNOWN);
    if (asset == NULL)
      {
      DBG_ERROR ("_ASSET_NOT_FOUND_ : %s", pszPathIn);
      return (0);
      }

    ulFileSize = AAsset_getLength (asset);
    AAsset_close(asset);
    }
  else if (strncmp (pszPathIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) == 0)
    {
    RStr  strFullPath = filePath_localStoragePath;
    strFullPath += DIRECTORY_SEPARATOR_STR;
    strFullPath += &pszPathIn[URI_PREFIX_FILE_LENGTH];

    FILE* file = fopen(strFullPath.AsChar (), "rb");
    if (file == NULL)
      {
      DBG_ERROR ("_ASSET_NOT_FOUND_ : %s", pszPathIn);
      return (0);
      }
    fseek (file, 0, SEEK_END);
    ulFileSize = ftell (file);
    fclose (file);
    }

  return (ulFileSize);
  };
#else // ANDROID_NDK
//------------------------------------------------------------------------------
UINT  FilePath::GetFileSize  (const char *  pszPathIn)
  {
  FILE *   fp;
  UINT     ulFileSize = 0;

  if ((fp = fopen (pszPathIn, "rb")) != NULL)
    {
    fseek (fp, 0, SEEK_END);
    ulFileSize = ftell (fp);
    };
  return (ulFileSize);
  };
#endif // ANDROID_NDK



//------------------------------------------------------------------------------
VOID FilePath::MakeDir (const char *  pszPathIn)
  {
  if (! DirExists (pszPathIn))
    {
    RStrArray   arrayPath;
    RStr        strCurrPath;

    INT  iNumDirs = SplitPath (pszPathIn, DIRECTORY_SEPARATOR, arrayPath);

    for (INT  iDirIndex = 0; iDirIndex < iNumDirs; ++iDirIndex)
      {
      if ((iDirIndex == 0) && (pszPathIn[0] != DIRECTORY_SEPARATOR))
        {
        // path is relative, so don't start it with a slash.
        strCurrPath += arrayPath [iDirIndex];
        }
      else
        {
        strCurrPath += "/" + arrayPath [iDirIndex];
        };
      if (! DirExists (strCurrPath))
        {
        mkdir (strCurrPath.AsChar (), 0777);
        //printf ("Does not exist %s\n", strCurrPath.AsChar ());
        }
      else
        {
        //printf ("Exists %s\n", strCurrPath.AsChar ());
        };
      };
    };
  };


//------------------------------------------------------------------------------
const char *  FilePath::GetCwd  (VOID)
  {
  static CHAR   szBuffer [512];

  memset (szBuffer, 0, 512);
  if (getcwd (szBuffer, 511) == NULL)
    {
    return (RStr::kEmpty);
    };

  return (szBuffer);
  };



#endif // LINUX



#ifdef WIN32

//------------------------------------------------------------------------
RStrArray  FilePath::ls  (const char *  pszFullPathSpec,
                          BOOL          bShowFiles,
                          BOOL          bShowDirs,
                          BOOL          bFullPaths,
                          EStatus *     pStatusOut)
  {
  RStrArray        arrayOut;

  // Note:  Anytime you store or use a path in TEdit, use UNIX style forward slashes.
  //   However anytime a string is passed to windows, make sure you reverse the slashes
  //   to back-slashes.  You must do any regular expression compares as forward slashes
  //   otherwise you'll wind up escaping characters.


  if (pStatusOut)  *pStatusOut = EStatus::kSuccess;

  // use the given path, or the current path, or the current directory

  RStr  strPathOut = pszFullPathSpec;
  if (strPathOut.IsEmpty ())
    {
    strPathOut = strCurrPath;
    };
  if (strPathOut.IsEmpty ())
    {
    strPathOut = ".";
    };

  // Note:  Extract any file spec for glob matching.

  RegEx  rexFileSpec ("*", RegEx::kGlob);
  INT32  iFinalSlash = strPathOut.ReverseFindChar (DIRECTORY_SEPARATOR);
  if ((iFinalSlash != -1) && (iFinalSlash != (INT32 (strPathOut.Length ()) - 1)))
    {
    RStr  strFileSpec = strPathOut.ChompRight (strPathOut.Length () - iFinalSlash - 1);

    rexFileSpec.Set (strFileSpec, RegEx::kGlob);
    };

  // make sure the directory ends in a slash
  if (strPathOut [strPathOut.Length () -1] != DIRECTORY_SEPARATOR)
    {
    strPathOut += DIRECTORY_SEPARATOR;
    };

  /*
  // if the first part of the filename is a drive letter, remove the opening slash.
  RStr  strDosPathOut = strPathOut;

  if (strDosPathOut * RegEx ("^/?[[:alpha:]]:") != "")
    {
    // found a drive letter
    if (strDosPathOut.GetAt (0) == UINT32 (DIRECTORY_SEPARATOR))
      {
      strDosPathOut.ClipLeft (1);
      };
    };
  */

  // convert to forward slashes so Windows can handle it.
  strPathOut.ReplaceChar (DIRECTORY_SEPARATOR, WIN32_DIRECTORY_SEPARATOR);

  // set a wildcard to search the entire directory.
  RStr  strPathOutWild = strPathOut + "*";


  WIN32_FIND_DATA fdFindFileData;


  HANDLE hFile = FindFirstFile (strPathOutWild.AsChar (), &fdFindFileData);

  if (hFile == INVALID_HANDLE_VALUE)
    {
    // unable to find the first file.
    if (pStatusOut)  *pStatusOut = EStatus (EStatus::kFailure, RStr ("Unable to find first file for ") + strPathOutWild);
    }
  else
    {
    do
      {
      if (fdFindFileData.cFileName [0] != '.')
        {
        RStr  strFullPath = strPathOut + fdFindFileData.cFileName;

        if (GetFileAttributes (strFullPath.AsChar ()) & FILE_ATTRIBUTE_DIRECTORY)
          {
          // directory bit set.  This is a directory
          if (bShowDirs)
            {
            RStr  strPathReturned;

            if (bFullPaths)
              {
              strPathReturned = strFullPath;
              }
            else
              {
              strPathReturned = fdFindFileData.cFileName;
              };
            strPathReturned.ReplaceChar (WIN32_DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);
            arrayOut.Append (strPathReturned);
            };
          }
        else
          {
          // this is a file
          if (bShowFiles)
            {
            // Perform filename glob matching.
            RStr  strFileNameReturned = fdFindFileData.cFileName;
            strFileNameReturned.ReplaceChar (WIN32_DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);


            if (rexFileSpec.Match (strFileNameReturned) == strFileNameReturned)
              {
              // we have a match.
              RStr  strPathReturned;

              if (bFullPaths)
                {
                strPathReturned = strFullPath;
                }
              else
                {
                strPathReturned = strFileNameReturned;
                };
              strPathReturned.ReplaceChar (WIN32_DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);
              arrayOut.Append (strPathReturned);
              };
            };
          };
        }
      else
        {
        /* bypass hidden files and . and .. */;
        };
      } while (FindNextFile (hFile, &fdFindFileData) != 0);

    FindClose (hFile);
    };

  return (arrayOut);
  };


//------------------------------------------------------------------------
EStatus  FilePath::cd  (const RStr &  strDirIn,
                         BOOL          bChangeShellDir)
  {
  RStr             strNewPath;

  if (strDirIn [0] == UINT32 (DIRECTORY_SEPARATOR))
    {
    // absolute directory
    strNewPath = strDirIn;
    }
  else
    {
    // relative path
    strNewPath = strCurrPath + strDirIn;
    };

  // make sure the path ends in a slash

  if (strNewPath [strNewPath.Length () - 1] != UINT32 (DIRECTORY_SEPARATOR))
    {
    strNewPath += DIRECTORY_SEPARATOR;
    };

  // if the first part of the filename is a drive letter, remove the opening slash.
  RStr  strDosPathOut = strNewPath;

  if (strDosPathOut * RegEx ("^/?[[:alpha:]]:") != "")
    {
    // found a drive letter
    if (strDosPathOut.GetAt (0) == UINT32 (DIRECTORY_SEPARATOR))
      {
      strDosPathOut.ClipLeft (1);
      };
    };


  // convert to forward slashes so Windows can handle it.
  strDosPathOut.ReplaceChar (DIRECTORY_SEPARATOR, WIN32_DIRECTORY_SEPARATOR);

  if (GetFileAttributes (strDosPathOut.AsChar ()) & FILE_ATTRIBUTE_DIRECTORY)
    {
    // directory bit set.  This is a directory
    strCurrPath = strNewPath;

    strCurrPath.ReplaceChar (WIN32_DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);


    return (EStatus::kSuccess);
    };
  return (EStatus::kFailure);
  };


//------------------------------------------------------------------------------
bool FilePath::DirExists (const char *  pszPathIn)
  {
  // Note: May want to strip out double slashes here.
  // to be implemented

  if (GetFileAttributes (pszPathIn) & FILE_ATTRIBUTE_DIRECTORY)
    {
    return (true);
    };
  return (false);
  };


//------------------------------------------------------------------------------
bool FilePath::FileExists (const char *  pszPathIn) const
  {
  // Note:  Ideally we should be using GetFileAttributes to check and see if this
  //   is a file or not, but apparently Windows is broken and can't find the file
  //   on a Linux server.  So we will try to open the file to see if exists.


  FILE *  fp;
  if ((fp = fopen (pszPathIn, "rb")) == NULL)
    {
    return (false);
    };
  fclose (fp);
  return (true);

  //if (GetFileAttributes (pszPathIn) & FILE_ATTRIBUTE_NORMAL)
  //  {
  //  return (true);
  //  };
  //return (false);
  };



//------------------------------------------------------------------------------
VOID FilePath::MakeDir (const char *  pszPathIn)
  {
  if (! DirExists (pszPathIn))
    {
    RStrArray   arrayPath;
    RStr        strCurrPath;

    INT  iNumDirs = SplitPath (pszPathIn, DIRECTORY_SEPARATOR, arrayPath);

    for (INT  iDirIndex = 0; iDirIndex < iNumDirs; ++iDirIndex)
      {
      if ((iDirIndex == 0) && (pszPathIn[0] != DIRECTORY_SEPARATOR))
        {
        // path is relative, so don't start it with a slash.
        strCurrPath += arrayPath [iDirIndex];
        }
      else
        {
        strCurrPath += "/" + arrayPath [iDirIndex];
        };
      if (! DirExists (strCurrPath))
        {
        CreateDirectory (strCurrPath.AsChar (), NULL);
        //printf ("Does not exist %s\n", strCurrPath.AsChar ());
        }
      else
        {
        //printf ("Exists %s\n", strCurrPath.AsChar ());
        };
      };
    };
  };


//------------------------------------------------------------------------------
RStr  FilePath::GetCwd  (VOID)
  {
  // to be implemented
  return ("");
  };




#endif // WIN32


//------------------------------------------------------------------------------
VOID  FilePath::DirTreeSearch  (const char *  pszStartingPath,
                                RegEx         rexSearchOne,
                                RegEx         rexSearchTwo,
                                RStrArray &   arrayAllPathsOut)
  {
  // recursively search the directory tree, starting at the directory passed as
  //  the starting path.  If you find a directory containing both rexSearchOne
  //  and rexSearchTwo (either in the files or directories), then add this directory
  //  to arrayAllPathsOut.  We search all sub-directories, except the sub directories
  //  whose parent matched the search parameters.  So leaves cannot contain more leaves.

  RStrArray  arrayToSearch;
  INT        iSearchPos = 0;
  RStrArray  arraySubDirs;
  EStatus    statusDirs;
  INT        iIndex;
  FilePath  filePath;
  INT        iFoundCount = 0;


  //printf ("Start path %s\n", pszStartingPath);
  // initialize search parameters
  arrayToSearch.Append (pszStartingPath);

  while (arrayToSearch.Length () > iSearchPos)
    {
    RStr   strCurrPath = arrayToSearch [iSearchPos];
    ++iSearchPos;

    //printf ("Searching path %s\n", strCurrPath.AsChar ());

    if (DirTreeMatch  (strCurrPath, rexSearchOne, rexSearchTwo))
      {
      arrayAllPathsOut.Append (strCurrPath);
      ++iFoundCount;
      }
    else
      {
      // still no match.  Add the child directories to the search path.
      arraySubDirs = filePath.lsd (strCurrPath, true, &statusDirs);
      INT  iNumDirs = arraySubDirs.Length ();

      for (iIndex = 0; iIndex < iNumDirs; ++iIndex)
        {
        // make sure the directory ends in a slash
        if (arraySubDirs [iIndex].GetAt (arraySubDirs [iIndex].Length () - 1) != DIRECTORY_SEPARATOR)
          {
          arraySubDirs [iIndex] += "/";
          };

        arrayToSearch.Append (arraySubDirs [iIndex]);
        //printf ("Adding sub dir %s\n", arraySubDirs [iIndex].AsChar ());
        };
      };
    };

  // if we didn't find anything, we may be in a subdirectory of a desired directory.
  //  search directly back up the path for a matching directory, but don't consider
  //  searching down subdirectories for matches.

  if (iFoundCount == 0)
    {
    RStr  strCurrPath = pszStartingPath;

    do
      {
      INT  iSlashPos = strCurrPath.ReverseFindChar (DIRECTORY_SEPARATOR, strCurrPath.Length () - 2);

      if (iSlashPos == -1) break;

      strCurrPath.TruncateRight (iSlashPos);

      //printf ("Tail Searching path %s\n", strCurrPath.AsChar ());

      if (DirTreeMatch  (strCurrPath, rexSearchOne, rexSearchTwo))
        {
        arrayAllPathsOut.Append (strCurrPath);
        break;
        }
      } while (strCurrPath.Length () > 2);
    };
  };


//------------------------------------------------------------------------------
BOOL  FilePath::DirTreeMatch  (const char *  pszPathIn,
                               RegEx         rexSearchOne,
                               RegEx         rexSearchTwo)
  {
  // this routine is called by DirTreeSearch.  It checks the given path for the existance
  //  of the two search expressions in either files or subdirectories.

  RStrArray  arraySubDirs;
  RStrArray  arrayFiles;
  EStatus    statusFiles;
  EStatus    statusDirs;
  FilePath  filePath;
  INT        iIndex;
  BOOL       bMatchOne = (rexSearchOne.Pattern () == "") ? true : false;
  BOOL       bMatchTwo = (rexSearchTwo.Pattern () == "") ? true : false;

  // get the directory and file listings
  arrayFiles   = filePath.lsf (pszPathIn, true, &statusFiles);
  arraySubDirs = filePath.lsd (pszPathIn, true, &statusDirs);

  // check the files for a match
  if (statusFiles == EStatus::kSuccess)
    {
    INT  iNumFiles = arrayFiles.Length ();

    for (iIndex = 0; iIndex < iNumFiles; ++iIndex)
      {
      //printf ("Considering file %s\n", arrayFiles [iIndex].AsChar ());
      if (arrayFiles [iIndex] * rexSearchOne != "")
        {
        // match
        bMatchOne = true;
        }
      else if (arrayFiles [iIndex] * rexSearchTwo != "")
        {
        // match
        bMatchTwo = true;
        };

      if ((bMatchOne) && (bMatchTwo))
        {
        // match
        //printf ("Found file %s\n", arrayFiles [iIndex].AsChar ());
        break;
        };
      };
    };

  if ((! bMatchOne) || (! bMatchTwo))
    {
    // didn't match on the files.  Try the directories.

    // check the directories for a match
    if (statusDirs == EStatus::kSuccess)
      {
      INT  iNumDirs = arraySubDirs.Length ();

      for (iIndex = 0; iIndex < iNumDirs; ++iIndex)
        {
        // make sure the directory ends in a slash
        if (arraySubDirs [iIndex].GetAt (arraySubDirs [iIndex].Length () - 1) != DIRECTORY_SEPARATOR)
          {
          arraySubDirs [iIndex] += "/";
          };

        if (arraySubDirs [iIndex] * rexSearchOne != "")
          {
          bMatchOne = true;
          }
        else if (arraySubDirs [iIndex] * rexSearchTwo != "")
          {
          bMatchTwo = true;
          };

        if ((bMatchOne) && (bMatchTwo))
          {
          // match
          //printf ("Found dir %s %s\n", arraySubDirs [iIndex].AsChar (), pszPathIn);
          break;
          };
        };
      };
    };
  return (bMatchOne && bMatchTwo);
  };


#ifndef ANDROID_NDK
//------------------------------------------------------------------------------
EStatus  FilePath::ReadFromFile  (const char *     szFilenameIn,
                                  INT              iStartIndexIn,
                                  INT &            iBufferSizeInOut,
                                  unsigned char *  pbyBufferOut)
  {
  /// Reads up to iBufferSize bytes into pbyBufferOut from szFilenameIn starting at offset iStartIndexIn.
  /// Does not do any of its own allocation; only uses the buffer provided.

  FILE *             fp;
  RStr               strFilenameFinal = szFilenameIn;


  // Make sure input values are valid
  //ASSERT (strFilenameFinal != "");
  if (strFilenameFinal == RStr("")) return EStatus (EStatus::kFailure, "FilePath::ReadFromFile () :  Empty filename passed");
  if (iBufferSizeInOut == 0) return EStatus (EStatus::kFailure, "FilePath::ReadFromFile () : Zero length buffer passed.");

  #ifdef WIN32
    strFilenameFinal.ReplaceChar (DIRECTORY_SEPARATOR, WIN32_DIRECTORY_SEPARATOR);
  #endif

  fp = fopen (strFilenameFinal.AsChar (), "rb");
  if (fp == NULL)
    {
    RStr  strOut;
    strOut.Format ("FilePath::ReadFromFile - Unable to open file %s", strFilenameFinal.AsChar ());
    return (EStatus (EStatus::kFailure, strOut.AsChar ()));
    };

  fseek (fp, 0, SEEK_END);
  UINT32  uSize = ftell (fp);
  fseek (fp, iStartIndexIn, SEEK_SET);

  iBufferSizeInOut = RMin (iBufferSizeInOut, (INT) uSize);

  if (fread (pbyBufferOut, 1, (UINT) iBufferSizeInOut, fp) != (UINT) iBufferSizeInOut)
    {
    fclose (fp);
    return (EStatus (EStatus::kFailure, "FilePath::ReadFromFile () : Unable to read entire file"));
    };

  fclose (fp);
  return (EStatus::kSuccess);
  };
#endif // NOT ANDROID_NDK


#ifdef ANDROID_NDK
//------------------------------------------------------------------------------
EStatus  FilePath::ReadFromFile  (const char *     szFilenameIn,
                                  INT              iStartIndexIn,
                                  INT &            iBufferSizeInOut,
                                  unsigned char *  pbyBufferOut)
  {
  /// Reads up to iBufferSize bytes into pbyBufferOut from szFilenameIn starting at offset iStartIndexIn.
  /// Does not do any of its own allocation; only uses the buffer provided.

  if ((szFilenameIn == NULL) || (szFilenameIn[0] == '\0'))  return EStatus (EStatus::kFailure, "FilePath::ReadFromFile () :  Empty filename passed");
  if (iBufferSizeInOut == 0) return EStatus (EStatus::kFailure, "FilePath::ReadFromFile () : Zero length buffer passed.");

  // read from the asset director via the asset manager.

  //AAssetManager* mgr = AAssetManager_fromJava (filePath_jniEnv, filePath_jobjAssetManager);

  if (strncmp (szFilenameIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    AAsset* asset = AAssetManager_open(filePath_assetManager, &szFilenameIn[URI_PREFIX_RESOURCE_LENGTH], AASSET_MODE_UNKNOWN);
    if (asset == NULL)
      {
      __android_log_print(ANDROID_LOG_ERROR, "FilePath", "_ASSET_NOT_FOUND_");
      RStr  strOut ("FilePath::ReadFromFile () : Unable to find asset ");
      strOut += szFilenameIn;
      return EStatus (EStatus::kFailure, strOut.AsChar ());
      }

    long lSize = AAsset_getLength (asset);
    iBufferSizeInOut = RMin (iBufferSizeInOut, lSize);

    AAsset_read (asset, pbyBufferOut, iBufferSizeInOut);
    AAsset_close(asset);
    }
  else if (strncmp (szFilenameIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) == 0)
    {
    RStr  strFullPath = filePath_localStoragePath;
    strFullPath += DIRECTORY_SEPARATOR_STR;
    strFullPath += &szFilenameIn[URI_PREFIX_FILE_LENGTH];

    FILE* file = fopen(strFullPath.AsChar (), "rb");
    if (file == NULL)
      {
      RStr  strOut ("FilePath::ReadFromFile () : Unable to find file ");
      strOut += szFilenameIn;
      return EStatus (EStatus::kFailure, strOut.AsChar ());
      }

    fseek (file, 0, SEEK_END);
    UINT32  uSize = ftell (file);
    fseek (file, iStartIndexIn, SEEK_SET);

    iBufferSizeInOut = RMin (iBufferSizeInOut, (INT) uSize);

    if (fread (pbyBufferOut, 1, (UINT) iBufferSizeInOut, file) != (UINT) iBufferSizeInOut)
      {
      fclose (file);
      return (EStatus (EStatus::kFailure, "FilePath::ReadFromFile () : Unable to read entire file"));
      };
    fclose (file);
    };

  return (EStatus::kSuccess);
  };
#endif // ANDROID_NDK


#ifndef ANDROID_NDK
//------------------------------------------------------------------------------
EStatus  FilePath::WriteToFile  (const char *     szFilenameIn,
                                 BOOL             bAppend,
                                 INT              iBytesToWriteIn,
                                 unsigned char *  pbyBufferIn)
  {
  FILE *             fp;
  INT                iBytesToWrite = iBytesToWriteIn;
  RStr               strFilenameFinal = szFilenameIn;


  #ifdef WIN32
    strFilenameFinal.ReplaceChar (DIRECTORY_SEPARATOR, WIN32_DIRECTORY_SEPARATOR);
  #endif


  // Make sure input values are valid
  //ASSERT (strFilenameFinal != "");
  if (strFilenameFinal == RStr("")) return EStatus (EStatus::kFailure, "FilePath::WriteToFile () : Empty Filename");
  if (bAppend)
    {
    fp = fopen (strFilenameFinal.AsChar (), "ab");
    }
  else
    {
    fp = fopen (strFilenameFinal.AsChar (), "wb");
    }
  if (fp == NULL)
    {
    //sprintf (szDbgBuffer, "Unable to open file %s", szFilename); DebugMessage (szDbgBuffer);
    RStr  strOut ("FilePath::WriteToFile () : Unable to open file ");
    strOut += strFilenameFinal;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    };
  if (fwrite (pbyBufferIn, 1, iBytesToWrite, fp) != (unsigned int) iBytesToWrite)
    {
    fclose (fp);

    RStr  strOut ("FilePath::WriteToFile () : Unable to write to file ");
    strOut += strFilenameFinal;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    };

  fclose (fp);
  return (EStatus::kSuccess);
  };
#endif // NOT ANDROID_NDK


#ifdef ANDROID_NDK
//------------------------------------------------------------------------------
EStatus  FilePath::WriteToFile  (const char *     szFilenameIn,
                                 BOOL             bAppend,
                                 INT              iBytesToWriteIn,
                                 unsigned char *  pbyBufferIn)
  {
  FILE *             fp;
  INT                iBytesToWrite = iBytesToWriteIn;

  if (strncmp (szFilenameIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    RStr  strOut ("FilePath::WriteToFile () : Writing to resource path not supported ");
    strOut += szFilenameIn;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    }
  else if (strncmp (szFilenameIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) != 0)
    {
    RStr  strOut ("FilePath::WriteToFile () : File path must have file:// URI prefix ");
    strOut += szFilenameIn;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    }
  RStr  strPath = filePath_localStoragePath;
  strPath += DIRECTORY_SEPARATOR_STR;

  RStr  strFilenameFinal = strPath;
  strFilenameFinal += &szFilenameIn[URI_PREFIX_FILE_LENGTH];

  // Make sure input values are valid
  //ASSERT (strFilenameFinal != "");
  if (strFilenameFinal == RStr("")) return EStatus (EStatus::kFailure, "FilePath::WriteToFile () : Empty Filename");

  // make sure the directory exists.
  MakeDir (strPath.AsChar ());

  if (bAppend)
    {
    fp = fopen (strFilenameFinal.AsChar (), "ab");
    }
  else
    {
    fp = fopen (strFilenameFinal.AsChar (), "wb");
    }
  if (fp == NULL)
    {
    //sprintf (szDbgBuffer, "Unable to open file %s", szFilename); DebugMessage (szDbgBuffer);
    RStr  strOut ("FilePath::WriteToFile () : Unable to open file ");
    strOut += strFilenameFinal;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    };
  if (fwrite (pbyBufferIn, 1, iBytesToWrite, fp) != (unsigned int) iBytesToWrite)
    {
    fclose (fp);

    RStr  strOut ("FilePath::WriteToFile () : Unable to write to file ");
    strOut += strFilenameFinal;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    };

  fclose (fp);
  return (EStatus::kSuccess);
  };
#endif // ANDROID_NDK



#endif



