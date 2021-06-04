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

#include "Stdh.h"

#include <Engine/Anim/AnimObject.h>

#include <Engine/Math/Functions.h>
#include <Engine/Base/Stream.h>
#include <Engine/Base/Timer.h>

#include <Engine/Anim/AnimData.h>

// Constructor
CAnimObject::CAnimObject(void) {
  // set invalid data for validation check
  ao_AnimData = NULL;
  
  ao_llAnimStart = 0;
  ao_iCurrentAnim = -1;
  ao_iLastAnim = -1;
  ao_ulFlags = AOF_PAUSED;
};

// Destructor
CAnimObject::~CAnimObject(void) {
  ao_AnimData->RemReference();
};

// Copy from another object of same class
ENGINE_API void CAnimObject::Copy(CAnimObject &aoOther) {
  SetData(aoOther.GetData());
  
  ao_llAnimStart = aoOther.ao_llAnimStart;
  ao_iCurrentAnim = aoOther.ao_iCurrentAnim;
  ao_iLastAnim = aoOther.ao_iLastAnim;
  ao_ulFlags = aoOther.ao_ulFlags;
}

// Synchronize with another animation object (set same anim and frames)
ENGINE_API void CAnimObject::Synchronize(CAnimObject &aoOther) {
  // copy animations, time and flags
  INDEX ctAnims = GetAnimsCt();
  
  ao_llAnimStart = aoOther.ao_llAnimStart;
  ao_iCurrentAnim = ClampUp(aoOther.ao_iCurrentAnim, ctAnims - 1L);
  ao_iLastAnim = ClampUp(aoOther.ao_iLastAnim, ctAnims - 1L);
  ao_ulFlags = aoOther.ao_ulFlags;
}

// Get animation's length
FLOAT CAnimObject::GetAnimLength(INDEX iAnim) const {
  if (ao_AnimData == NULL) {
    return 1.0f;
  }
  
  ASSERT(ao_AnimData != NULL);
  
  if (iAnim >= ao_AnimData->ad_NumberOfAnims) {
    iAnim = 0;
  }
  
  ASSERT((iAnim >= 0) && (iAnim < ao_AnimData->ad_NumberOfAnims));
  
  COneAnim *pCOA = &ao_AnimData->ad_Anims[iAnim];
  return pCOA->oa_NumberOfFrames * pCOA->oa_SecsPerFrame;
};

FLOAT CAnimObject::GetCurrentAnimLength(void) const {
  return GetAnimLength(ao_iCurrentAnim);
}

// Calculate frame that coresponds to given time
INDEX CAnimObject::FrameInTime(TIME time) const {
  ASSERT(ao_AnimData != NULL);
  ASSERT((ao_iCurrentAnim >= 0) && (ao_iCurrentAnim < ao_AnimData->ad_NumberOfAnims));
  
  INDEX iFrameInAnim;

  // [Cecil] New timer: Anim time in seconds
  TIME tmAnim = CTimer::InSeconds(ao_llAnimStart);

  COneAnim *pCOA = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
  if (ao_ulFlags & AOF_PAUSED) {
    // return index of paused frame inside global frame array
    iFrameInAnim = ClipFrame(pCOA->oa_NumberOfFrames + ClipFrame(FloatToInt(tmAnim / pCOA->oa_SecsPerFrame)));
    
  } else {
    // return index of frame inside global frame array of frames in given moment
    iFrameInAnim = ClipFrame(FloatToInt((time - tmAnim) / pCOA->oa_SecsPerFrame));
  }
  
  return pCOA->oa_FrameIndices[iFrameInAnim];
}

// Pause the animation
void CAnimObject::PauseAnim(void) {
  // dont pause twice
  if (ao_ulFlags & AOF_PAUSED) {
    return;
  }
  
  ao_ulFlags |= AOF_PAUSED;
  
  // set difference from current time as start time, so get frame will get correct current frame
  ao_llAnimStart = _pTimer->GetGameTick() - ao_llAnimStart;
  
  MarkChanged();
}

// Continue paused animation
void CAnimObject::ContinueAnim(void) {
  if (!(ao_ulFlags & AOF_PAUSED)) {
    return;
  }
  
  // calculate freezed frame index inside current animation (not in global list of frames!)
  COneAnim *pCOA = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
  
  if (pCOA->oa_NumberOfFrames <= 0) {
    return;
  }
  
  INDEX iStoppedFrame = (pCOA->oa_NumberOfFrames + (SLONG)(CTimer::InSeconds(ao_llAnimStart) / pCOA->oa_SecsPerFrame)
                      % pCOA->oa_NumberOfFrames) % pCOA->oa_NumberOfFrames;
  
  // using current frame index calculate time so animation continues from same frame
  ao_llAnimStart = _pTimer->GetGameTick() - CTimer::InTicks(pCOA->oa_SecsPerFrame) * iStoppedFrame;
  ao_ulFlags &= ~AOF_PAUSED;
  
  MarkChanged();
}

// Offset the animation phase
void CAnimObject::OffsetPhase(TIME tm) {
  ao_llAnimStart += CTimer::InTicks(tm);
}

// Loop anims forward
void CAnimObject::NextAnim() {
  ASSERT(ao_iCurrentAnim != -1);
  ASSERT(ao_AnimData != NULL);
  
  ao_iCurrentAnim = (ao_iCurrentAnim + 1) % ao_AnimData->ad_NumberOfAnims;
  ao_iLastAnim = ao_iCurrentAnim;
  ao_llAnimStart = _pTimer->GetGameTick();
  
  MarkChanged();
};

// Loop anims backward
void CAnimObject::PrevAnim() {
  ASSERT(ao_iCurrentAnim != -1);
  ASSERT(ao_AnimData != NULL);
  
  ao_iCurrentAnim = (ao_AnimData->ad_NumberOfAnims + ao_iCurrentAnim - 1) % ao_AnimData->ad_NumberOfAnims;
  ao_iLastAnim = ao_iCurrentAnim;
  ao_llAnimStart = _pTimer->GetGameTick();
  
  MarkChanged();
};

// Selects frame for given time offset from animation start (0)
void CAnimObject::SelectFrameInTime(TIME tmOffset) {
  ao_llAnimStart = CTimer::InTicks(tmOffset); // set fixed start time
  MarkChanged();
}

void CAnimObject::FirstFrame(void) {
  SelectFrameInTime(0.0f);
}

void CAnimObject::LastFrame(void) {
  class COneAnim *pCOA = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
  SelectFrameInTime(GetAnimLength(ao_iCurrentAnim) - pCOA->oa_SecsPerFrame);
}

// Loop frames forward
void CAnimObject::NextFrame() {
  ASSERT(ao_iCurrentAnim != -1);
  ASSERT(ao_AnimData != NULL);
  ASSERT(ao_ulFlags & AOF_PAUSED);
  
  ao_llAnimStart += CTimer::InTicks(ao_AnimData->ad_Anims[ao_iCurrentAnim].oa_SecsPerFrame);
  
  MarkChanged();
};

// Loop frames backward
void CAnimObject::PrevFrame() {
  ASSERT(ao_iCurrentAnim != -1);
  ASSERT(ao_AnimData != NULL);
  ASSERT(ao_ulFlags & AOF_PAUSED);
  
  ao_llAnimStart -= CTimer::InTicks(ao_AnimData->ad_Anims[ao_iCurrentAnim].oa_SecsPerFrame);
  
  MarkChanged();
};

// Retrieves paused flag
BOOL CAnimObject::IsPaused() {
  return ao_ulFlags & AOF_PAUSED;
};

// Test if some updateable object is up to date with this anim object
BOOL CAnimObject::IsUpToDate(const CUpdateable &ud) const {
  // if the object itself has changed, or its data has changed
  if (!CChangeable::IsUpToDate(ud) || !ao_AnimData->IsUpToDate(ud)) {
    // something has changed
    return FALSE;
  }
  
  // otherwise, nothing has changed
  return TRUE;
}

// Attach data to this object
void CAnimObject::SetData(CAnimData *pAD) {
  // mark new data as referenced once more
  pAD->AddReference();
  
  // mark old data as referenced once less
  ao_AnimData->RemReference();
  
  // remember new data
  ao_AnimData = pAD;
  
  if (pAD != NULL) {
    StartAnim(0);
  }
  
  // mark that something has changed
  MarkChanged();
}

// Obtain anim and set it for this object
void CAnimObject::SetData_t(const CTFileName &fnmAnim) {
  // if the filename is empty
  if (fnmAnim == "") {
    // release current anim
    SetData(NULL);

  // if the filename is not empty
  } else {
    // obtain it (adds one reference)
    CAnimData *pad = _pAnimStock->Obtain_t(fnmAnim);
    
    // set it as data (adds one more reference, and remove old reference)
    SetData(pad);
    
    // release it (removes one reference)
    _pAnimStock->Release(pad); // total reference count +1+1-1 = +1 for new data -1 for old data
  }
}

// Set new animation (but don't start it)
void CAnimObject::SetAnim(INDEX iNew) {
  if (ao_AnimData == NULL) {
    return;
  }
  
  // clamp animation
  if (iNew >= GetAnimsCt()) {
    iNew = 0;
  }
  
  // if new animation
  if (ao_iCurrentAnim != iNew) {
    // remember starting time
    ao_llAnimStart = _pTimer->GetGameTick();
  }
  
  // set new animation number
  ao_iCurrentAnim = iNew;
  ao_iLastAnim = iNew;
  
  // mark that something has changed
  MarkChanged();
};

// Start new animation
void CAnimObject::StartAnim(INDEX iNew) {
  if (ao_AnimData == NULL) {
    return;
  }
  
  // set new animation
  SetAnim(iNew);
  
  // set pause off, looping on
  ao_ulFlags = AOF_LOOPING;
};

// Start playing an animation
void CAnimObject::PlayAnim(INDEX iNew, ULONG ulFlags) {
  if (ao_AnimData == NULL) {
    return;
  }
  
  // clamp animation
  if (iNew >= GetAnimsCt()) {
    iNew = 0;
  }

  // if anim needs to be reset at start
  if (!(ulFlags & AOF_NORESTART) || ao_iCurrentAnim != iNew) {
    // if smooth transition
    if (ulFlags & AOF_SMOOTHCHANGE) {
      // calculate time to end of the current anim
      class COneAnim *pCOA = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
      TIME tmLength = GetCurrentAnimLength();
      
      FLOAT fFrame = (CTimer::InSeconds(_pTimer->GetGameTick() - ao_llAnimStart) / pCOA->oa_SecsPerFrame);
      INDEX iFrame = INDEX(fFrame);
      FLOAT fFract = fFrame - iFrame;
      iFrame = ClipFrame(iFrame);
      
      TIME tmPassed = (iFrame + fFract) * pCOA->oa_SecsPerFrame;
      TIME tmLeft = tmLength - tmPassed;
      
      // set time ahead to end of the current animation
      ao_llAnimStart = _pTimer->GetGameTick() + CTimer::InTicks(tmLeft);
      
      // remember last animation
      ao_iLastAnim = ao_iCurrentAnim;
      
      // set new animation number
      ao_iCurrentAnim = iNew;

    // if normal transition
    } else {
      ao_iLastAnim = iNew;
      ao_iCurrentAnim = iNew;
      // remember starting time
      ao_llAnimStart = _pTimer->GetGameTick();
    }
  }
  
  // set pause off, looping flag from flags
  ao_ulFlags = ulFlags & (AOF_LOOPING | AOF_PAUSED);

  // mark that something has changed
  MarkChanged();
};

// Seamlessly continue playing another animation from same point
void CAnimObject::SwitchToAnim(INDEX iNew) {
  if (ao_AnimData == NULL) {
    return;
  }
  
  // clamp animation
  if (iNew >= GetAnimsCt()) {
    iNew = 0;
  }
  
  // set new animation number
  ao_iCurrentAnim = iNew;
  ao_iLastAnim = ao_iCurrentAnim;
}

// Reset animation (restart)
void CAnimObject::ResetAnim() {
  if (ao_AnimData == NULL) {
    return;
  }
  
  // remember starting time
  ao_llAnimStart = _pTimer->GetGameTick();
  
  // mark that something has changed
  MarkChanged();
};

// Get info about some animation
void CAnimObject::GetAnimInfo(INDEX iAnimNo, CAnimInfo &aiInfo) const {
  if (iAnimNo >= ao_AnimData->ad_NumberOfAnims) {
    iAnimNo = 0;
  }
  
  ASSERT(iAnimNo < ao_AnimData->ad_NumberOfAnims);
  
  strcpy(aiInfo.ai_AnimName, ao_AnimData->ad_Anims[iAnimNo].oa_Name);
  
  aiInfo.ai_SecsPerFrame = ao_AnimData->ad_Anims[iAnimNo].oa_SecsPerFrame;
  aiInfo.ai_NumberOfFrames = ao_AnimData->ad_Anims[iAnimNo].oa_NumberOfFrames;
}

// Clip frame index to be inside valid range (wrap around for looping anims)
INDEX CAnimObject::ClipFrame(INDEX iFrame) const {
  if (ao_AnimData->ad_NumberOfAnims == 0) {
    return 0;
  }
  
  class COneAnim *pCOA = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
  
  // if looping
  if (ao_ulFlags & AOF_LOOPING) {
    // wrap-around
    if (pCOA->oa_NumberOfFrames <= 0) {
      return 0;
    }
    
    return ULONG(iFrame) % pCOA->oa_NumberOfFrames;
    
  // if not looping
  } else {
    // clamp
    if (iFrame < 0) {
      return 0;
      
    } else if (iFrame >= pCOA->oa_NumberOfFrames) {
      return pCOA->oa_NumberOfFrames - 1;
    }
  }
  
  return iFrame;
}

// Get info about time passed until now in current animation
TIME CAnimObject::GetPassedTime(void) const {
  if (ao_AnimData == NULL) {
    return 0.0f;
  }
  
  INDEX iStoppedFrame;

  // [Cecil] New timer: Anim time in seconds
  TIME tmAnim = CTimer::InSeconds(ao_llAnimStart);

  class COneAnim *pCOA = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
  
  if (!(ao_ulFlags & AOF_PAUSED)) {
    iStoppedFrame = ClipFrame((INDEX)(CTimer::InSeconds(_pTimer->GetGameTick() - ao_llAnimStart) / pCOA->oa_SecsPerFrame));
  } else {
    iStoppedFrame = ClipFrame((INDEX)(tmAnim / pCOA->oa_SecsPerFrame));
  }
  
  return (iStoppedFrame * pCOA->oa_SecsPerFrame);
}

// If animation is finished
BOOL CAnimObject::IsAnimFinished(void) const {
  if (ao_AnimData == NULL) {
    return FALSE;
  }
  
  if (ao_ulFlags & AOF_LOOPING) {
    return FALSE;
  }

  INDEX iStoppedFrame;

  // [Cecil] New timer: Anim time in seconds
  TIME tmAnim = CTimer::InSeconds(ao_llAnimStart);

  class COneAnim *pCOA = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
  
  if (!(ao_ulFlags & AOF_PAUSED)) {
    iStoppedFrame = ClipFrame((INDEX)(CTimer::InSeconds(_pTimer->GetGameTick() - ao_llAnimStart) / pCOA->oa_SecsPerFrame));
  } else {
    iStoppedFrame = ClipFrame((INDEX)(tmAnim / pCOA->oa_SecsPerFrame));
  }
  
  return (iStoppedFrame == pCOA->oa_NumberOfFrames - 1);
}

// Get number of animations in curent anim data
INDEX CAnimObject::GetAnimsCt(void) const {
  if (ao_AnimData == NULL) {
    return 1;
  }
  
  ASSERT(ao_AnimData != NULL);
  return (ao_AnimData->ad_NumberOfAnims);
};

// Get index of current animation
INDEX CAnimObject::GetAnim(void) const {
  return (ao_iCurrentAnim);
};

// Gets the number of current frame
INDEX CAnimObject::GetFrame(void) const {
  // return frame index that coresponds to current moment
  return FrameInTime(_pTimer->CurrentTick());
}

// Gets number of frames in current anim
INDEX CAnimObject::GetFramesInCurrentAnim(void) const {
  ASSERT(ao_AnimData != NULL);
  return ao_AnimData->ad_Anims[ao_iCurrentAnim].oa_NumberOfFrames;
};

// Get information for linear interpolation beetween frames
void CAnimObject::GetFrame(INDEX &iFrame0, INDEX &iFrame1, FLOAT &fRatio) const {
  if (ao_AnimData == NULL || ao_AnimData->ad_NumberOfAnims <= 0
   || ao_AnimData->ad_Anims[ao_iCurrentAnim].oa_NumberOfFrames <= 0) {
    iFrame0 = 0;
    iFrame1 = 0;
    fRatio = 0.0f;
    return;
  }
  
  ASSERT(ao_AnimData != NULL);
  ASSERT((ao_iCurrentAnim >= 0) && (ao_iCurrentAnim < ao_AnimData->ad_NumberOfAnims));
  FTICK ftNow = _pTimer->LerpedGameTick(); //_pTimer->CurrentTick() + _pTimer->GetLerpFactor() * _pTimer->TickQuantum;

  if (ao_ulFlags & AOF_PAUSED) {
    // return index of paused frame inside global frame array
    class COneAnim *pCOA = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
    INDEX iStoppedFrame = ClipFrame((SLONG)(CTimer::InSeconds(ao_llAnimStart) / pCOA->oa_SecsPerFrame));
    
    iFrame0 = iFrame1 = pCOA->oa_FrameIndices[iStoppedFrame];
    fRatio = 0.0f;
    
  } else {
    // return index of frame inside global frame array of frames in given moment
    FTICK ftCurrentRelative = ftNow - ao_llAnimStart;

    if (ftCurrentRelative >= 0) {
      class COneAnim *pOA0 = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
      float fFrameNow = CTimer::InSeconds(ftCurrentRelative) / pOA0->oa_SecsPerFrame;
      
      iFrame0 = pOA0->oa_FrameIndices[ClipFrame(ULONG(fFrameNow))];
      iFrame1 = pOA0->oa_FrameIndices[ClipFrame(ULONG(fFrameNow + 1))];
      
      fRatio = fFrameNow - (float)floor(fFrameNow);
      
    } else {
      class COneAnim *pOA0 = &ao_AnimData->ad_Anims[ao_iLastAnim];
      class COneAnim *pOA1 = &ao_AnimData->ad_Anims[ao_iCurrentAnim];
      INDEX iAnim = ao_iCurrentAnim;
      
      ((CAnimObject *)this)->ao_iCurrentAnim = ao_iLastAnim;
      float fFrameNow = CTimer::InSeconds(ftCurrentRelative) / pOA0->oa_SecsPerFrame + pOA0->oa_NumberOfFrames;
      iFrame0 = pOA0->oa_FrameIndices[Clamp(SLONG(fFrameNow), 0L, pOA0->oa_NumberOfFrames - 1L)];
      
      INDEX iFrameNext = SLONG(fFrameNow + 1);
      
      if (iFrameNext >= pOA0->oa_NumberOfFrames) {
        iFrame1 = pOA1->oa_FrameIndices[0];
      } else {
        iFrame1 = pOA0->oa_FrameIndices[Clamp(iFrameNext, 0L, pOA0->oa_NumberOfFrames - 1L)];
      }
      
      ((CAnimObject *)this)->ao_iCurrentAnim = iAnim;
      fRatio = fFrameNow - (float)floor(fFrameNow);
    }
  }
}

// Write anim object
void CAnimObject::Write_t(CTStream *pstr) {
  // [Cecil] New timer: 'ANim Object 2'
  (*pstr).WriteID_t("ANO2");
  (*pstr).WriteRawChunk_t(&ao_llAnimStart, sizeof(TICK));
  (*pstr).WriteRawChunk_t(&ao_iCurrentAnim, sizeof(INDEX));
  (*pstr).WriteRawChunk_t(&ao_iLastAnim, sizeof(INDEX));
  (*pstr).WriteRawChunk_t(&ao_ulFlags, sizeof(INDEX));
};

// Read anim object
void CAnimObject::Read_t(CTStream *pstr) {
  // [Cecil] New timer: 'ANim Object 2'
  if ((*pstr).PeekID_t() == CChunkID("ANO2")) {
    (*pstr).ExpectID_t("ANO2");
    (*pstr).ReadRawChunk_t(&ao_llAnimStart, sizeof(TICK));
    (*pstr).ReadRawChunk_t(&ao_iCurrentAnim, sizeof(INDEX));
    (*pstr).ReadRawChunk_t(&ao_iLastAnim, sizeof(INDEX));
    (*pstr).ReadRawChunk_t(&ao_ulFlags, sizeof(INDEX));

  } else if ((*pstr).PeekID_t() == CChunkID("ANOB")) {
    (*pstr).ExpectID_t("ANOB");

    // [Cecil] New timer: Convert to ticks
    TIME tmAnimStart;
    (*pstr).ReadRawChunk_t(&tmAnimStart, sizeof(TIME));
    ao_llAnimStart = CTimer::InTicks(tmAnimStart);

    (*pstr).ReadRawChunk_t(&ao_iCurrentAnim, sizeof(INDEX));
    (*pstr).ReadRawChunk_t(&ao_iLastAnim, sizeof(INDEX));
    (*pstr).ReadRawChunk_t(&ao_ulFlags, sizeof(INDEX));

  } else {
    (*pstr).ReadRawChunk_t(&ao_llAnimStart, sizeof(TICK));
    (*pstr).ReadRawChunk_t(&ao_iCurrentAnim, sizeof(INDEX));
    ao_iLastAnim = ao_iCurrentAnim;
    ao_ulFlags = 0;
  }

  // clamp animation
  if (ao_AnimData == NULL || ao_iCurrentAnim >= GetAnimsCt()) {
    ao_iCurrentAnim = 0;
  }
  
  // clamp animation
  if (ao_AnimData == NULL || ao_iLastAnim >= GetAnimsCt()) {
    ao_iLastAnim = 0;
  }
};
