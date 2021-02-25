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

#ifndef SE_INCL_LINEARALLOCATOR_H
#define SE_INCL_LINEARALLOCATOR_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

// Template class for dynamic allocation of objects in a linear fashion
template<class Type> class CLinearAllocator {
  public:
    CListHead la_lhBlocks; // list of allocated memory blocks
    INDEX la_ctAllocationStep; // number of objects allocated
    INDEX la_ctObjects;        // number of objects allocated
    INDEX la_ctFree;           // number of free objects in current block
    Type *la_ptNextFree; // pointer to next free object

    // Allocate a new memory block
    void AllocBlock(INDEX iCount);

  public:
    // Constructor
    CLinearAllocator(void);

    // Copy constructor
    CLinearAllocator(CLinearAllocator<Type> &laOriginal);

    // Destructor - frees all memory
    ~CLinearAllocator(void);

    // Destroy all objects and reset the allocator to initial (empty) state
    void Clear(void);

    // Set how many elements to allocate when stack overflows
    inline void SetAllocationStep(INDEX ctStep);

    // Allocate a new object
    inline Type &New(void);
    inline Type *New(INDEX ct);

    // Free all objects but keep allocated space and relinearize it
    inline void Reset(void);

    // These are just for compatibility with CDynamicStackArray<>
    inline Type &Push(void) {
      return New();
    };

    inline Type *Push(INDEX ct) {
      return New(ct);
    };

    inline void PopAll(void) {
      Reset();
    };
};

#endif /* include-once check. */
