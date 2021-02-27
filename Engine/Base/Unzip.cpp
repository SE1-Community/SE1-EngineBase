/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

// Unzip.cpp : Defines the entry point for the console application.

#include "StdH.h"
#include <Engine/Base/Stream.h>
#include <Engine/Base/FileName.h>
#include <Engine/Base/Translation.h>
#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/Synchronization.h>
#include <Engine/Math/Functions.h>

#include <Engine/Templates/StaticArray.cpp>
#include <Engine/Templates/StaticStackArray.cpp>

#include <Engine/Base/Unzip.h>

extern CTCriticalSection zip_csLock; // critical section for access to zlib functions

// Get error string for a zlib error
CTString GetZlibError(int ierr) {
  switch (ierr) {
    case Z_OK:            return TRANS("Z_OK           "); break;
    case Z_STREAM_END:    return TRANS("Z_STREAM_END   "); break;
    case Z_NEED_DICT:     return TRANS("Z_NEED_DICT    "); break;
    case Z_STREAM_ERROR:  return TRANS("Z_STREAM_ERROR "); break;
    case Z_DATA_ERROR:    return TRANS("Z_DATA_ERROR   "); break;
    case Z_MEM_ERROR:     return TRANS("Z_MEM_ERROR    "); break;
    case Z_BUF_ERROR:     return TRANS("Z_BUF_ERROR    "); break;
    case Z_VERSION_ERROR: return TRANS("Z_VERSION_ERROR"); break;

    case Z_ERRNO: {
      CTString strError;
      strError.PrintF(TRANS("Z_ERRNO: %s"), strerror(errno));
      return strError;
    } break;

    default: {
      CTString strError;
      strError.PrintF(TRANS("Unknown ZLIB error: %d"), ierr);
      return strError;
    } break;
  }
}

// Constructor
CZipHandle::CZipHandle(void) {
  zh_bOpen = FALSE;
  zh_fFile = NULL;
  zh_pubBufIn = NULL;
  memset(&zh_zstream, 0, sizeof(zh_zstream));
}

void CZipHandle::Clear(void) {
  zh_bOpen = FALSE;
  zh_zeEntry.Clear();

  // clear the zlib stream
  CTSingleLock slZip(&zip_csLock, TRUE);
  inflateEnd(&zh_zstream);
  memset(&zh_zstream, 0, sizeof(zh_zstream));

  // free buffers
  if (zh_pubBufIn != NULL) {
    FreeMemory(zh_pubBufIn);
    zh_pubBufIn = NULL;
  }
  // close the zip archive file
  if (zh_fFile != NULL) {
    fclose(zh_fFile);
    zh_fFile = NULL;
  }
}

void CZipHandle::ThrowZLIBError_t(int ierr, const CTString &strDescription) {
  ThrowF_t(TRANS("(%s/%s) %s - ZLIB error: %s - %s"), (const CTString &)*zh_zeEntry.ze_pfnmArchive,
           (const CTString &)zh_zeEntry.ze_fnm, strDescription, GetZlibError(ierr), zh_zstream.msg);
}

// All files in all active zip archives
static CStaticStackArray<CZipEntry> _azeFiles;

// Handles for currently open files
static CStaticStackArray<CZipHandle> _azhHandles;

// Filenames of all archives
static CStaticStackArray<CTFileName> _afnmArchives;

// Convert slashes to backslashes in a file path
void ConvertSlashes(char *p) {
  while (*p != 0) {
    if (*p == '/') {
      *p = '\\';
    }
    p++;
  }
}

// Read directory of a zip archive and add all files in it to active set
void ReadZIPDirectory_t(CTFileName *pfnmZip) {
  FILE *f = fopen(*pfnmZip, "rb");

  if (f == NULL) {
    ThrowF_t(TRANS("%s: Cannot open file (%s)"), (CTString &)*pfnmZip, strerror(errno));
  }

  // start at the end of file, minus expected minimum overhead
  fseek(f, 0, SEEK_END);
  int iPos = ftell(f) - sizeof(long) - sizeof(EndOfDir) + 2;

  // do not search more than 128k (should be around 65k at most)
  int iMinPos = iPos - 128 * 1024;

  if (iMinPos < 0) {
    iMinPos = 0;
  }

  EndOfDir eod;
  BOOL bEODFound = FALSE;

  // while not at beginning
  for (; iPos > iMinPos; iPos--) {
    // read signature
    fseek(f, iPos, SEEK_SET);

    int slSig;
    fread(&slSig, sizeof(slSig), 1, f);

    // if this is the sig
    if (slSig == SIGNATURE_EOD) {
      // read directory end
      fread(&eod, sizeof(eod), 1, f);

      // if multi-volume zip
      if (eod.eod_swDiskNo != 0 || eod.eod_swDirStartDiskNo != 0 || eod.eod_swEntriesInDirOnThisDisk != eod.eod_swEntriesInDir) {
        // fail
        ThrowF_t(TRANS("%s: Multi-volume zips are not supported"), (CTString &)*pfnmZip);
      }

      // check against empty zips
      if (eod.eod_swEntriesInDir <= 0) {
        // fail
        ThrowF_t(TRANS("%s: Empty zip"), (CTString &)*pfnmZip);
      }

      // all ok
      bEODFound = TRUE;
      break;
    }
  }

  // if eod not found
  if (!bEODFound) {
    // fail
    ThrowF_t(TRANS("%s: Cannot find 'end of central directory'"), (CTString &)*pfnmZip);
  }

  // check if the zip is from a mod
  BOOL bMod = pfnmZip->HasPrefix(_fnmApplicationPath + "Mods\\") || pfnmZip->HasPrefix(_fnmCDPath + "Mods\\");

  // go to the beginning of the central dir
  fseek(f, eod.eod_slDirOffsetInFile, SEEK_SET);
  INDEX ctFiles = 0;

  // for each file
  for (INDEX iFile = 0; iFile < eod.eod_swEntriesInDir; iFile++) {
    // read the sig
    int slSig;
    fread(&slSig, sizeof(slSig), 1, f);

    // if this is not the expected sig
    if (slSig != SIGNATURE_FH) {
      // fail
      ThrowF_t(TRANS("%s: Wrong signature for 'file header' number %d'"), (CTString &)*pfnmZip, iFile);
    }

    // read its header
    FileHeader fh;
    fread(&fh, sizeof(fh), 1, f);

    // read the filename
    const SLONG slMaxFileName = 512;
    char strBuffer[slMaxFileName + 1];
    memset(strBuffer, 0, sizeof(strBuffer));

    if (fh.fh_swFileNameLen > slMaxFileName) {
      ThrowF_t(TRANS("%s: Too long filepath in zip"), (CTString &)*pfnmZip);
    }

    if (fh.fh_swFileNameLen <= 0) {
      ThrowF_t(TRANS("%s: Invalid filepath length in zip"), (CTString &)*pfnmZip);
    }

    fread(strBuffer, fh.fh_swFileNameLen, 1, f);

    // skip eventual comment and extra fields
    if (fh.fh_swFileCommentLen + fh.fh_swExtraFieldLen > 0) {
      fseek(f, fh.fh_swFileCommentLen + fh.fh_swExtraFieldLen, SEEK_CUR);
    }

    // if the file is directory
    if (strBuffer[strlen(strBuffer) - 1] == '/') {
      // check size
      if (fh.fh_slUncompressedSize != 0 || fh.fh_slCompressedSize != 0) {
        ThrowF_t(TRANS("%s/%s: Invalid directory"), (CTString &)*pfnmZip, strBuffer);
      }

    // if the file is real file
    } else {
      ctFiles++;

      // convert filename
      ConvertSlashes(strBuffer);

      // create a new entry
      CZipEntry &ze = _azeFiles.Push();

      // remember the file's data
      ze.ze_fnm = CTString(strBuffer);
      ze.ze_pfnmArchive = pfnmZip;
      ze.ze_slCompressedSize = fh.fh_slCompressedSize;
      ze.ze_slUncompressedSize = fh.fh_slUncompressedSize;
      ze.ze_slDataOffset = fh.fh_slLocalHeaderOffset;
      ze.ze_ulCRC = fh.fh_slCRC32;
      ze.ze_bMod = bMod;

      // check for compressopn
      if (fh.fh_swCompressionMethod == 0) {
        ze.ze_bStored = TRUE;
      } else if (fh.fh_swCompressionMethod == 8) {
        ze.ze_bStored = FALSE;
      } else {
        ThrowF_t(TRANS("%s/%s: Only 'deflate' compression is supported"), (CTString &)*ze.ze_pfnmArchive, ze.ze_fnm);
      }
    }
  }

  // if error reading
  if (ferror(f)) {
    // fail
    ThrowF_t(TRANS("%s: Error reading central directory"), (CTString &)*pfnmZip);
  }

  // report that file was read
  CPrintF(TRANS("  %s: %d files\n"), (CTString &)*pfnmZip, ctFiles++);
}

// Add one zip archive to current active set
void UNZIPAddArchive(const CTFileName &fnm) {
  // remember its filename
  CTFileName &fnmNew = _afnmArchives.Push();
  fnmNew = fnm;
}

// Read directory of an archive
void ReadOneArchiveDir_t(CTFileName &fnm) {
  // remember current number of files
  INDEX ctOrgFiles = _azeFiles.Count();

  // read the directory and add all files
  try {
    ReadZIPDirectory_t(&fnm);

  // if failed
  } catch (char *) {
    // if some files were added
    if (ctOrgFiles < _azeFiles.Count()) {
      // remove them
      if (ctOrgFiles == 0) {
        _azeFiles.PopAll();
      } else {
        _azeFiles.PopUntil(ctOrgFiles - 1);
      }
    }

    // cascade the error
    throw;
  }
}

int qsort_ArchiveCTFileName_reverse(const void *elem1, const void *elem2) {
  // get the filenames
  const CTFileName &fnm1 = *(CTFileName *)elem1;
  const CTFileName &fnm2 = *(CTFileName *)elem2;

  // find if any is in a mod or on CD
  BOOL bMod1 = fnm1.HasPrefix(_fnmApplicationPath + "Mods\\");
  BOOL bCD1 = fnm1.HasPrefix(_fnmCDPath);
  BOOL bModCD1 = fnm1.HasPrefix(_fnmCDPath + "Mods\\");
  BOOL bMod2 = fnm2.HasPrefix(_fnmApplicationPath + "Mods\\");
  BOOL bCD2 = fnm2.HasPrefix(_fnmCDPath);
  BOOL bModCD2 = fnm2.HasPrefix(_fnmCDPath + "Mods\\");

  // calculate priorities based on location of gro file
  INDEX iPriority1 = 0;

  if (bMod1) {
    iPriority1 = 3;
  } else if (bModCD1) {
    iPriority1 = 2;
  } else if (bCD1) {
    iPriority1 = 0;
  } else {
    iPriority1 = 1;
  }

  INDEX iPriority2 = 0;

  if (bMod2) {
    iPriority2 = 3;
  } else if (bModCD2) {
    iPriority2 = 2;
  } else if (bCD2) {
    iPriority2 = 0;
  } else {
    iPriority2 = 1;
  }

  // find sorting order
  if (iPriority1 < iPriority2) {
    return +1;
  } else if (iPriority1 > iPriority2) {
    return -1;
  } else {
    return -stricmp(fnm1, fnm2);
  }
}
// read directories of all currently added archives, in reverse alphabetical order
void UNZIPReadDirectoriesReverse_t(void) {
  // if no archives
  if (_afnmArchives.Count() == 0) {
    // do nothing
    return;
  }

  // sort the archive filenames reversely
  qsort(&_afnmArchives[0], _afnmArchives.Count(), sizeof(CTFileName), qsort_ArchiveCTFileName_reverse);

  CTString strAllErrors = "";

  // for each archive
  for (INDEX iArchive = 0; iArchive < _afnmArchives.Count(); iArchive++) {
    // read its directory
    try {
      ReadOneArchiveDir_t(_afnmArchives[iArchive]);

    // if failed
    } catch (char *strError) {
      // remember the error
      strAllErrors += strError;
      strAllErrors += "\n";
    }
  }

  // if there were errors
  if (strAllErrors != "") {
    // report them
    ThrowF_t("%s", strAllErrors);
  }
}

// Check if a zip file entry exists
BOOL UNZIPFileExists(const CTFileName &fnm) {
  // for each file
  for (INDEX iFile = 0; iFile < _azeFiles.Count(); iFile++) {
    // if it is that one
    if (_azeFiles[iFile].ze_fnm == fnm) {
      return TRUE;
    }
  }

  return FALSE;
}

// Enumeration for all files in all zips
INDEX UNZIPGetFileCount(void) {
  return _azeFiles.Count();
}

const CTFileName &UNZIPGetFileAtIndex(INDEX i) {
  return _azeFiles[i].ze_fnm;
}

BOOL UNZIPIsFileAtIndexMod(INDEX i) {
  return _azeFiles[i].ze_bMod;
}

// Get index of a file (-1 for no file)
INDEX UNZIPGetFileIndex(const CTFileName &fnm) {
  // for each file
  for (INDEX iFile = 0; iFile < _azeFiles.Count(); iFile++) {
    // if it is that one
    if (_azeFiles[iFile].ze_fnm == fnm) {
      return iFile;
    }
  }

  return -1;
}

// Get info on a zip file entry
void UNZIPGetFileInfo(INDEX iHandle, CTFileName &fnmZip, SLONG &slOffset, SLONG &slSizeCompressed, SLONG &slSizeUncompressed, BOOL &bCompressed) {
  // check handle number
  if (iHandle < 0 || iHandle >= _azhHandles.Count()) {
    ASSERT(FALSE);
    return;
  }

  // get the handle
  CZipHandle &zh = _azhHandles[iHandle];

  // check the handle
  if (!zh.zh_bOpen) {
    ASSERT(FALSE);
    return;
  }

  // get parameters
  fnmZip = *zh.zh_zeEntry.ze_pfnmArchive;
  bCompressed = !zh.zh_zeEntry.ze_bStored;
  slOffset = zh.zh_zeEntry.ze_slDataOffset;
  slSizeCompressed = zh.zh_zeEntry.ze_slCompressedSize;
  slSizeUncompressed = zh.zh_zeEntry.ze_slUncompressedSize;
}

// Open a zip file entry for reading
INDEX UNZIPOpen_t(const CTFileName &fnm) {
  CZipEntry *pze = NULL;

  // for each file
  for (INDEX iFile = 0; iFile < _azeFiles.Count(); iFile++) {
    // if it is that one
    if (_azeFiles[iFile].ze_fnm == fnm) {
      // stop searching
      pze = &_azeFiles[iFile];
      break;
    }
  }

  // if not found
  if (pze == NULL) {
    // fail
    ThrowF_t(TRANS("File not found: %s"), (const CTString &)fnm);
  }

  // for each existing handle
  BOOL bHandleFound = FALSE;
  INDEX iHandle = 1;

  for (; iHandle < _azhHandles.Count(); iHandle++) {
    // if unused
    if (!_azhHandles[iHandle].zh_bOpen) {
      // use that one
      bHandleFound = TRUE;
      break;
    }
  }

  // if no free handle found
  if (!bHandleFound) {
    // create a new one
    iHandle = _azhHandles.Count();
    _azhHandles.Push(1);
  }

  // get the handle
  CZipHandle &zh = _azhHandles[iHandle];
  ASSERT(!zh.zh_bOpen);
  zh.zh_zeEntry = *pze;

  // open zip archive for reading
  zh.zh_fFile = fopen(*pze->ze_pfnmArchive, "rb");

  // if failed to open it
  if (zh.zh_fFile == NULL) {
    // clear the handle
    zh.Clear();

    // fail
    ThrowF_t(TRANS("Cannot open '%s': %s"), (const CTString &)*pze->ze_pfnmArchive, strerror(errno));
  }

  // seek to the local header of the entry
  fseek(zh.zh_fFile, zh.zh_zeEntry.ze_slDataOffset, SEEK_SET);

  // read the sig
  int slSig;
  fread(&slSig, sizeof(slSig), 1, zh.zh_fFile);

  // if this is not the expected sig
  if (slSig != SIGNATURE_LFH) {
    // fail
    ThrowF_t(TRANS("%s/%s: Wrong signature for 'local file header'"), (CTString &)*zh.zh_zeEntry.ze_pfnmArchive,
             zh.zh_zeEntry.ze_fnm);
  }

  // read the header
  LocalFileHeader lfh;
  fread(&lfh, sizeof(lfh), 1, zh.zh_fFile);

  // determine exact compressed data position
  zh.zh_zeEntry.ze_slDataOffset = ftell(zh.zh_fFile) + lfh.lfh_swFileNameLen + lfh.lfh_swExtraFieldLen;

  // seek there
  fseek(zh.zh_fFile, zh.zh_zeEntry.ze_slDataOffset, SEEK_SET);

  // allocate buffers
  zh.zh_pubBufIn = (UBYTE *)AllocMemory(BUF_SIZE);

  // initialize zlib stream
  CTSingleLock slZip(&zip_csLock, TRUE);
  zh.zh_zstream.next_out = NULL;
  zh.zh_zstream.avail_out = 0;
  zh.zh_zstream.next_in = NULL;
  zh.zh_zstream.avail_in = 0;
  zh.zh_zstream.zalloc = (alloc_func)Z_NULL;
  zh.zh_zstream.zfree = (free_func)Z_NULL;

  int err = inflateInit2(&zh.zh_zstream, -15); // 32k windows

  // if failed
  if (err != Z_OK) {
    // clean up what is possible
    FreeMemory(zh.zh_pubBufIn);
    zh.zh_pubBufIn = NULL;

    fclose(zh.zh_fFile);
    zh.zh_fFile = NULL;

    // throw error
    zh.ThrowZLIBError_t(err, TRANS("Cannot init inflation"));
  }

  // return the handle successfully
  zh.zh_bOpen = TRUE;

  return iHandle;
}

// Get uncompressed size of a file
SLONG UNZIPGetSize(INDEX iHandle) {
  // check handle number
  if (iHandle < 0 || iHandle >= _azhHandles.Count()) {
    ASSERT(FALSE);
    return 0;
  }

  // get the handle
  CZipHandle &zh = _azhHandles[iHandle];

  // check the handle
  if (!zh.zh_bOpen) {
    ASSERT(FALSE);
    return 0;
  }

  return zh.zh_zeEntry.ze_slUncompressedSize;
}

// Get CRC of a file
ULONG UNZIPGetCRC(INDEX iHandle) {
  // check handle number
  if (iHandle < 0 || iHandle >= _azhHandles.Count()) {
    ASSERT(FALSE);
    return 0;
  }

  // get the handle
  CZipHandle &zh = _azhHandles[iHandle];

  // check the handle
  if (!zh.zh_bOpen) {
    ASSERT(FALSE);
    return 0;
  }

  return zh.zh_zeEntry.ze_ulCRC;
}

// Read a block from zip file
void UNZIPReadBlock_t(INDEX iHandle, UBYTE *pub, SLONG slStart, SLONG slLen) {
  // check handle number
  if (iHandle < 0 || iHandle >= _azhHandles.Count()) {
    ASSERT(FALSE);
    return;
  }

  // get the handle
  CZipHandle &zh = _azhHandles[iHandle];

  // check the handle
  if (!zh.zh_bOpen) {
    ASSERT(FALSE);
    return;
  }

  // if behind the end of file
  if (slStart >= zh.zh_zeEntry.ze_slUncompressedSize) {
    // do nothing
    return;
  }

  // clamp length to end of the entry data
  slLen = Min(slLen, zh.zh_zeEntry.ze_slUncompressedSize - slStart);

  // if not compressed
  if (zh.zh_zeEntry.ze_bStored) {
    // just read from file
    fseek(zh.zh_fFile, zh.zh_zeEntry.ze_slDataOffset + slStart, SEEK_SET);
    fread(pub, 1, slLen, zh.zh_fFile);

    return;
  }

  CTSingleLock slZip(&zip_csLock, TRUE);

  // if behind the current pointer
  if (slStart < zh.zh_zstream.total_out) {
    // reset the zlib stream to beginning
    inflateReset(&zh.zh_zstream);
    zh.zh_zstream.avail_in = 0;
    zh.zh_zstream.next_in = NULL;

    // seek to start of zip entry data inside archive
    fseek(zh.zh_fFile, zh.zh_zeEntry.ze_slDataOffset, SEEK_SET);
  }

  // while ahead of the current pointer
  while (slStart > zh.zh_zstream.total_out) {
    // if zlib has no more input
    while (zh.zh_zstream.avail_in == 0) {
      // read more to it
      SLONG slRead = fread(zh.zh_pubBufIn, 1, BUF_SIZE, zh.zh_fFile);

      if (slRead <= 0) {
        return; // !!!!
      }

      // tell zlib that there is more to read
      zh.zh_zstream.next_in = zh.zh_pubBufIn;
      zh.zh_zstream.avail_in = slRead;
    }

    // read dummy data from the output
    #define DUMMY_SIZE 256
    UBYTE aubDummy[DUMMY_SIZE];

    // decode to output
    zh.zh_zstream.avail_out = Min(SLONG(slStart - zh.zh_zstream.total_out), SLONG(DUMMY_SIZE));
    zh.zh_zstream.next_out = aubDummy;

    int ierr = inflate(&zh.zh_zstream, Z_SYNC_FLUSH);

    if (ierr != Z_OK && ierr != Z_STREAM_END) {
      zh.ThrowZLIBError_t(ierr, TRANS("Error seeking in zip"));
    }
  }

  // if not streaming continuously
  if (slStart != zh.zh_zstream.total_out) {
    // this should not happen
    ASSERT(FALSE);

    // read empty
    memset(pub, 0, slLen);

    return;
  }

  // set zlib for writing to the block
  zh.zh_zstream.avail_out = slLen;
  zh.zh_zstream.next_out = pub;

  // while there is something to write to given block
  while (zh.zh_zstream.avail_out > 0) {
    // if zlib has no more input
    while (zh.zh_zstream.avail_in == 0) {
      // read more to it
      SLONG slRead = fread(zh.zh_pubBufIn, 1, BUF_SIZE, zh.zh_fFile);

      if (slRead <= 0) {
        return; // !!!!
      }

      // tell zlib that there is more to read
      zh.zh_zstream.next_in = zh.zh_pubBufIn;
      zh.zh_zstream.avail_in = slRead;
    }

    // decode to output
    int ierr = inflate(&zh.zh_zstream, Z_SYNC_FLUSH);

    if (ierr != Z_OK && ierr != Z_STREAM_END) {
      zh.ThrowZLIBError_t(ierr, TRANS("Error reading from zip"));
    }
  }
}

// Close a zip file entry
void UNZIPClose(INDEX iHandle) {
  // check handle number
  if (iHandle < 0 || iHandle >= _azhHandles.Count()) {
    ASSERT(FALSE);
    return;
  }

  // get the handle
  CZipHandle &zh = _azhHandles[iHandle];

  // check the handle
  if (!zh.zh_bOpen) {
    ASSERT(FALSE);
    return;
  }

  // clear it
  zh.Clear();
}
