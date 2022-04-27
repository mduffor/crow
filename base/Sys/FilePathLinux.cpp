/* -----------------------------------------------------------------
                             File Path Linux

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


#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

// NOTE:  This file is conditionally included from FilePath.cpp
// NOTE:  The header guards should avoid double-inclusion here when included from FilePath.cpp
#include "Sys/Types.hpp"
#include "Debug.hpp"
#include "Containers/RStrArray.hpp"
#include "Util/RegEx.hpp"
#include "Sys/FilePath.hpp"

RStr            filePath_localStoragePath;


//-----------------------------------------------------------------------------
//  FilePathLinux
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------
EStatus  FPLinuxExpandFilename (const char *  szFilenameIn,
                                RStr &        strFullPathOut)
  {
  // Make sure input values are valid
  if ((szFilenameIn == NULL) || (szFilenameIn[0] == '\0')) return EStatus::Failure ("FilePath :  Empty filename passed");

  strFullPathOut.Set (szFilenameIn);
  if (strncmp (szFilenameIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    strFullPathOut = filePath_localStoragePath;
    strFullPathOut += DIRECTORY_SEPARATOR_STR;
    strFullPathOut += &szFilenameIn[URI_PREFIX_RESOURCE_LENGTH];
    }
  else if (strncmp (szFilenameIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) == 0)
    {
    strFullPathOut = filePath_localStoragePath;
    strFullPathOut += DIRECTORY_SEPARATOR_STR;
    strFullPathOut += &szFilenameIn[URI_PREFIX_FILE_LENGTH];
    };
  return (EStatus::kSuccess);
  };

//------------------------------------------------------------------------
RStrArray  FilePath::ls  (const char *  pszFullPathSpec,
                          BOOL          bShowFiles,
                          BOOL          bShowDirs,
                          BOOL          bFullPaths,
                          EStatus *     pStatusOut)
  {
  DIR *            dirHandle;
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
    if (pStatusOut)  *pStatusOut = EStatus::Failure (strOut.AsChar ());
    }
  else
    {
    BOOL  bDone = false;
    while (!bDone)
      {
      struct dirent *  pDirEntry = readdir (dirHandle);
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


//------------------------------------------------------------------------------
bool FilePath::FileExists (const char *  pszPathIn)
  {
  struct stat  statInfo;

  RStr  strFullPath;
  EStatus  status = FPLinuxExpandFilename (pszPathIn, strFullPath);
  if (status != EStatus::kSuccess)
    {
    DBG_ERROR (status.GetDescription ());
    return (false);
    };

  if (stat (strFullPath.AsChar (), &statInfo) == 0)
    {
    if (S_ISREG (statInfo.st_mode))
      {
      return (true);
      };
    };
  return (false);
  };

//------------------------------------------------------------------------------
UINT  FilePath::GetFileSize  (const char *  pszPathIn)
  {
  FILE *   fp;
  UINT     ulFileSize = 0;

  RStr  strFullPath;
  EStatus  status = FPLinuxExpandFilename (pszPathIn, strFullPath);
  if (status != EStatus::kSuccess)
    {
    DBG_ERROR (status.GetDescription ());
    return (UINT (0));
    };

  if ((fp = fopen (strFullPath.AsChar (), "rb")) != NULL)
    {
    fseek (fp, 0, SEEK_END);
    ulFileSize = ftell (fp);
    fclose (fp);
    };
  return (ulFileSize);
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
  static char   szBuffer [512];

  memset (szBuffer, 0, 512);
  if (getcwd (szBuffer, 511) == NULL)
    {
    return (RStr::kEmpty);
    };

  return (&szBuffer[0]);
  };


//------------------------------------------------------------------------------
UINT32  FilePath::Separator (VOID)
  {
  return (DIRECTORY_SEPARATOR);
  };


//------------------------------------------------------------------------------
EStatus  FilePath::ReadFromFile  (const char *     szFilenameIn,
                                  INT              iStartIndexIn,
                                  INT &            iBufferSizeInOut,
                                  unsigned char *  pbyBufferOut)
  {
  /// Reads up to iBufferSize bytes into pbyBufferOut from szFilenameIn starting at offset iStartIndexIn.
  /// Does not do any of its own allocation; only uses the buffer provided.

  // Make sure input values are valid
  if ((szFilenameIn == NULL) || (szFilenameIn[0] == '\0')) return EStatus::Failure ("FilePath::ReadFromFile () :  Empty filename passed");

  RStr  strFullPath;
  EStatus  status = FPLinuxExpandFilename (szFilenameIn, strFullPath);
  if (status != EStatus::kSuccess)
    {
    return (status);
    };

  if (!strFullPath.IsEmpty ())
    {
    FILE *  fp = fopen (strFullPath.AsChar (), "rb");
    if (fp == NULL)
      {
      RStr  strOut;
      strOut.Format ("FilePath::ReadFromFile - Unable to open file %s", strFullPath.AsChar());
      return (EStatus::Failure (strOut.AsChar ()));
      };

    fseek (fp, 0, SEEK_END);
    UINT32  uSize = ftell (fp);
    fseek (fp, long (iStartIndexIn), SEEK_SET);

    iBufferSizeInOut = RMin (iBufferSizeInOut, (INT) uSize);

    if (fread (pbyBufferOut, 1, (UINT) iBufferSizeInOut, fp) != (UINT) iBufferSizeInOut)
      {
      fclose (fp);
      return (EStatus::Failure ("FilePath::ReadFromFile () : Unable to read entire file"));
      };

    fclose (fp);
    };

  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------------
EStatus  FilePath::WriteToFile  (const char *     szFilenameIn,
                                 BOOL             bAppend,
                                 INT              iBytesToWriteIn,
                                 unsigned char *  pbyBufferIn)
  {
  FILE *             fp;
  INT                iBytesToWrite = iBytesToWriteIn;

  //DBG_INFO ("FilePathLinux::WriteToFile () : %s", szFilenameIn);

  if (strncmp (szFilenameIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    RStr  strOut ("FilePath::WriteToFile () : Writing to resource path not supported ");
    strOut += szFilenameIn;
    DBG_ERROR (strOut.AsChar ());
    return EStatus::Failure (strOut.AsChar ());
    }
  // NOTE: For Linux, we will relax the restriction that files must be in
  //  the format of a URI

  //else if (strncmp (szFilenameIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) != 0)
  //  {
  //  RStr  strOut ("FilePath::WriteToFile () : File path must have file:// URI prefix ");
  //  strOut += szFilenameIn;
  //  return EStatus::Failure (strOut.AsChar ());
  //  }

  RStr  strFullPath;
  EStatus  status = FPLinuxExpandFilename (szFilenameIn, strFullPath);
  if (status != EStatus::kSuccess)
    {
    DBG_ERROR (status.GetDescription ());
    return (status);
    };

  // Make sure input values are valid
  if (strFullPath.IsEmpty ()) return EStatus::Failure ("FilePath::WriteToFile () : Empty Filename");

  // make sure the directory exists.
  MakeDir (GetPathNoFilename (strFullPath.AsChar ()));

  if (bAppend)
    {
    fp = fopen (strFullPath.AsChar (), "ab");
    }
  else
    {
    fp = fopen (strFullPath.AsChar (), "wb");
    }
  if (fp == NULL)
    {
    //sprintf (szDbgBuffer, "Unable to open file %s", szFilename); DebugMessage (szDbgBuffer);
    RStr  strOut ("FilePath::WriteToFile () : Unable to open file ");
    strOut += strFullPath;
    return EStatus::Failure (strOut.AsChar ());
    };
  if (fwrite (pbyBufferIn, 1, size_t (iBytesToWrite), fp) != (unsigned int) iBytesToWrite)
    {
    fclose (fp);

    RStr  strOut ("FilePath::WriteToFile () : Unable to write to file ");
    strOut += strFullPath;
    return EStatus::Failure (strOut.AsChar ());
    };

  fclose (fp);
  return (EStatus::kSuccess);
  };

//------------------------------------------------------------------------------
const char *  FilePath::ExpandPathURI (const char *  szPathIn)
  {
  static RStr        strPathOut;

  strPathOut = szPathIn;

  if (strncmp (szPathIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) == 0)
    {
    strPathOut = filePath_localStoragePath;
    strPathOut += DIRECTORY_SEPARATOR_STR;
    strPathOut += &szPathIn[URI_PREFIX_FILE_LENGTH];
    }

  return (strPathOut.AsChar ());
  };





