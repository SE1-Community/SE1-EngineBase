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

#ifndef SE_INCL_NAMETABLE_TEMPLATE_H
#define SE_INCL_NAMETABLE_TEMPLATE_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Templates/StaticArray.cpp>

// [Cecil] 2021-06-03: Reworked into actual templates

template<class Type> class CNameTableSlot {
  public:
    ULONG nts_ulKey; // hashing key
    Type *nts_ptElement; // the element inhere

    CNameTableSlot(void) {
      nts_ptElement = NULL;
    };

    void Clear(void) {
      nts_ptElement = NULL;
    };
};

// Template class for storing pointers to objects for fast access by name
template<class Type> class CNameTable {
  public:
    INDEX nt_ctCompartments; // number of compartments in table
    INDEX nt_ctSlotsPerComp; // number of slots in one compartment
    INDEX nt_ctSlotsPerCompStep; // allocation step for number of slots in one compartment
    CStaticArray<CNameTableSlot<Type>> nt_antsSlots; // all slots are here

    // Internal finding
    CNameTableSlot<Type> *FindSlot(ULONG ulKey, const CTString &strName);

    // Expand the name table to next step
    void Expand(void);

  public:
    // Constructor
    CNameTable(void);

    // Destructor - frees all memory
    ~CNameTable(void);

    // Remove all slots and reset the nametable to initial (empty) state
    void Clear(void);

    // Set allocation parameters
    void SetAllocationParameters(INDEX ctCompartments, INDEX ctSlotsPerComp, INDEX ctSlotsPerCompStep);

    // Find an object by name
    Type *Find(const CTString &strName);

    // Add a new object
    void Add(Type *ptNew);

    // Remove an object
    void Remove(Type *ptOld);

    // Remove all objects but keep slots
    void Reset(void);

    // Get estimated efficiency of the nametable
    CTString GetEfficiency(void);
};

#include <Engine/Templates/NameTable.inl>

#endif /* include-once check. */
