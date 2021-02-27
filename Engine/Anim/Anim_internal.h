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

#ifndef SE_INCL_ANIM_INTERNAL_H
#define SE_INCL_ANIM_INTERNAL_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Base/Serial.h>
#include <Engine/Base/Lists.h>

#define NAME_SIZE 32
typedef char NAME[NAME_SIZE];

#define PATH_MAX 260
typedef char FILE_NAME[PATH_MAX];

// An object used for obtaining animation's information
class CAnimInfo {
  public:
    NAME ai_AnimName;
    TIME ai_SecsPerFrame; // speed of this animation in seconds
    INDEX ai_NumberOfFrames;
};

// Node used for linking file names representing frames
// Nodes of this kind are returned result of LoadFromScript function
class ENGINE_API CFileNameNode {
  public:
    FILE_NAME cfnn_FileName;
    CListNode cfnn_Node;

    // Remember given file name and add this node into string list
    CFileNameNode(const char *NewFileName, CListHead *LH);
};

// One animation of an animatable object
class COneAnim {
  public:
    NAME oa_Name; // animation name
    TIME oa_SecsPerFrame; // speed of this animation
    INDEX oa_NumberOfFrames; // frame count in the animation
    INDEX *oa_FrameIndices; // array of frame indices

    // Constructor
    COneAnim();

    // Destructor
    ~COneAnim();

    // Assignment
    COneAnim &operator=(const COneAnim &oaAnim);
};

// Node used for linking pointers to COneAnim objects while loading
// script file before turning them into an array
class COneAnimNode {
  public:
    CListNode coan_Node;
    COneAnim *coan_OneAnim;

    // Remember pointer to animation and add this node at the end of given animation list
    COneAnimNode(COneAnim *AnimToInsert, CListHead *LH);

    // Destructor
    ~COneAnimNode();
};

// This temporary list head class is used for automatic deleting of temporary list on exit
class CTmpListHead : public CListHead {
  public:
    // Destructor
    ~CTmpListHead();
};

#endif /* include-once check. */
