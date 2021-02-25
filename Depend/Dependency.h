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

#ifndef SE_INCL_DEPENDENCY_H
#define SE_INCL_DEPENDENCY_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

// Adjust file path automatically for application path prefix
void AdjustFilePath_t(CTFileName &fnm);

class CDependInfo {
  public:
    // Attributes
    CListNode di_Node;
    CTFileName di_fnFileName;
    time_t di_tTime;
    CTFileName di_fnParent;

    // Default constructor
    CDependInfo(CTFileName fnFileName, CTFileName fnParent);

    // Check if the file is updated
    BOOL IsFileOnDiskUpdated(void);

    // Check if time of the file is same
    inline BOOL IsUpdated(const CDependInfo &diOther) {
      return (diOther.di_tTime == di_tTime);
    };

    // Check if the file is older
    inline BOOL IsOlder(const CDependInfo &diOther) {
      return (di_tTime < diOther.di_tTime);
    };

    // Comparison
    inline BOOL operator==(const CDependInfo &diOther) const {
      return (diOther.di_fnFileName == di_fnFileName);
    };

    // Write info
    inline void Write_t(CTStream *ostrFile) const {
      *ostrFile << di_fnFileName;
      ostrFile->Write_t(&di_tTime, sizeof(time_t));
    };

    // Read info
    inline void Read_t(CTStream *istrFile) {
      *istrFile >> di_fnFileName;
      istrFile->Read_t(&di_tTime, sizeof(time_t));
    };
};

class CDependencyList {
  public:
    CListHead dl_ListHead;

    // Extract all dependencies from the list
    void ExtractDependencies();

    // Remove updated files from the list
    void RemoveUpdatedFiles();

    // Create list from an ASCII file
    void ImportASCII(CTFileName fnAsciiFile);

    // Export list members into an ASCII file in form sutable for archivers
    void ExportASCII_t(CTFileName fnAsciiFile);

    // Substract other list from this one
    void Substract(CDependencyList &dlToSubstract);

    // Extract translation strings from all files in list
    void ExtractTranslations_t(const CTFileName &fnTranslations);

    // Clear dependency list
    void Clear(void);

    // Check if given file already has its own DependInfo object linked in list
    BOOL ExistsInList(CListHead &lh, CTFileName fnTestName) const;

    // Write and read
    void Read_t(CTStream *istrFile); // throw char *
    void Write_t(CTStream *ostrFile); // throw char *
};

#endif /* include-once check. */
