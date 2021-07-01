/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// common.h
// Definitions common between client and server, but not game.dll
//

#ifndef __COMMON_H__
#define __COMMON_H__

#include "../shared/shared.h"
#include "files.h"
#include "font.h"

#define	BASE_MODDIRNAME		"baseq2"

/*
=============================================================================

	PROTOCOL

=============================================================================
*/

#define	PROTOCOL_VERSION	34

#define	UPDATE_BACKUP		16	// copies of entityState_t to keep buffered must be power of two
#define	UPDATE_MASK			(UPDATE_BACKUP-1)

//
// client to server
//
enum {
	CLC_BAD,
	CLC_NOP,
	CLC_MOVE,				// [[userCmd_t]
	CLC_USERINFO,			// [[userinfo string]
	CLC_STRINGCMD			// [string] message
};

// playerState_t communication
enum {
	PS_M_TYPE			= 1 << 0,
	PS_M_ORIGIN			= 1 << 1,
	PS_M_VELOCITY		= 1 << 2,
	PS_M_TIME			= 1 << 3,
	PS_M_FLAGS			= 1 << 4,
	PS_M_GRAVITY		= 1 << 5,
	PS_M_DELTA_ANGLES	= 1 << 6,

	PS_VIEWOFFSET		= 1 << 7,
	PS_VIEWANGLES		= 1 << 8,
	PS_KICKANGLES		= 1 << 9,
	PS_BLEND			= 1 << 10,
	PS_FOV				= 1 << 11,
	PS_WEAPONINDEX		= 1 << 12,
	PS_WEAPONFRAME		= 1 << 13,
	PS_RDFLAGS			= 1 << 14
};


// userCmd_t communication
// ms and light always sent, the others are optional
enum {
	CM_ANGLE1		= 1 << 0,
	CM_ANGLE2		= 1 << 1,
	CM_ANGLE3		= 1 << 2,
	CM_FORWARD		= 1 << 3,
	CM_SIDE			= 1 << 4,
	CM_UP			= 1 << 5,
	CM_BUTTONS		= 1 << 6,
	CM_IMPULSE		= 1 << 7
};


// sound communication
// a sound without an ent or pos will be a local only sound
enum {
	SND_VOLUME		= 1 << 0,	// a byte
	SND_ATTENUATION	= 1 << 1,	// a byte
	SND_POS			= 1 << 2,	// three coordinates
	SND_ENT			= 1 << 3,	// a short 0-2: channel, 3-12: entity
	SND_OFFSET		= 1 << 4	// a byte, msec offset from frame start
};

#define DEFAULT_SOUND_PACKET_VOLUME			1.0
#define DEFAULT_SOUND_PACKET_ATTENUATION	1.0

// entityState_t communication
enum {
	// try to pack the common update flags into the first byte
	U_ORIGIN1		= 1 << 0,
	U_ORIGIN2		= 1 << 1,
	U_ANGLE2		= 1 << 2,
	U_ANGLE3		= 1 << 3,
	U_FRAME8		= 1 << 4,		// frame is a byte
	U_EVENT			= 1 << 5,
	U_REMOVE		= 1 << 6,		// REMOVE this entity, don't add it
	U_MOREBITS1		= 1 << 7,		// read one additional byte

	// second byte
	U_NUMBER16		= 1 << 8,		// NUMBER8 is implicit if not set
	U_ORIGIN3		= 1 << 9,
	U_ANGLE1		= 1 << 10,
	U_MODEL			= 1 << 11,
	U_RENDERFX8		= 1 << 12,		// fullbright, etc
	U_EFFECTS8		= 1 << 14,		// autorotate, trails, etc
	U_MOREBITS2		= 1 << 15,		// read one additional byte

	// third byte
	U_SKIN8			= 1 << 16,
	U_FRAME16		= 1 << 17,		// frame is a short
	U_RENDERFX16	= 1 << 18,		// 8 + 16 = 32
	U_EFFECTS16		= 1 << 19,		// 8 + 16 = 32
	U_MODEL2		= 1 << 20,		// weapons, flags, etc
	U_MODEL3		= 1 << 21,
	U_MODEL4		= 1 << 22,
	U_MOREBITS3		= 1 << 23,		// read one additional byte

	// fourth byte
	U_OLDORIGIN		= 1 << 24,		// FIXME: get rid of this
	U_SKIN16		= 1 << 25,
	U_SOUND			= 1 << 26,
	U_SOLID			= 1 << 27
};

/*
=============================================================================

	MESSAGE FUNCTIONS

=============================================================================
*/

typedef struct {
	qBool		allowOverflow;	// if false, do a Com_Error
	qBool		overFlowed;		// set to true if the buffer size failed
	byte		*data;
	int			maxSize;
	int			curSize;
	int			readCount;
} netMsg_t;

// supporting functions
void	MSG_Init (netMsg_t *buf, byte *data, int length);
void	MSG_Clear (netMsg_t *buf);
void	*MSG_GetSpace (netMsg_t *buf, int length);
void	MSG_Write (netMsg_t *buf, void *data, int length);
void	MSG_Print (netMsg_t *buf, char *data);	// strcats onto the sizebuf

// writing
void	MSG_WriteChar (netMsg_t *msg_read, int c);
void	MSG_WriteByte (netMsg_t *msg_read, int c);
void	MSG_WriteShort (netMsg_t *msg_read, int c);
void	MSG_WriteLong (netMsg_t *msg_read, int c);
void	MSG_WriteFloat (netMsg_t *msg_read, float f);
void	MSG_WriteString (netMsg_t *msg_read, char *s);
void	MSG_WriteCoord (netMsg_t *msg_read, float f);
void	MSG_WritePos (netMsg_t *msg_read, vec3_t pos);
void	MSG_WriteAngle (netMsg_t *msg_read, float f);
void	MSG_WriteAngle16 (netMsg_t *msg_read, float f);
void	MSG_WriteDeltaUsercmd (netMsg_t *msg_read, struct userCmd_s *from, struct userCmd_s *cmd);
void	MSG_WriteDeltaEntity (entityState_t *from, entityState_t *to, netMsg_t *msg, qBool force, qBool newentity);
void	MSG_WriteDir (netMsg_t *msg_read, vec3_t vector);

// reading
void	MSG_BeginReading (netMsg_t *msg_read);
int		MSG_ReadChar (netMsg_t *msg_read);
int		MSG_ReadByte (netMsg_t *msg_read);
int		MSG_ReadShort (netMsg_t *msg_read);
int		MSG_ReadLong (netMsg_t *msg_read);
float	MSG_ReadFloat (netMsg_t *msg_read);
char	*MSG_ReadString (netMsg_t *msg_read);
char	*MSG_ReadStringLine (netMsg_t *msg_read);
float	MSG_ReadCoord (netMsg_t *msg_read);
void	MSG_ReadPos (netMsg_t *msg_read, vec3_t pos);
float	MSG_ReadAngle (netMsg_t *msg_read);
float	MSG_ReadAngle16 (netMsg_t *msg_read);
void	MSG_ReadDeltaUsercmd (netMsg_t *msg_read, struct userCmd_s *from, struct userCmd_s *cmd);
void	MSG_ReadDir (netMsg_t *msg_read, vec3_t vector);
void	MSG_ReadData (netMsg_t *msg_read, void *buffer, int size);

/*
=============================================================================

	ALIAS COMMANDS

=============================================================================
*/

#define	MAX_ALIAS_NAME		32
#define	MAX_ALIAS_LOOPS		16

typedef struct aliasCmd_s {
	struct		aliasCmd_s	*next;
	char		name[MAX_ALIAS_NAME];
	char		*value;

	int			compFrame;		// for tab completion
} aliasCmd_t;

extern aliasCmd_t	*aliasCmds;
extern int			aliasCount;			// for detecting runaway loops

aliasCmd_t *Alias_Exists (char *aliasName);
void	Alias_WriteAliases (FILE *f);

void	Alias_RemoveAlias (char *aliasName);
void	Alias_AddAlias (char *aliasName);

void	Alias_Init (void);
void	Alias_Shutdown (void);

/*
=============================================================================

	COMMANDS / COMMAND BUFFER

=============================================================================
*/

typedef struct cmdFunc_s {
	struct		cmdFunc_s	*next;
	char		*name;
	void		(*function) (void);
	char		*description;

	int			compFrame;			// for tab completion
} cmdFunc_t;

extern cmdFunc_t	*cmdFunctions;	// possible commands to execute
extern qBool		cmdWait;

void	Cbuf_Init (void);
void	Cbuf_Shutdown (void);

void	Cbuf_AddText (char *text);

void	Cbuf_InsertText (char *text);
void	Cbuf_Execute (void);

void	Cbuf_CopyToDefer (void);
void	Cbuf_InsertFromDefer (void);

// ==========================================================================

cmdFunc_t *Cmd_Exists (char *cmdName);

void	Cmd_AddCommand (char *cmdName, void (*function) (void), char *description);
void	Cmd_RemoveCommand (char *cmdName);

int		Cmd_Argc (void);
char	*Cmd_Argv (int arg);
char	*Cmd_Args (void);

void	Cmd_TokenizeString (char *text, qBool macroExpand);
void	Cmd_ExecuteString (char *text);
void	Cmd_ForwardToServer (void);

void	Cmd_Init (void);
void	Cmd_Shutdown (void);

/*
=============================================================================

	CVARS

	Console Variables
=============================================================================
*/

extern cVar_t	*cvarVariables;
extern qBool	cvarUserInfoModified;

char		*Cvar_Userinfo (void);
char		*Cvar_Serverinfo (void);

cVar_t		*Cvar_Get (char *varName, char *value, int flags);
void		Cvar_GetLatchedVars (int flags);

int			Cvar_VariableInteger (char *varName);
char		*Cvar_VariableString (char *varName);
float		Cvar_VariableValue (char *varName);

void		Cvar_WriteVariables (FILE *f);

// these take a pointer
cVar_t		*Cvar_VariableForceSet (cVar_t *var, char *value);
cVar_t		*Cvar_VariableForceSetValue (cVar_t *var, float value);
cVar_t		*Cvar_VariableFullSet (cVar_t *var, char *value, int flags);
cVar_t		*Cvar_VariableSet (cVar_t *var, char *value);
cVar_t		*Cvar_VariableSetValue (cVar_t *var, float value);

// these take a string (eww)
cVar_t		*Cvar_ForceSet (char *varName, char *value);
cVar_t		*Cvar_ForceSetValue (char *varName, float value);
cVar_t		*Cvar_FullSet (char *varName, char *value, int flags);
cVar_t		*Cvar_Set (char *varName, char *value);
cVar_t		*Cvar_SetValue (char *varName, float value);

qBool		Cvar_Command (void);

void		Cvar_Init (void);
void		Cvar_Shutdown (void);

/*
=============================================================================

	NET

=============================================================================
*/

#define	PORT_ANY		-1
#define	PORT_MASTER		27900
#define PORT_CLIENT		27901
#define	PORT_SERVER		27910

#define	MAX_MSGLEN		1400		// max length of a message

enum {
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX
};

enum {
	NS_CLIENT,
	NS_SERVER
};

typedef struct netAdr_s {
	int			naType;

	byte		ip[4];
	byte		ipx[10];

	uShort		port;
} netAdr_t;

void	NET_Init (void);
void	NET_Shutdown (void);

void	NET_Config (qBool multiplayer);

qBool	NET_GetPacket (int sock, netAdr_t *fromAddr, netMsg_t *message);
void	NET_SendPacket (int sock, int length, void *data, netAdr_t to);

qBool	NET_CompareAdr (netAdr_t a, netAdr_t b);
qBool	NET_CompareBaseAdr (netAdr_t a, netAdr_t b);
qBool	NET_IsLocalAddress (netAdr_t adr);
char	*NET_AdrToString (netAdr_t a);
qBool	NET_StringToAdr (char *s, netAdr_t *a);
void	NET_Sleep (int msec);

typedef struct netChan_s {
	qBool		fatalError;

	int			sock;

	int			dropped;			// between last packet and previous

	int			lastReceived;		// for timeouts
	int			lastSent;			// for retransmits

	netAdr_t	remoteAddress;
	int			qPort;				// qport value to write when transmitting

	// sequencing variables
	int			incomingSequence;
	int			incomingAcknowledged;
	int			incomingReliableAcknowledged;	// single bit
	int			incomingReliableSequence;		// single bit, maintained local

	int			outgoingSequence;
	int			reliableSequence;			// single bit
	int			lastReliableSequence;		// sequence number of last send

	// reliable staging and holding areas
	netMsg_t	message;		// writing buffer to send to server
	byte		messageBuff[MAX_MSGLEN-16];		// leave space for header

	// message is copied to this buffer when it is first transfered
	int			reliableLength;
	byte		reliableBuff[MAX_MSGLEN-16];	// unacked reliable message
} netChan_t;

extern	netAdr_t	netFrom;
extern	netMsg_t	netMessage;
extern	byte		netMessageBuffer[MAX_MSGLEN];

void	Netchan_Init (void);
void	Netchan_Setup (int sock, netChan_t *chan, netAdr_t adr, int qport);

qBool	Netchan_NeedReliable (netChan_t *chan);
void	Netchan_Transmit (netChan_t *chan, int length, byte *data);
void	Netchan_OutOfBand (int net_socket, netAdr_t adr, int length, byte *data);
void	Netchan_OutOfBandPrint (int net_socket, netAdr_t adr, char *format, ...);
qBool	Netchan_Process (netChan_t *chan, netMsg_t *msg);

qBool	Netchan_CanReliable (netChan_t *chan);

/*
=============================================================================

	CMODEL

=============================================================================
*/

cBspModel_t	*CM_LoadMap (char *name, qBool clientLoad, uInt *checksum);
void		CM_UnloadMap (void);
cBspModel_t	*CM_InlineModel (char *name);	// *1, *2, etc

// ==========================================================================

char		*CM_EntityString (void);
char		*CM_SurfRName (int texNum);

int			CM_NumClusters (void);
int			CM_NumInlineModels (void);
int			CM_NumTexInfo (void);

// ==========================================================================

int			CM_HeadnodeForBox (vec3_t mins, vec3_t maxs);	// creates a clipping hull for an arbitrary box

int			CM_PointContents (vec3_t p, int headNode);	// returns an ORed contents mask
int			CM_TransformedPointContents (vec3_t p, int headNode, vec3_t origin, vec3_t angles);

trace_t		CM_Trace (vec3_t start, vec3_t end, float size,  int contentMask);
trace_t		CM_BoxTrace (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs,  int headNode, int brushMask);
trace_t		CM_TransformedBoxTrace (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs, int headNode,
									int brushMask, vec3_t origin, vec3_t angles);

byte		*CM_ClusterPVS (int cluster);
byte		*CM_ClusterPHS (int cluster);

int			CM_PointLeafnum (vec3_t p);

// call with topnode set to the headnode, returns with topnode
// set to the first node that splits the box
int			CM_BoxLeafnums (vec3_t mins, vec3_t maxs, int *list, int listSize, int *topNode);

int			CM_LeafContents (int leafnum);
int			CM_LeafCluster (int leafnum);
int			CM_LeafArea (int leafnum);

void		CM_SetAreaPortalState (int portalNum, qBool open);
qBool		CM_AreasConnected (int area1, int area2);

int			CM_WriteAreaBits (byte *buffer, int area);
qBool		CM_HeadnodeVisible (int headNode, byte *visBits);

void		CM_WritePortalState (FILE *f);
void		CM_ReadPortalState (FILE *f);

/*
=============================================================================

	MISCELLANEOUS

=============================================================================
*/

extern cVar_t	*developer;
extern cVar_t	*dedicated;
extern cVar_t	*host_speeds;
extern cVar_t	*log_stats;

extern FILE		*logStatsFP;

// host_speeds times
extern int	timeBeforeGame;
extern int	timeAfterGame;
extern int	timeBeforeRefresh;
extern int	timeAfterRefresh;

// client/server interactions
void	Com_BeginRedirect (int target, char *buffer, int buffersize, void (*flush));
void	Com_EndRedirect (void);
void	Com_Printf (int flags, char *fmt, ...);
void	Com_Error (int code, char *fmt, ...);
void	Com_Quit (void);

// client/server states
enum {
	CA_UNINITIALIZED,	// initial state
	CA_DISCONNECTED,	// not talking to a server
	CA_CONNECTING,		// sending request packets to the server
	CA_CONNECTED,		// netChan_t established, waiting for svc_serverdata
	CA_ACTIVE			// game views should be displayed
};
int		Com_ClientState (void);
void	Com_SetClientState (int state);

enum {
	SS_DEAD,			// no map loaded
	SS_LOADING,			// spawning level edicts
	SS_GAME,			// actively running
	SS_CINEMATIC,		// playing a cinematic
	SS_DEMO,			// playing a demo
	SS_PIC				// just showing a pic
};
int		Com_ServerState (void);		// this should have just been a cvar...
void	Com_SetServerState (int state);

// initialization and processing
void	Com_Init (int argc, char **argv);
void	FASTCALL Com_Frame (int msec);
void	Com_Shutdown (void);

// crc and checksum
byte	Com_BlockSequenceCRCByte (byte *base, int length, int sequence);
uInt	Com_BlockChecksum (void *buffer, int length);

/*
==============================================================================

	MEMORY ALLOCATION

==============================================================================
*/

#define MEM_SENTINEL1	0xFEBDFAED
#define MEM_SENTINEL2	0xDFBA

typedef struct memChunk_s {
	struct		memChunk_s	*prev, *next;

	uInt		sentinel1;		// for memory integrity checking

	int			poolNum;		// each pool has it's own tags
	int			tagNum;			// for group free
	size_t		size;			// size of allocation including this header

	void		*memPointer;	// pointer to allocated memory
	size_t		memSize;		// size minus the header

	uInt		sentinel2;		// for memory integrity checking
} memChunk_t;

// these are private to the client
enum {
	MEMPOOL_GENERIC,

	MEMPOOL_FILESYS,
	MEMPOOL_IMAGESYS,
	MEMPOOL_MODELSYS,
	MEMPOOL_SHADERSYS,
	MEMPOOL_SOUNDSYS,

	MEMPOOL_GAMESYS,
	MEMPOOL_CGAMESYS,
	MEMPOOL_UISYS,

	MEMPOOL_MAX
};

// constants
#define Mem_Free(ptr)						_Mem_Free((ptr),__FILE__,__LINE__)
#define Mem_FreeTags(poolNum,tagNum)		_Mem_FreeTags((poolNum),(tagNum),__FILE__,__LINE__)
#define Mem_FreePool(poolNum)				_Mem_FreePool((poolNum),__FILE__,__LINE__)
#define Mem_Alloc(size)						_Mem_Alloc((size),__FILE__,__LINE__)
#define Mem_PoolAlloc(size,poolNum,tagNum)	_Mem_PoolAlloc((size),(poolNum),(tagNum),__FILE__,__LINE__)

#define Mem_StrDup(in)						_Mem_StrDup((in),__FILE__,__LINE__)
#define Mem_PoolStrDup(in,poolNum,tagNum)	_Mem_PoolStrDup((in),(poolNum),(tagNum),__FILE__,__LINE__)
#define Mem_PoolSize(poolNum)				_Mem_PoolSize((poolNum),__FILE__,__LINE__)
#define Mem_TagSize(poolNum,tagNum)			_Mem_TagSize((poolNum),(tagNum),__FILE__,__LINE__)

#define Mem_CheckPoolIntegrity(poolNum)		_Mem_CheckPoolIntegrity((poolNum),__FILE__,__LINE__)
#define Mem_CheckGlobalIntegrity()			_Mem_CheckGlobalIntegrity(__FILE__,__LINE__)

// functions
void	_Mem_Free (const void *ptr, const char *fileName, const int fileLine);
void	_Mem_FreeTags (const int poolNum, const int tagNum, const char *fileName, const int fileLine);
void	_Mem_FreePool (const int poolNum, const char *fileName, const int fileLine);
void	*_Mem_Alloc (const size_t size, const char *fileName, const int fileLine);
void	*_Mem_PoolAlloc (size_t size, const int poolNum, const int tagNum, const char *fileName, const int fileLine);

char	*_Mem_StrDup (const char *in, const char *fileName, const int fileLine);
char	*_Mem_PoolStrDup (const char *in, const int poolNum, const int tagNum, const char *fileName, const int fileLine);
int		_Mem_PoolSize (const int poolNum, const char *fileName, const int fileLine);
int		_Mem_TagSize (const int poolNum, const int tagNum, const char *fileName, const int fileLine);

void	_Mem_CheckPoolIntegrity (const int poolNum, const char *fileName, const int fileLine);
void	_Mem_CheckGlobalIntegrity (const char *fileName, const int fileLine);

void	Mem_Register (void);
void	Mem_Init (void);
void	Mem_Shutdown (void);

/*
==============================================================================

	NON-PORTABLE SYSTEM SERVICES

==============================================================================
*/

enum {
	LIB_CGAME,
	LIB_GAME,
	LIB_UI
};

extern	int	sysTime;		// time returned by last Sys_Milliseconds
int		Sys_Milliseconds (void);

void	Sys_Init (void);
void	Sys_AppActivate (void);

void	Sys_UnloadLibrary (int libType);
void	*Sys_LoadLibrary (int libType, void *parms);

char	*Sys_ConsoleInput (void);
void	Sys_ConsoleOutput (char *string);

void	Sys_SendKeyEvents (void);
void	Sys_Error (char *error, ...);
void	Sys_Quit (void);
char	*Sys_GetClipboardData (void);

void	Sys_Mkdir (char *path);

// pass in an attribute mask of things you wish to REJECT
char	*Sys_FindFirst (char *path, uInt mustHave, uInt cantHave);
char	*Sys_FindNext (uInt mustHave, uInt cantHave);
void	Sys_FindClose (void);

/*
==============================================================

	CLIENT / SERVER SYSTEMS

==============================================================
*/

//
// console.c
//

void	Con_Init (void);
void	Con_Print (int flags, const char *text);

//
// cl_cgapi.c
//

qBool	CL_CGModule_Pmove (pMove_t *pMove, float airAcceleration);

//
// cl_main.c
//

void	CL_Drop (void);
void	FASTCALL CL_Frame (int msec);
void	CL_Init (void);
void	CL_Shutdown (void);

//
// cl_screen.c
//

void	SCR_BeginLoadingPlaque (void);
void	SCR_EndLoadingPlaque (void);

// this is in the client code, but can be used for debugging from server
void	SCR_DebugGraph (float value, int color);

//
// cl_uiapi.c
//

void	CL_UIModule_MainMenu (void);

//
// keys.c
//

void	Key_WriteBindings (FILE *f);

char	*Key_GetBindingBuf (int binding);
qBool	Key_IsDown (int keyNum);

void	Key_Init (void);
void	Key_Shutdown (void);

//
// sv_main.c
//

void	SV_Init (void);
void	SV_Shutdown (char *finalmsg, qBool reconnect);
void	SV_Frame (int msec);

#endif // __COMMON_H__