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

#ifndef SE_INCL_NETWORKMESSAGE_H
#define SE_INCL_NETWORKMESSAGE_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Base/Lists.h>
#include <Engine/Math/Vector.h>

// Message type transmitted as 6-bit value (up to 64 values allowed)
// Upper 2 bits are used to indicate compression used

// Add new message types to the end
typedef enum NetworkMessageType {
  // broadcast requesting server infos
  MSG_REQ_ENUMSERVERS,
  MSG_SERVERINFO,

  MSG_KEEPALIVE, // sent when there's nothing else to send - just to keep connections valid

  // disconnection explanation from server
  MSG_INF_DISCONNECTED,

  // info message with pings of all players
  MSG_INF_PINGS,

  // main session state connecting to server
  MSG_REQ_CONNECTLOCALSESSIONSTATE,
  MSG_REP_CONNECTLOCALSESSIONSTATE,

  // remote session state connecting to server
  MSG_REQ_CONNECTREMOTESESSIONSTATE,
  MSG_REP_CONNECTREMOTESESSIONSTATE,

  // remote session requesting current state delta from original
  MSG_REQ_STATEDELTA,
  MSG_REP_STATEDELTA,

  // client initialing CRC check
  MSG_REQ_CRCLIST,
  MSG_REQ_CRCCHECK,
  MSG_REP_CRCCHECK,

  // player connecting to server
  MSG_REQ_CONNECTPLAYER,
  MSG_REP_CONNECTPLAYER,

  MSG_REQ_PAUSE, // request pause/unpause game

  // request character change for a player
  MSG_REQ_CHARACTERCHANGE,

  MSG_ACTION, // action packet from client to server
  MSG_SYNCCHECK, // data to check for lost synchronization (client to server)
  MSG_ACTIONPREDICT, // a copy of action stored for prediction

  // sequenced packets from server to session states
  MSG_SEQ_ALLACTIONS,      // packed actions of all players from server to clients
  MSG_SEQ_ADDPLAYER,       // instructions for adding a new player to session states
  MSG_SEQ_REMPLAYER,       // instructions for removing a new player from session states
  MSG_SEQ_PAUSE,           // game was paused/unpaused
  MSG_SEQ_CHARACTERCHANGE, // a player has changed character

  MSG_GAMESTREAMBLOCKS,        // packet with one or more game stream messages
  MSG_REQUESTGAMESTREAMRESEND, // request for resend of a game stream message

  // chat messages
  MSG_CHAT_IN,  // chat request from client to server
  MSG_CHAT_OUT, // chat message routed to certain clients

  // parameter setting messages
  MSG_SET_CLIENTSETTINGS, // adjust server side settings of a client

  // remote administration
  MSG_ADMIN_COMMAND,  // c2s incoming console command request
  MSG_ADMIN_RESPONSE, // s2c results of the console command

  MSG_EXTRA = '/', // used for special communications like rcon and similar

  // added to the end so that it would not mess up old numbering - that would corrupt demo playing
  // disconnection confirmation from the client
  MSG_REP_DISCONNECTED,

} MESSAGETYPE;

extern struct ErrorTable MessageTypes;

// Holder for network message, can be read/written like a stream
class ENGINE_API CNetworkMessage {
  public:
    MESSAGETYPE nm_mtType; // type of this message

    #define MAX_NETWORKMESSAGE_SIZE 2048 // max. length of message buffer
    UBYTE *nm_pubMessage; // the message data itself
    SLONG nm_slMaxSize; // size of message buffer

    UBYTE *nm_pubPointer; // pointer for reading/writing message
    SLONG nm_slSize; // size of message
    INDEX nm_iBit; // next bit index to read/write (0 if not reading/writing bits)

  public:
    // Constructor for empty message (for receiving)
    CNetworkMessage(void);

    // Constructor for initializing message that is to be sent
    CNetworkMessage(MESSAGETYPE mtType);

    // Copy constructor
    CNetworkMessage(const CNetworkMessage &nmOriginal);

    // Assignment
    void operator=(const CNetworkMessage &nmOriginal);

    // Destructor
    ~CNetworkMessage(void);

    // Reinit a message that is to be sent (to write different contents)
    void Reinit(void);

    // Ignore the contents of this message
    void IgnoreContents(void);

    // Dump message to console
    void Dump(void);

    // Get the type of this message
    inline MESSAGETYPE GetType(void) const {
      ASSERT(this != NULL);
      return MESSAGETYPE(nm_mtType & 0x3F);
    };

    // Check if end of message
    BOOL EndOfMessage(void);

    // Rewind message to start, so that written message can be read again
    void Rewind(void);

    // Pack a message to another message (message type is left untouched)
    void Pack(CNetworkMessage &nmPacked, CCompressor &comp);
    void PackDefault(CNetworkMessage &nmPacked);

    // Unpack a message to another message (message type is left untouched)
    void Unpack(CNetworkMessage &nmUnpacked, CCompressor &comp);
    void UnpackDefault(CNetworkMessage &nmUnpacked);

    // Write and read
    void Read(void *pvBuffer, SLONG slSize);
    void Write(const void *pvBuffer, SLONG slSize);
    void ReadBits(void *pvBuffer, INDEX ctBits);
    void WriteBits(const void *pvBuffer, INDEX ctBits);

    // [Cecil] 2021-02-21: Macro for reading objects
    #define DEFINE_NMSG_READ(_Type) \
      inline CNetworkMessage &operator>>(_Type &_Val) { \
        Read(&_Val, sizeof(_Val)); \
        return *this; \
      }

    // Read an object from message
    DEFINE_NMSG_READ(float);
    DEFINE_NMSG_READ(ULONG);
    DEFINE_NMSG_READ(UWORD);
    DEFINE_NMSG_READ(UBYTE);
    DEFINE_NMSG_READ(SLONG);
    DEFINE_NMSG_READ(SWORD);
    DEFINE_NMSG_READ(SBYTE);
    DEFINE_NMSG_READ(MESSAGETYPE);

    CNetworkMessage &operator>>(CTString &str);

    // [Cecil] 2021-02-20: Macro for writing objects
    #define DEFINE_NMSG_WRITE(_Type) \
      inline CNetworkMessage &operator<<(const _Type &_Val) { \
        Write(&_Val, sizeof(_Val)); \
        return *this; \
      }

    // Write an object into message
    DEFINE_NMSG_WRITE(float);
    DEFINE_NMSG_WRITE(ULONG);
    DEFINE_NMSG_WRITE(UWORD);
    DEFINE_NMSG_WRITE(UBYTE);
    DEFINE_NMSG_WRITE(SLONG);
    DEFINE_NMSG_WRITE(SWORD);
    DEFINE_NMSG_WRITE(SBYTE);
    DEFINE_NMSG_WRITE(MESSAGETYPE);

    CNetworkMessage &operator<<(const CTString &str);

    // [Cecil] New timer: Ticks
    DEFINE_NMSG_READ(TICK);
    DEFINE_NMSG_WRITE(TICK);

    // Insert a sub-message into this message
    void InsertSubMessage(const CNetworkMessage &nmSubMessage);

    // Extract a sub-message from this message
    void ExtractSubMessage(CNetworkMessage &nmSubMessage);

    // Shrink message buffer to exactly fit contents
    void Shrink(void);
};

/* These can be received duplicated or misordered. They are resequenced at the receive side as needed
   Can be sent more than one together as submessages in a message and duplicated across messages
   as a compensation for eventual packet loss. */

// A message block used for streaming data across network
class CNetworkStreamBlock : public CNetworkMessage {
  public:
    CListNode nsb_lnInStream; // node in list of blocks in stream
    INDEX nsb_iSequenceNumber; // index for sorting in list

  public:
    // Constructor for receiving - uninitialized block
    CNetworkStreamBlock(void);

    // Constructor for sending - empty packet with given type and sequence
    CNetworkStreamBlock(MESSAGETYPE mtType, INDEX iSequenceNumber);

    // Read a block from a received message
    void ReadFromMessage(CNetworkMessage &nmToRead);

    // Add a block to a message to send
    void WriteToMessage(CNetworkMessage &nmToWrite);

    // Remove the block from stream
    void RemoveFromStream(void);

    // Write and read the block
    void Write_t(CTStream &strm); // throw char *
    void Read_t(CTStream &strm); // throw char *
};

// Stream of message blocks that can be sent across network
class CNetworkStream {
  public:
    enum Result {
      R_OK = 1,
      R_BLOCKMISSING,        // block is missing in the stream
      R_BLOCKNOTRECEIVEDYET, // block is not yet received
    };

  public:
    CListHead ns_lhBlocks; // list of blocks of this stream (higher sequences first)

    // Add a block that is already allocated to the stream
    void AddAllocatedBlock(CNetworkStreamBlock *pnsbBlock);

  public:
    // Constructor
    CNetworkStream(void);

    // Destructor
    ~CNetworkStream(void);

    // Clear the object (remove all blocks)
    void Clear(void);

    // Copy from another network stream
    void Copy(CNetworkStream &nsOther);

    // Get number of blocks used by this object
    INDEX GetUsedBlocks(void);

    // Get amount of memory used by this object
    SLONG GetUsedMemory(void);

    // Get index of newest sequence stored
    INDEX GetNewestSequence(void);

    // Add a block to the stream (makes a copy of block)
    void AddBlock(CNetworkStreamBlock &nsbBlock);

    // Read a block as a submessage from a message and add it to the stream
    void ReadBlock(CNetworkMessage &nmMessage);

    // Get a block from stream by its sequence number
    CNetworkStream::Result GetBlockBySequence(INDEX iSequenceNumber, CNetworkStreamBlock *&pnsbBlock);

    // Find oldest block after given one (for batching missing sequences)
    INDEX GetOldestSequenceAfter(INDEX iSequenceNumber);

    // Write given number of newest blocks to a message
    INDEX WriteBlocksToMessage(CNetworkMessage &nmMessage, INDEX ctBlocks);

    // Remove all blocks but the given number of newest ones
    void RemoveOlderBlocks(INDEX ctBlocksToKeep);

    // Remove all blocks with sequence older than given
    void RemoveOlderBlocksBySequence(INDEX iLastSequenceToKeep);
};

class ENGINE_API CPlayerAction {
  public:
    // Order is important for compression and normalization - do not reorder!
    FLOAT3D pa_vTranslation;
    ANGLE3D pa_aRotation;
    ANGLE3D pa_aViewRotation;

    // Keep flags that are likely to be changed/set more often at lower bits,
    // so that better compression can be achieved for network transmission
    ULONG pa_ulButtons; // 32 bits for action buttons (application defined)

    __int64 pa_llCreated; // when was created (for ping calc.) in ms

  public:
    CPlayerAction(void);

    // Clear the object (this sets up no actions)
    void Clear(void);

    // Normalize action (remove invalid floats like -0)
    void Normalize(void);

    // Create a checksum value for sync-check
    void ChecksumForSync(ULONG &ulCRC);

    // Dump sync data to text file
    void DumpSync_t(CTStream &strm); // throw char *

    void Lerp(const CPlayerAction &pa0, const CPlayerAction &pa1, FLOAT fFactor);

    // Write an object into message
    friend CNetworkMessage &operator<<(CNetworkMessage &nm, const CPlayerAction &pa);

    // Read an object from message
    friend CNetworkMessage &operator>>(CNetworkMessage &nm, CPlayerAction &pa);

    // Write an object into stream
    friend CTStream &operator<<(CTStream &strm, const CPlayerAction &pa);

    // Read an object from stream
    friend CTStream &operator>>(CTStream &strm, CPlayerAction &pa);
};

#endif /* include-once check. */
