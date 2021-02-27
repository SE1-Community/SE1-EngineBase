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

#include <Engine/Anim/AnimData.h>

#include <Engine/Base/Stream.h>
#include <Engine/Templates/DynamicArray.cpp>
#include <Engine/Templates/Stock_CAnimData.h>

// Constructor
CAnimData::CAnimData() {
  ad_Anims = NULL;
  ad_NumberOfAnims = 0;
};

// Destructor
CAnimData::~CAnimData() {
  Clear();
};

// Clear animation data object
void CAnimData::Clear() {
  if (ad_Anims != NULL) {
    delete[] ad_Anims;
  }

  ad_Anims = NULL;
  ad_NumberOfAnims = 0;

  // clear serial object
  CSerial::Clear();
};

// Get amount of memory used by this object
SLONG CAnimData::GetUsedMemory(void) {
  SLONG slUsed = sizeof(*this) + sizeof(COneAnim) * ad_NumberOfAnims;
  slUsed += strlen(GetName()) + 1;

  for (INDEX iAnim = 0; iAnim < ad_NumberOfAnims; iAnim++) {
    slUsed += ad_Anims[iAnim].oa_NumberOfFrames * sizeof(INDEX);
  }

  return slUsed;
}

// Check if this kind of objects is auto-freed
BOOL CAnimData::IsAutoFreed(void) {
  return FALSE;
}

// Reference counting functions
void CAnimData::AddReference(void) {
  if (this != NULL) {
    MarkUsed();
  }
};

void CAnimData::RemReference(void) {
  if (this != NULL) {
    RemReference_internal();
  }
};

void CAnimData::RemReference_internal(void) {
  _pAnimStock->Release(this);
};

// Create given number of default animations (1 frame, given name and speed)
void CAnimData::CreateAnimations(INDEX ctAnimations, CTString strName, INDEX iDefaultFrame, TIME tmSpeed) {
  ASSERT(strlen(strName) < NAME_SIZE);

  // clear existing animations
  Clear();

  // set new number of anims
  ad_NumberOfAnims = ctAnimations;

  // create requested number of animations
  ad_Anims = new COneAnim[ctAnimations];

  // set default data for aeach new animation
  for (INDEX iAnimations = 0; iAnimations < ctAnimations; iAnimations++) {
    // set default (or given) name
    strcpy(ad_Anims[iAnimations].oa_Name, strName);

    // set default (or given) speed
    ad_Anims[iAnimations].oa_SecsPerFrame = tmSpeed;

    // create one frame for this animation
    ad_Anims[iAnimations].oa_NumberOfFrames = 1;
    ad_Anims[iAnimations].oa_FrameIndices = (INDEX *)AllocMemory(sizeof(INDEX));
    ad_Anims[iAnimations].oa_FrameIndices[0] = iDefaultFrame;
  }
}

// Replace frames array with given one
void CAnimData::SetFrames(INDEX iAnimation, INDEX ctFrames, INDEX *pNewFrames) {
  ASSERT(iAnimation < ad_NumberOfAnims);

  // clear existing animation
  if (ad_Anims[iAnimation].oa_FrameIndices != NULL) {
    FreeMemory(ad_Anims[iAnimation].oa_FrameIndices);
  }

  // create array for new frames
  ad_Anims[iAnimation].oa_FrameIndices = (INDEX *)AllocMemory(sizeof(INDEX) * ctFrames);

  // copy given array of frames
  for (INDEX iFrames = 0; iFrames < ctFrames; iFrames++) {
    ad_Anims[iAnimation].oa_FrameIndices[iFrames] = pNewFrames[iFrames];
  }

  // set new number of frames
  ad_Anims[iAnimation].oa_NumberOfFrames = ctFrames;
}

// Fill animation data to contain one animation named "OnlyAnim" with one frame
void CAnimData::DefaultAnimation() {
  ASSERT(ad_Anims == NULL);

  ad_NumberOfAnims = 1;
  ad_Anims = new COneAnim[1];
  strcpy(ad_Anims->oa_Name, "OnlyAnim");

  ad_Anims->oa_SecsPerFrame = (TIME)0.02;
  ad_Anims->oa_NumberOfFrames = 1;
  ad_Anims->oa_FrameIndices = (INDEX *)AllocMemory(sizeof(INDEX));
  ad_Anims->oa_FrameIndices[0] = 0;
};

// Return index of given frame name in global frame names list
// If it is not found, new CFileNameObject is added into frames list
INDEX FindFrameIndex(CListHead *pFrameFileList, const char *pFileName) {
  UWORD i = 0;

  FOREACHINLIST(CFileNameNode, cfnn_Node, *pFrameFileList, it) {
    if (strcmpi(it->cfnn_FileName, pFileName) == 0) {
      return (i);
    }

    i++;
  }

  new CFileNameNode(pFileName, pFrameFileList);

  return (i);
};

CTString GetFrameFileName(CListHead *pFrameFileList, INDEX iMemberInList) {
  ASSERT(iMemberInList < pFrameFileList->Count());

  UWORD iMember = 0;

  FOREACHINLIST(CFileNameNode, cfnn_Node, *pFrameFileList, it) {
    if (iMember == iMemberInList) {
      return CTString(it->cfnn_FileName);
    }

    iMember++;
  }

  ASSERTALWAYS("Frame with given index is not found in list of frames");
  return "";
}

// If found given word at the beginning of curently loaded line
#define EQUAL_SUB_STR(str) (strnicmp(ld_line, str, strlen(str)) == 0)

// Loads part of given script file until word AnimEnd is reached
// Fills ACanimData (its instance) with apropriate data (animations and their frame indices)
// and fills given list head with string nodes containing file names representing frames
// needed to be loaded by a parent object
void CAnimData::LoadFromScript_t(CTStream *File, CListHead *pFrameFileList) {
  UWORD i;
  char error_str[256];
  char key_word[256];
  char base_path[PATH_MAX] = "";
  char file_name[PATH_MAX];
  char anim_name[256];
  char full_path[PATH_MAX];
  char ld_line[128];

  CTmpListHead TempAnimationList;

  //ASSERT(ad_Anims == NULL);

  // clears possible animations
  CAnimData::Clear();

  FOREVER {
    // Repeat reading of one line of script file until it is not empty or comment
    do {
      File->GetLine_t(ld_line, 128);
    } while ((strlen(ld_line) == 0) || (ld_line[0] == ';'));

    // If key-word is "/*", search end of comment block
    if (EQUAL_SUB_STR("/*")) {
      do {
        File->GetLine_t(ld_line, 128);
      } while (!EQUAL_SUB_STR("*/"));
    }

    // If key-word is "DIRECTORY", remember it and add "\" character at the end of new path
    // if it is not yet there
    else if (EQUAL_SUB_STR("DIRECTORY")) {
      _strupr(ld_line);
      sscanf(ld_line, "DIRECTORY %s", base_path);

      if (base_path[strlen(base_path) - 1] != '\\') {
        strcat(base_path, "\\");
      }
    }

    // Key-word animation must follow its name (in same line),
    // its speed and its number of frames (new lines)
    else if (EQUAL_SUB_STR("ANIMATION")) {
      if (strlen(ld_line) <= (strlen("ANIMATION") + 1)) {
        throw("You have to give descriptive name to every animation.");
      }

      // Create new animation
      COneAnim *poaOneAnim = new COneAnim;
      _strupr(ld_line);
      sscanf(ld_line, "ANIMATION %s", poaOneAnim->oa_Name);

      File->GetLine_t(ld_line, 128);

      if (!EQUAL_SUB_STR("SPEED")) {
        throw("Expecting key word \"SPEED\" after key word \"ANIMATION\".");
      }

      _strupr(ld_line);
      sscanf(ld_line, "SPEED %f", &poaOneAnim->oa_SecsPerFrame);

      CDynamicArray<CTString> astrFrames;
      SLONG slLastPos;

      FOREVER {
        slLastPos = File->GetPos_t();
        File->GetLine_t(ld_line, 128);
        _strupr(ld_line);

        // jump over old key word "FRAMES" and comments
        if (EQUAL_SUB_STR("FRAMES") || (ld_line[0] == ';')) {
          continue;
        }

        // key words that start or end animations or empty line breaks frame reading
        if ((EQUAL_SUB_STR("ANIMATION")) || (strlen(ld_line) == 0) || (EQUAL_SUB_STR("ANIM_END"))) {
          break;
        }

        sscanf(ld_line, "%s", key_word);

        if (key_word == CTString("ANIM")) {
          // read file name from line and add it at the end of last path string loaded
          sscanf(ld_line, "%s %s", error_str, anim_name);

          // search trough all allready readed animations for macro one
          FOREACHINLIST(COneAnimNode, coan_Node, TempAnimationList, itOAN) {
            if (itOAN->coan_OneAnim->oa_Name == CTString(anim_name)) {
              CTString *pstrMacroFrames = astrFrames.New(itOAN->coan_OneAnim->oa_NumberOfFrames);

              for (INDEX iMacroFrame = 0; iMacroFrame < itOAN->coan_OneAnim->oa_NumberOfFrames; iMacroFrame++) {
                *pstrMacroFrames = GetFrameFileName(pFrameFileList, itOAN->coan_OneAnim->oa_FrameIndices[iMacroFrame]);
                pstrMacroFrames++;
              }
            }
          }

        } else {
          // read file name from line and add it at the end of last path string loaded
          sscanf(ld_line, "%s", file_name);
          sprintf(full_path, "%s%s", base_path, file_name);
          CTString *pstrNewFile = astrFrames.New(1);
          *pstrNewFile = CTString(full_path);
        }
      }

      if (astrFrames.Count() == 0) {
        ThrowF_t("Can't find any frames for animation %s.\nThere must be at least 1 frame "
                 "per animation.\nList of frames must start at line after line containing key"
                 "word SPEED.",
                 poaOneAnim->oa_Name);
      }

      // set position before last line readed
      File->SetPos_t(slLastPos);

      // Allocate array of indices
      poaOneAnim->oa_NumberOfFrames = astrFrames.Count();
      poaOneAnim->oa_FrameIndices = (INDEX *)AllocMemory(poaOneAnim->oa_NumberOfFrames * sizeof(INDEX));

      INDEX iFrame = 0;

      FOREACHINDYNAMICARRAY(astrFrames, CTString, itStrFrame) {
        // find existing index (of insert new one) for this file name into FileNameList
        poaOneAnim->oa_FrameIndices[iFrame] = FindFrameIndex(pFrameFileList, *itStrFrame);
        iFrame++;
      }

      // clear used array
      astrFrames.Clear();

      // Add this new animation instance to temporary animation list
      new COneAnimNode(poaOneAnim, &TempAnimationList);
      ad_NumberOfAnims++;

    } else if (EQUAL_SUB_STR("ANIM_END")) {
      break;

    } else {
      sprintf(error_str, "Incorrect word readed from script file.\n");
      strcat(error_str, "Probable cause: missing \"ANIM_END\" key-word at end of animation list.");

      throw(error_str);
    }
  }

  SLONG lc = TempAnimationList.Count();
  ASSERT(lc != 0);

  // create array of OneAnim object containing members as many as temporary list
  ad_Anims = new COneAnim[lc];

  // copy list to array
  lc = 0;

  FOREACHINLIST(COneAnimNode, coan_Node, TempAnimationList, it2) {
    strcpy(ad_Anims[lc].oa_Name, it2->coan_OneAnim->oa_Name);
    ad_Anims[lc].oa_SecsPerFrame = it2->coan_OneAnim->oa_SecsPerFrame;
    ad_Anims[lc].oa_NumberOfFrames = it2->coan_OneAnim->oa_NumberOfFrames;
    ad_Anims[lc].oa_FrameIndices = (INDEX *)AllocMemory(ad_Anims[lc].oa_NumberOfFrames * sizeof(INDEX));

    for (i = 0; i < it2->coan_OneAnim->oa_NumberOfFrames; i++) {
      ad_Anims[lc].oa_FrameIndices[i] = it2->coan_OneAnim->oa_FrameIndices[i];
    }

    lc++;
  }

  FORDELETELIST(COneAnimNode, coan_Node, TempAnimationList, litDel) {
    delete &litDel.Current();
  }
};

void CAnimData::Write_t(CTStream *ostrFile) {
  // First we save main ID
  ostrFile->WriteID_t(CChunkID("ADAT"));

  // Then we save number of how many animations do we have and then save them all
  ostrFile->Write_t(&ad_NumberOfAnims, sizeof(INDEX));

  for (SLONG i = 0; i < ad_NumberOfAnims; i++) {
    // Next block saves all data for one animation
    ostrFile->Write_t(&ad_Anims[i].oa_Name, sizeof(NAME));
    ostrFile->Write_t(&ad_Anims[i].oa_SecsPerFrame, sizeof(TIME));
    ostrFile->Write_t(&ad_Anims[i].oa_NumberOfFrames, sizeof(INDEX));
    ostrFile->Write_t(ad_Anims[i].oa_FrameIndices, ad_Anims[i].oa_NumberOfFrames * sizeof(INDEX));
  }
};

// Print #define <animation name> lines for all animations into given file
void CAnimData::ExportAnimationNames_t(CTStream *ostrFile, CTString strAnimationPrefix) {
  char chrLine[256];

  // for each animation
  for (INDEX iAnimation = 0; iAnimation < ad_NumberOfAnims; iAnimation++) {
    // prepare one #define line (add prefix)
    sprintf(chrLine, "#define %s%s %d", strAnimationPrefix, ad_Anims[iAnimation].oa_Name, iAnimation);

    // put it into file
    ostrFile->PutLine_t(chrLine);
  }
}

// Get info about some animation
void CAnimData::GetAnimInfo(INDEX iAnimNo, CAnimInfo &aiInfo) const {
  if (iAnimNo >= ad_NumberOfAnims) {
    iAnimNo = 0;
  }

  strcpy(aiInfo.ai_AnimName, ad_Anims[iAnimNo].oa_Name);
  aiInfo.ai_SecsPerFrame = ad_Anims[iAnimNo].oa_SecsPerFrame;
  aiInfo.ai_NumberOfFrames = ad_Anims[iAnimNo].oa_NumberOfFrames;
}

// Add animation
void CAnimData::AddAnimation(void) {
  COneAnim *pNewAnims = new COneAnim[ad_NumberOfAnims + 1];

  for (INDEX iOldAnim = 0; iOldAnim < ad_NumberOfAnims; iOldAnim++) {
    pNewAnims[iOldAnim] = ad_Anims[iOldAnim];
  }

  // set default values for added animation
  strcpy(pNewAnims[ad_NumberOfAnims].oa_Name, "New animation");
  pNewAnims[ad_NumberOfAnims].oa_SecsPerFrame = 0.02f;

  // create one frame for this animation
  pNewAnims[ad_NumberOfAnims].oa_NumberOfFrames = 1;
  pNewAnims[ad_NumberOfAnims].oa_FrameIndices = (INDEX *)AllocMemory(sizeof(INDEX));
  pNewAnims[ad_NumberOfAnims].oa_FrameIndices[0] = 0;

  // release old array
  delete[] ad_Anims;

  // copy animations ptr
  ad_Anims = pNewAnims;
  ad_NumberOfAnims++;
}

// Replace requested animation's name with given one
void CAnimData::SetName(INDEX iAnimation, CTString strNewName) {
  ASSERT(strlen(strNewName) < NAME_SIZE);
  strcpy(ad_Anims[iAnimation].oa_Name, strNewName);
};

// Replace requested animation's speed with given one
void CAnimData::SetSpeed(INDEX iAnimation, TIME tmSpeed) {
  ad_Anims[iAnimation].oa_SecsPerFrame = tmSpeed;
};

// Obtain frame index for given place in array representing given animation
INDEX CAnimData::GetFrame(INDEX iAnimation, INDEX iFramePlace) {
  ASSERT(iFramePlace < ad_Anims[iAnimation].oa_NumberOfFrames);
  return ad_Anims[iAnimation].oa_FrameIndices[iFramePlace];
};

// Set frame index for given place in array representing given animation
void CAnimData::SetFrame(INDEX iAnimation, INDEX iFramePlace, INDEX iNewFrame) {
  ASSERT(iFramePlace < ad_Anims[iAnimation].oa_NumberOfFrames);
  ad_Anims[iAnimation].oa_FrameIndices[iFramePlace] = iNewFrame;
};

// Get number of animations
INDEX CAnimData::GetAnimsCt(void) const {
  return ad_NumberOfAnims;
};

// Delete animation
void CAnimData::DeleteAnimation(INDEX iAnim) {
  if (ad_NumberOfAnims <= 1) {
    return;
  }

  COneAnim *pNewAnims = new COneAnim[ad_NumberOfAnims - 1];
  INDEX iNewAnim = 0;

  for (INDEX iOldAnim = 0; iOldAnim < ad_NumberOfAnims; iOldAnim++) {
    // copy all animations except one to delete
    if (iOldAnim != iAnim) {
      pNewAnims[iNewAnim] = ad_Anims[iOldAnim];
      iNewAnim++;
    }
  }

  // release old array of animation
  delete[] ad_Anims;

  // copy animations ptr
  ad_Anims = pNewAnims;
  ad_NumberOfAnims--;
}

// While loading object containing DataObject and expect DataObject definition to be loaded,
// call its Load function. Then it will call this Read function to load data from an open file
void CAnimData::Read_t(CTStream *istrFile) {
  ASSERT(ad_Anims == NULL);

  // First we recognize main ID
  istrFile->ExpectID_t(CChunkID("ADAT"));

  // Then we load and create number of animations
  istrFile->Read_t(&ad_NumberOfAnims, sizeof(INDEX));
  ad_Anims = new COneAnim[ad_NumberOfAnims];

  for (SLONG i = 0; i < ad_NumberOfAnims; i++) {
    // Next block reads and allocates all data for one animation
    istrFile->Read_t(&ad_Anims[i].oa_Name, sizeof(NAME));
    istrFile->Read_t(&ad_Anims[i].oa_SecsPerFrame, sizeof(TIME));
    istrFile->Read_t(&ad_Anims[i].oa_NumberOfFrames, sizeof(INDEX));

    ad_Anims[i].oa_FrameIndices = (INDEX *)AllocMemory(ad_Anims[i].oa_NumberOfFrames * sizeof(INDEX));
    istrFile->Read_t(ad_Anims[i].oa_FrameIndices, ad_Anims[i].oa_NumberOfFrames * sizeof(INDEX));
  }
};
