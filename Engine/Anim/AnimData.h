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

#ifndef SE_INCL_ANIMDATA_H
#define SE_INCL_ANIMDATA_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Anim/Anim_internal.h>

// Animation data for a class of animatable objects
class CAnimData : public CSerial {
  public:
    INDEX ad_NumberOfAnims;
    class COneAnim *ad_Anims; // array of animations

  public:
    // Fill member variables with invalid data
    ENGINE_API CAnimData();

    // Free allocated data (ad_Anims array), check invalid data
    ENGINE_API ~CAnimData();

    // Clear animation data object
    ENGINE_API void Clear();

    // Check if this kind of objects is auto-freed
    virtual BOOL IsAutoFreed(void);

    // Reference counting functions
    virtual void RemReference_internal(void);

    // Get amount of memory used by this object
    SLONG GetUsedMemory(void);

    // Reference counting functions
    void AddReference(void);
    void RemReference(void);

    // Create given number of default animations (1 frame, given name and apeed)
    ENGINE_API void CreateAnimations(INDEX ctAnimations, CTString strName = "None", INDEX iDefaultFrame = 0, TIME tmSpeed = 0.02f);

    // Replace frames array with given one
    ENGINE_API void SetFrames(INDEX iAnimation, INDEX ctFrames, INDEX *pNewFrames);

    // Replace requested animation's name with given one
    ENGINE_API void SetName(INDEX iAnimation, CTString strNewName);

    // Replace requested animation's speed with given one
    ENGINE_API void SetSpeed(INDEX iAnimation, TIME tmSpeed);

    // Obtain frame index for given place in array representing given animation
    ENGINE_API INDEX GetFrame(INDEX iAnimation, INDEX iFramePlace);

    // Set frame index for given place in array representing given animation
    ENGINE_API void SetFrame(INDEX iAnimation, INDEX iFramePlace, INDEX iNewFrame);

    // Fill animation data object vith valid data containing one animation, one frame
    ENGINE_API void DefaultAnimation();

    // Get animation's info
    ENGINE_API void GetAnimInfo(INDEX iAnimNo, CAnimInfo &aiInfo) const;

    // Add animation
    ENGINE_API void AddAnimation(void);

    // Delete animation
    ENGINE_API void DeleteAnimation(INDEX iAnim);

    // Get number of animations
    ENGINE_API INDEX GetAnimsCt(void) const;

    // Load list of frames from script file
    ENGINE_API void LoadFromScript_t(CTStream *File, CListHead *FrameFileList); // throw char *

    // Print #define <animation name> lines for all animations into given file
    void ExportAnimationNames_t(CTStream *ostrFile, CTString strAnimationPrefix); // throw char *
    void Read_t(CTStream *istrFile); // throw char *
    void Write_t(CTStream *ostrFile); // throw char *
};

#endif /* include-once check. */
