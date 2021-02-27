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

#ifndef SE_INCL_UNZIP_H
#define SE_INCL_UNZIP_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/zlib/zlib.h>

#pragma pack(1)

// Before each file in the zip
#define SIGNATURE_LFH 0x04034b50
struct LocalFileHeader {
  SWORD lfh_swVersionToExtract;
  SWORD lfh_swGPBFlag;
  SWORD lfh_swCompressionMethod;
  SWORD lfh_swModFileTime;
  SWORD lfh_swModFileDate;
  SLONG lfh_slCRC32;
  SLONG lfh_slCompressedSize;
  SLONG lfh_slUncompressedSize;
  SWORD lfh_swFileNameLen;
  SWORD lfh_swExtraFieldLen;

  // Follows:
  // filename (variable size)
  // extra field (variable size)
};

// After file data, only if compressed from a non-seekable stream
// this exists only if bit 3 in GPB flag is set
#define SIGNATURE_DD 0x08074b50
struct DataDescriptor {
  SLONG dd_slCRC32;
  SLONG dd_slCompressedSize;
  SLONG dd_slUncompressedSize;
};

// One file in central dir
#define SIGNATURE_FH 0x02014b50
struct FileHeader {
  SWORD fh_swVersionMadeBy;
  SWORD fh_swVersionToExtract;
  SWORD fh_swGPBFlag;
  SWORD fh_swCompressionMethod;
  SWORD fh_swModFileTime;
  SWORD fh_swModFileDate;
  SLONG fh_slCRC32;
  SLONG fh_slCompressedSize;
  SLONG fh_slUncompressedSize;
  SWORD fh_swFileNameLen;
  SWORD fh_swExtraFieldLen;
  SWORD fh_swFileCommentLen;
  SWORD fh_swDiskNoStart;
  SWORD fh_swInternalFileAttributes;
  SLONG fh_swExternalFileAttributes;
  SLONG fh_slLocalHeaderOffset;

  // Follows:
  // filename (variable size)
  // extra field (variable size)
  // file comment (variable size)
};

// At the end of entire zip file
#define SIGNATURE_EOD 0x06054b50
struct EndOfDir {
  SWORD eod_swDiskNo;
  SWORD eod_swDirStartDiskNo;
  SWORD eod_swEntriesInDirOnThisDisk;
  SWORD eod_swEntriesInDir;
  SLONG eod_slSizeOfDir;
  SLONG eod_slDirOffsetInFile;
  SWORD eod_swCommentLenght;

  // Follows:
  // zipfile comment (variable size)
};

#pragma pack()

// One entry (a zipped file) in a zip archive
class CZipEntry {
  public:
    CTFileName *ze_pfnmArchive;  // path of the archive
    CTFileName ze_fnm;           // file name with path inside archive
    SLONG ze_slCompressedSize;   // size of file in the archive
    SLONG ze_slUncompressedSize; // size when uncompressed
    SLONG ze_slDataOffset;       // position of compressed data inside archive
    ULONG ze_ulCRC;              // checksum of the file
    BOOL ze_bStored;             // set if file is not compressed, but stored
    BOOL ze_bMod;                // set if from a mod's archive

    void Clear(void) {
      ze_pfnmArchive = NULL;
      ze_fnm.Clear();
    }
};

// An open instance of a file inside a zip
class CZipHandle {
  public:
    BOOL zh_bOpen; // set if the handle is used
    CZipEntry zh_zeEntry; // the entry itself
    z_stream zh_zstream; // zlib filestream for decompression
    FILE *zh_fFile; // open handle of the archive

    #define BUF_SIZE 1024
    UBYTE *zh_pubBufIn; // input buffer

    // Constructor
    CZipHandle(void);

    void Clear(void);
    void ThrowZLIBError_t(int ierr, const CTString &strDescription);
};

// Add one zip archive to current active set
void UNZIPAddArchive(const CTFileName &fnm);

// Read directories of all currently added archives, in reverse alphabetical order
void UNZIPReadDirectoriesReverse_t(void);

// Check if a zip file entry exists
BOOL UNZIPFileExists(const CTFileName &fnm);

// Open a zip file entry for reading
INDEX UNZIPOpen_t(const CTFileName &fnm);

// Get uncompressed size of a file
SLONG UNZIPGetSize(INDEX iHandle);

// Get CRC of a file
ULONG UNZIPGetCRC(INDEX iHandle);

// Read a block from zip file
void UNZIPReadBlock_t(INDEX iHandle, UBYTE *pub, SLONG slStart, SLONG slLen);

// Close a zip file entry
void UNZIPClose(INDEX iHandle);

// Get info on a zip file entry
void UNZIPGetFileInfo(INDEX iHandle, CTFileName &fnmZip, SLONG &slOffset,
                      SLONG &slSizeCompressed, SLONG &slSizeUncompressed, BOOL &bCompressed);

// Enumeration for all files in all zips
INDEX UNZIPGetFileCount(void);
const CTFileName &UNZIPGetFileAtIndex(INDEX i);

// Get index of a file (-1 for no file)
INDEX UNZIPGetFileIndex(const CTFileName &fnm);

// Check if a file is from a mod's zip
BOOL UNZIPIsFileAtIndexMod(INDEX i);

#endif /* include-once check. */
