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

#include <Engine/Anim/Anim_internal.h>

#include <Engine/Base/Memory.h>
#include <Engine/Base/ListIterator.inl>

// Remember given file name and add this node into string list
CFileNameNode::CFileNameNode(const char *NewFileName, CListHead *LH) {
  ASSERT(NewFileName != NULL);
  ASSERT(strlen(NewFileName) > 0);

  strcpy(cfnn_FileName, NewFileName);
  LH->AddTail(cfnn_Node);
};

// Constructor
COneAnim::COneAnim() {
  oa_FrameIndices = NULL;
};

// Destructor
COneAnim::~COneAnim() {
  ASSERT(oa_FrameIndices != NULL);
  
  // free allocated frame indices array for this animation
  FreeMemory(oa_FrameIndices);
  oa_FrameIndices = NULL;
};

// Assignment
COneAnim &COneAnim::operator=(const COneAnim &oaAnim) {
  ASSERT(oaAnim.oa_NumberOfFrames > 0);

  strcpy(oa_Name, oaAnim.oa_Name);

  oa_SecsPerFrame = oaAnim.oa_SecsPerFrame;
  oa_NumberOfFrames = oaAnim.oa_NumberOfFrames;

  if (oa_FrameIndices != NULL) {
    FreeMemory(oa_FrameIndices);
  }

  oa_FrameIndices = (INDEX *)AllocMemory(sizeof(INDEX) * oa_NumberOfFrames);

  for (INDEX iFrame = 0; iFrame < oa_NumberOfFrames; iFrame++) {
    oa_FrameIndices[iFrame] = oaAnim.oa_FrameIndices[iFrame];
  }

  return *this;
};

// Remember pointer to animation and add this node at the end of given animation list
COneAnimNode::COneAnimNode(COneAnim *AnimToInsert, CListHead *LH) {
  coan_OneAnim = AnimToInsert;
  LH->AddTail(coan_Node);
};

// Destructor
COneAnimNode::~COneAnimNode() {
  // delete the animation
  ASSERT(coan_OneAnim != NULL);
  delete coan_OneAnim;
};

// Destructor
CTmpListHead::~CTmpListHead() {
  FORDELETELIST(COneAnimNode, coan_Node, *this, it) {
    delete &it.Current();
  }
};
