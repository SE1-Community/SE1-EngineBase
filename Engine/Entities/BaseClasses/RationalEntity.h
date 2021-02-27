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

#ifndef SE_INCL_RATIONALENTITY_H
#define SE_INCL_RATIONALENTITY_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Entities/BaseClasses/LiveEntity.h>

// Flag for entities that are not waiting for thinking
#define THINKTIME_NEVER (-100.0f) //(-1.f)
// [Cecil] New timer: Think time in ticks
#define THINKTICK_NEVER (-100) //(-9223372036854775807)

// Entity that can percept things and make decisions (one that has its own AI)
class ENGINE_API CRationalEntity : public CLiveEntity {
  public:
    CListNode en_lnInTimers; // node in list of waiting timers - sorted by wait time

  public:
    TICK en_llTimer; // moment in time this entity waits for timer

    CStaticStackArray<SLONG> en_stslStateStack; // stack of states for entity AI

    // Calculate physics for moving
    virtual void ClearMovingTemp(void);
    virtual void PreMoving(void);
    virtual void DoMoving(void);
    virtual void PostMoving(void);

    // Create a checksum value for sync-check
    virtual void ChecksumForSync(ULONG &ulCRC, INDEX iExtensiveSyncCheck);

    // Dump sync data to text file
    virtual void DumpSync_t(CTStream &strm, INDEX iExtensiveSyncCheck); // throw char *

    // Copy entity from another entity of same class
    virtual void Copy(CEntity &enOther, ULONG ulFlags);

    // Read from stream
    virtual void Read_t(CTStream *istr); // throw char *

    // Write to stream
    virtual void Write_t(CTStream *ostr); // throw char *

    // Unwind stack to a given state
    void UnwindStack(SLONG slThisState);

  public:
    // Jump to a new state
    void Jump(SLONG slThisState, SLONG slTargetState, BOOL bOverride, const CEntityEvent &eeInput);

    // Call a subautomaton
    void Call(SLONG slThisState, SLONG slTargetState, BOOL bOverride, const CEntityEvent &eeInput);

    // Return from a subautomaton
    void Return(SLONG slThisState, const CEntityEvent &eeReturn);

    // Print stack to debug output
    const char *PrintStackDebug(void);

    // Set next timer event to occur at given moment time
    void TimerAt(TICK llAbsolute);

    // Set next timer event to occur after given time has elapsed
    void TimerAfter(TICK llDelta);
    void SetTimerAfter(TIME tmDelta);

    // Cancel eventual pending timer
    void UnsetTimer(void);

    // Called after creating and setting its properties
    virtual void OnInitialize(const CEntityEvent &eeInput);

    // Called before releasing entity
    virtual void OnEnd(void);

  public:
    // Constructor
    CRationalEntity(void);

    // Handle an event - return false if event was not handled
    virtual BOOL HandleEvent(const CEntityEvent &ee);

    // Return bytes of memory used by this object
    inline SLONG GetUsedMemory(void) {
      SLONG slUsedMemory = sizeof(CRationalEntity) - sizeof(CLiveEntity) + CLiveEntity::GetUsedMemory();
      slUsedMemory += en_stslStateStack.sa_Count * sizeof(SLONG);

      return slUsedMemory;
    };
};

extern "C" ENGINE_API class CDLLEntityClass CRationalEntity_DLLClass;

#endif /* include-once check. */
