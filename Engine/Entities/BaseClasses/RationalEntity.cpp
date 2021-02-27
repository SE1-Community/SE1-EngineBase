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

#include "StdH.h"

#include <Engine/Entities/BaseClasses/RationalEntity.h>
#include <Engine/Entities/EntityClass.h>

#include <Engine/Base/CRC.h>
#include <Engine/Base/Stream.h>
#include <Engine/World/World.h>

// Allocation step for state stack of a CRationalEntity
#define STATESTACK_ALLOCATIONSTEP 5

// Constructor
CRationalEntity::CRationalEntity(void) {}

// Calculate physics for moving
void CRationalEntity::ClearMovingTemp(void) {}

void CRationalEntity::PreMoving(void) {}
void CRationalEntity::DoMoving(void) {}
void CRationalEntity::PostMoving(void) {}

// Create a checksum value for sync-check
void CRationalEntity::ChecksumForSync(ULONG &ulCRC, INDEX iExtensiveSyncCheck) {
  CEntity::ChecksumForSync(ulCRC, iExtensiveSyncCheck);

  if (iExtensiveSyncCheck > 0) {
    CRC_AddLONG(ulCRC, en_llTimer); //CRC_AddFLOAT(ulCRC, en_timeTimer);
    CRC_AddLONG(ulCRC, en_stslStateStack.Count());
  }
}

// Dump sync data to text file
void CRationalEntity::DumpSync_t(CTStream &strm, INDEX iExtensiveSyncCheck) {
  CEntity::DumpSync_t(strm, iExtensiveSyncCheck);

  if (iExtensiveSyncCheck > 0) {
    strm.FPrintF_t("en_llTimer:  %g(%08x)\n", CTimer::InSeconds(en_llTimer), (ULONG &)en_llTimer);
    strm.FPrintF_t("en_stslStateStack.Count(): %d\n", en_stslStateStack.Count());
  }

  strm.FPrintF_t("en_fHealth:    %g(%08x)\n", en_fHealth, (ULONG &)en_fHealth);
}

// Copy entity from another entity of same class
void CRationalEntity::Copy(CEntity &enOther, ULONG ulFlags) {
  CLiveEntity::Copy(enOther, ulFlags);

  if (!(ulFlags & COPY_REINIT)) {
    CRationalEntity *prenOther = (CRationalEntity *)(&enOther);
    en_llTimer = prenOther->en_llTimer;
    en_stslStateStack = prenOther->en_stslStateStack;

    if (prenOther->en_lnInTimers.IsLinked()) {
      en_pwoWorld->AddTimer(this);
    }
  }
}

// Read from stream
void CRationalEntity::Read_t(CTStream *istr) {
  CLiveEntity::Read_t(istr);

  // [Cecil] New timer: 'Tick TiMer v1'
  if (istr->PeekID_t() != CChunkID("TTM1")) {
    FLOAT fTimer;
    (*istr) >> fTimer;
    en_llTimer = CTimer::InTicks(fTimer);

  } else {
    istr->ExpectID_t(CChunkID("TTM1"));
    (*istr) >> en_llTimer;
  }

  // if waiting for thinking
  if (en_llTimer != THINKTICK_NEVER) {
    // add to list of thinkers
    en_pwoWorld->AddTimer(this);
  }

  // read the state stack
  en_stslStateStack.Clear();
  en_stslStateStack.SetAllocationStep(STATESTACK_ALLOCATIONSTEP);

  INDEX ctStates;
  (*istr) >> ctStates;

  for (INDEX iState = 0; iState < ctStates; iState++) {
    (*istr) >> en_stslStateStack.Push();
  }
}

// Write to stream
void CRationalEntity::Write_t(CTStream *ostr) {
  CLiveEntity::Write_t(ostr);

  // if not currently waiting for thinking
  if (!en_lnInTimers.IsLinked()) {
    // set dummy thinking time as a flag for later loading
    en_llTimer = THINKTICK_NEVER;
  }

  // [Cecil] New timer: 'Tick TiMer v1'
  ostr->WriteID_t(CChunkID("TTM1"));
  (*ostr) << en_llTimer;

  // write the state stack
  (*ostr) << en_stslStateStack.Count();

  for (INDEX iState = 0; iState < en_stslStateStack.Count(); iState++) {
    (*ostr) << en_stslStateStack[iState];
  }
}

// Set next timer event to occur at given moment time
void CRationalEntity::TimerAt(TICK llAbsolute) {
  // must never set think back in time, except for special 'never' time
  ASSERTMSG(llAbsolute > _pTimer->GetGameTick() || llAbsolute == THINKTICK_NEVER, "Do not SetThink() back in time!");

  // set the timer
  en_llTimer = llAbsolute;

  // add to world's list of timers if neccessary
  if (en_llTimer != THINKTICK_NEVER) {
    en_pwoWorld->AddTimer(this);

  } else {
    if (en_lnInTimers.IsLinked()) {
      en_lnInTimers.Remove();
    }
  }
}

// Set next timer event to occur after given time has elapsed
void CRationalEntity::TimerAfter(TICK llDelta) {
  // set the execution for the moment that is that much ahead of the current tick
  TimerAt(_pTimer->GetGameTick() + llDelta);
}

void CRationalEntity::SetTimerAfter(TIME tmDelta) {
  // set the execution for the moment that is that much ahead of the current tick
  TimerAt(_pTimer->GetGameTick() + CTimer::InTicks(tmDelta));
}

// Cancel eventual pending timer
void CRationalEntity::UnsetTimer(void) {
  en_llTimer = THINKTICK_NEVER;

  if (en_lnInTimers.IsLinked()) {
    en_lnInTimers.Remove();
  }
}

// Unwind stack to a given state
void CRationalEntity::UnwindStack(SLONG slThisState) {
  // for each state on the stack (from top to bottom)
  for (INDEX iStateInStack = en_stslStateStack.Count() - 1; iStateInStack >= 0; iStateInStack--) {
    // if it is the state
    if (en_stslStateStack[iStateInStack] == slThisState) {
      // unwind to it
      en_stslStateStack.PopUntil(iStateInStack);
      return;
    }
  }

  // the state must be on the stack
  ASSERTALWAYS("Unwinding to unexisting state!");
}

// Jump to a new state
void CRationalEntity::Jump(SLONG slThisState, SLONG slTargetState, BOOL bOverride, const CEntityEvent &eeInput) {
  // unwind the stack to this state
  UnwindStack(slThisState);

  // set the new topmost state
  if (bOverride) {
    slTargetState = en_pecClass->ec_pdecDLLClass->GetOverridenState(slTargetState);
  }

  en_stslStateStack[en_stslStateStack.Count() - 1] = slTargetState;

  // handle the given event in the new state
  HandleEvent(eeInput);
};

// Call a subautomaton
void CRationalEntity::Call(SLONG slThisState, SLONG slTargetState, BOOL bOverride, const CEntityEvent &eeInput) {
  // unwind the stack to this state
  UnwindStack(slThisState);

  // push the new state to stack
  if (bOverride) {
    slTargetState = en_pecClass->ec_pdecDLLClass->GetOverridenState(slTargetState);
  }

  en_stslStateStack.Push() = slTargetState;

  // handle the given event in the new state
  HandleEvent(eeInput);
};

// Return from a subautomaton
void CRationalEntity::Return(SLONG slThisState, const CEntityEvent &eeReturn) {
  // unwind the stack to this state
  UnwindStack(slThisState);

  // pop one state from the stack
  en_stslStateStack.PopUntil(en_stslStateStack.Count() - 2);

  // handle the given event in the new topmost state
  HandleEvent(eeReturn);
};

// Print stack to debug output
const char *CRationalEntity::PrintStackDebug(void) {
  _RPT2(_CRT_WARN, "-- stack of '%s'@%gs\n", GetName(), CTimer::InSeconds(_pTimer->GetGameTick()));

  INDEX ctStates = en_stslStateStack.Count();

  for (INDEX iState = ctStates - 1; iState >= 0; iState--) {
    SLONG slState = en_stslStateStack[iState];
    _RPT2(_CRT_WARN, "0x%08x %s\n", slState, en_pecClass->ec_pdecDLLClass->HandlerNameForState(slState));
  }

  _RPT0(_CRT_WARN, "----\n");

  return "ok";
}

// Handle an event - return false if event was not handled
BOOL CRationalEntity::HandleEvent(const CEntityEvent &ee) {
  // for each state on the stack (from top to bottom)
  for (INDEX iStateInStack = en_stslStateStack.Count() - 1; iStateInStack >= 0; iStateInStack--) {
    // try to find a handler in that state
    pEventHandler pehHandler = HandlerForStateAndEvent(en_stslStateStack[iStateInStack], ee.ee_slEvent);

    // if there is a handler
    if (pehHandler != NULL) {
      // call the function
      BOOL bHandled = (this->*pehHandler)(ee);

      // if the event was successfully handled
      if (bHandled) {
        // return that it was handled
        return TRUE;
      }
    }
  }

  // if no transition was found, the event was not handled
  return FALSE;
}

// Called after creating and setting its properties
void CRationalEntity::OnInitialize(const CEntityEvent &eeInput) {
  // make sure entity doesn't destroy itself during intialization
  CEntityPointer penThis = this;

  // do not think
  en_llTimer = THINKTICK_NEVER;

  if (en_lnInTimers.IsLinked()) {
    en_lnInTimers.Remove();
  }

  // initialize state stack
  en_stslStateStack.Clear();
  en_stslStateStack.SetAllocationStep(STATESTACK_ALLOCATIONSTEP);

  en_stslStateStack.Push() = 1; // start state is always state with number 1

  // call the main function of the entity
  HandleEvent(eeInput);
}

// Called before releasing entity
void CRationalEntity::OnEnd(void) {
  // cancel eventual pending timer
  UnsetTimer();
}
