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

#ifdef NAMETABLE_CASESENSITIVE
  #define COMPARENAMES(a, b) (strcmp(a, b) == 0)

#else
  #define COMPARENAMES(a, b) (a == b)
#endif

// Constructor
template<class Type> CNameTable<Type>::CNameTable(void) {
  nt_ctCompartments = 0;
  nt_ctSlotsPerComp = 0;
  nt_ctSlotsPerCompStep = 0;
}

// Destructor - frees all memory
template<class Type> CNameTable<Type>::~CNameTable(void) {}

// Remove all slots, and reset the nametable to initial (empty) state
template<class Type> void CNameTable<Type>::Clear(void) {
  nt_ctCompartments = 0;
  nt_ctSlotsPerComp = 0;
  nt_ctSlotsPerCompStep = 0;
  nt_antsSlots.Clear();
}

// Internal finding
template<class Type> CNameTableSlot<Type> *CNameTable<Type>::FindSlot(ULONG ulKey, const CTString &strName) {
  ASSERT(nt_ctCompartments > 0 && nt_ctSlotsPerComp > 0);

  // find compartment number
  INDEX iComp = ulKey % nt_ctCompartments;

  // for each slot in the compartment
  INDEX iSlot = iComp * nt_ctSlotsPerComp;

  for (INDEX iSlotInComp = 0; iSlotInComp < nt_ctSlotsPerComp; iSlotInComp++, iSlot++) {
    CNameTableSlot<Type> *pnts = &nt_antsSlots[iSlot];

    // if empty
    if (pnts->nts_ptElement == NULL) {
      // skip it
      continue;
    }

    // if it has same key
    if (pnts->nts_ulKey == ulKey) {
      // if it is same element
      if (COMPARENAMES(pnts->nts_ptElement->GetName(), strName)) {
        // return it
        return pnts;
      }
    }
  }

  // not found
  return NULL;
}

// Set allocation parameters
template<class Type> void CNameTable<Type>::SetAllocationParameters(INDEX ctCompartments, INDEX ctSlotsPerComp, INDEX ctSlotsPerCompStep) {
  ASSERT(nt_ctCompartments == 0 && nt_ctSlotsPerComp == 0 && nt_ctSlotsPerCompStep == 0);
  ASSERT(ctCompartments > 0 && ctSlotsPerComp > 0 && ctSlotsPerCompStep > 0);

  nt_ctCompartments = ctCompartments;
  nt_ctSlotsPerComp = ctSlotsPerComp;
  nt_ctSlotsPerCompStep = ctSlotsPerCompStep;

  nt_antsSlots.New(nt_ctCompartments * nt_ctSlotsPerComp);
}

// Find an object by name
template<class Type> Type *CNameTable<Type>::Find(const CTString &strName) {
  ASSERT(nt_ctCompartments > 0 && nt_ctSlotsPerComp > 0);

  CNameTableSlot<Type> *pnts = FindSlot(strName.GetHash(), strName);

  if (pnts == NULL) {
    return NULL;
  }

  return pnts->nts_ptElement;
}

// Expand the name table to next step
template<class Type> void CNameTable<Type>::Expand(void) {
  ASSERT(nt_ctCompartments > 0 && nt_ctSlotsPerComp > 0);

  // if we are here -> the compartment has overflowed
  ASSERT(nt_ctSlotsPerCompStep > 0);

  // move the array of slots
  CStaticArray<CNameTableSlot<Type>> antsSlotsOld;
  antsSlotsOld.MoveArray(nt_antsSlots);

  // allocate new bigger array
  INDEX ctOldSlotsPerComp = nt_ctSlotsPerComp;
  nt_ctSlotsPerComp += nt_ctSlotsPerCompStep;
  nt_antsSlots.New(nt_ctSlotsPerComp * nt_ctCompartments);

  // for each compartment
  for (INDEX iComp = 0; iComp < nt_ctCompartments; iComp++) {
    // for each old slot in compartment
    for (INDEX iSlotInComp = 0; iSlotInComp < ctOldSlotsPerComp; iSlotInComp++) {
      CNameTableSlot<Type> &ntsOld = antsSlotsOld[iSlotInComp + iComp * ctOldSlotsPerComp];
      CNameTableSlot<Type> &ntsNew = nt_antsSlots[iSlotInComp + iComp * nt_ctSlotsPerComp];

      // if it is used
      if (ntsOld.nts_ptElement != NULL) {
        // copy it to new array
        ntsNew.nts_ptElement = ntsOld.nts_ptElement;
        ntsNew.nts_ulKey = ntsOld.nts_ulKey;
      }
    }
  }
}

static BOOL _bExpanding = FALSE; // check to prevend recursive expanding

// Add a new object
template<class Type> void CNameTable<Type>::Add(Type *ptNew) {
  ASSERT(nt_ctCompartments > 0 && nt_ctSlotsPerComp > 0);

  ULONG ulKey = ptNew->GetName().GetHash();

  // find compartment number
  INDEX iComp = ulKey % nt_ctCompartments;

  // for each slot in the compartment
  INDEX iSlot = iComp * nt_ctSlotsPerComp;

  for (INDEX iSlotInComp = 0; iSlotInComp < nt_ctSlotsPerComp; iSlotInComp++, iSlot++) {
    CNameTableSlot<Type> *pnts = &nt_antsSlots[iSlot];

    // if it is empty
    if (pnts->nts_ptElement == NULL) {
      // put it here
      pnts->nts_ulKey = ulKey;
      pnts->nts_ptElement = ptNew;
      return;
    }

    // must not already exist
    //ASSERT(pnts->nts_ptElement->GetName() != ptNew->GetName());
  }

  // if we are here -> the compartment has overflowed

  // expand the name table to next step
  ASSERT(!_bExpanding);

  _bExpanding = TRUE;
  Expand();

  // add the new element
  Add(ptNew);
  _bExpanding = FALSE;
}

// Remove an object
template<class Type> void CNameTable<Type>::Remove(Type *ptOld) {
  ASSERT(nt_ctCompartments > 0 && nt_ctSlotsPerComp > 0);

  // find its slot
  const CTString &strName = ptOld->GetName();
  CNameTableSlot<Type> *pnts = FindSlot(strName.GetHash(), strName);

  if (pnts != NULL) {
    // mark slot as unused
    ASSERT(pnts->nts_ptElement == ptOld);

    pnts->nts_ptElement = NULL;
  }
}

// Remove all objects but keep slots
template<class Type> void CNameTable<Type>::Reset(void) {
  for (INDEX iSlot = 0; iSlot < nt_antsSlots.Count(); iSlot++) {
    nt_antsSlots[iSlot].Clear();
  }
}

#undef NAMETABLE_CASESENSITIVE
