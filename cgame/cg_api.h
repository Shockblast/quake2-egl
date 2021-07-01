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
// cg_api.h
//

#ifndef __CGAMEAPI_H__
#define __CGAMEAPI_H__

#include "cgref.h"

#define CGAME_APIVERSION	6

#define	MAX_PARSE_ENTITIES			1024
#define MAX_PARSEENTITIES_MASK		(MAX_PARSE_ENTITIES-1)
#define MAX_CLIENTWEAPONMODELS		20		// PGM -- upped from 16 to fit the chainfist vwep

// ==========================================================================

typedef struct frame_s {
	qBool				valid;			// cleared if delta parsing was invalid
	int					serverFrame;
	int					serverTime;		// server time the message is valid for (in msec)
	int					deltaFrame;
	byte				areaBits[BSP_MAX_AREAS/8];		// portalarea visibility bits
	playerStateNew_t	playerState;
	int					numEntities;
	int					parseEntities;	// non-masked index into cg_parseEntities array
} frame_t;

// ==========================================================================

typedef struct cgExport_s {
	int			apiVersion;

	void		(*Init) (int playerNum, int serverProtocol, qBool attractLoop, int vidWidth, int vidHeight);
	void		(*Shutdown) (void);

	void		(*BeginFrameSequence) (frame_t frame);
	void		(*EndFrameSequence) (int numEntities);
	void		(*NewPacketEntityState) (int entnum, entityState_t state);
	// the sound code makes callbacks to the client for entitiy position
	// information, so entities can be dynamically re-spatialized
	void		(*GetEntitySoundOrigin) (int entNum, vec3_t org);

	void		(*ParseConfigString) (int num, char *str);

	void		(*StartServerMessage) (void);
	qBool		(*ParseServerMessage) (int command);
	void		(*EndServerMessage) (int realTime);

	void		(*Pmove) (pMoveNew_t *pmove, float airAcceleration);

	void		(*RegisterSounds) (void);

	void		(*RenderView) (int vidWidth, int vidHeight, int realTime, float netFrameTime, float refreshFrameTime, float stereoSeparation, qBool forceRefresh);
} cgExport_t;

typedef struct cgImport_s {
	void		(*Cbuf_AddText) (char *text);
	void		(*Cbuf_Execute) (void);
	void		(*Cbuf_ExecuteString) (char *text);
	void		(*Cbuf_InsertText) (char *text);

	trace_t		(*CM_BoxTrace) (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs, int headnode, int brushmask);
	int			(*CM_HeadnodeForBox) (vec3_t mins, vec3_t maxs);
	cBspModel_t	*(*CM_InlineModel) (char *name);
	int			(*CM_PointContents) (vec3_t p, int headnode);
	trace_t		(*CM_Trace) (vec3_t start, vec3_t end, float size, int contentmask);
	trace_t		(*CM_TransformedBoxTrace) (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs,
						  int headnode, int brushmask, vec3_t origin, vec3_t angles);
	int			(*CM_TransformedPointContents) (vec3_t p, int headnode, vec3_t origin, vec3_t angles);

	cmdFunc_t	*(*Cmd_AddCommand) (char *cmdName, void (*function) (void), char *description);
	void		(*Cmd_RemoveCommand) (char *cmdName, cmdFunc_t *command);
	int			(*Cmd_Argc) (void);
	char		*(*Cmd_Args) (void);
	char		*(*Cmd_Argv) (int arg);

	void		(*Com_Error) (int code, char *fmt, ...);
	void		(*Com_Printf) (int flags, char *fmt, ...);
	int			(*Com_ServerState) (void);

	cVar_t		*(*Cvar_Get) (char *name, char *value, int flags);

	int			(*Cvar_VariableInteger) (char *varName);
	char		*(*Cvar_VariableString) (char *varName);
	float		(*Cvar_VariableValue) (char *varName);

	cVar_t		*(*Cvar_VariableForceSet) (cVar_t *var, char *value);
	cVar_t		*(*Cvar_VariableForceSetValue) (cVar_t *var, float value);

	cVar_t		*(*Cvar_ForceSet) (char *varName, char *value);
	cVar_t		*(*Cvar_ForceSetValue) (char *varName, float value);
	cVar_t		*(*Cvar_Set) (char *name, char *value);
	cVar_t		*(*Cvar_SetValue) (char *name, float value);

	qBool		(*FS_CurrGame) (char *name);
	void		(*FS_FreeFileList) (char **list, int num, const char *fileName, const int fileLine);
	char		*(*FS_Gamedir) (void);
	int			(*FS_FindFiles) (char *path, char *filter, char *extension, char **fileList, int maxFiles, qBool addGameDir, qBool recurse);
	int			(*FS_LoadFile) (char *path, void **buffer);
	void		(*FS_FreeFile) (void *buffer, const char *fileName, const int fileLine);
	char		*(*FS_NextPath) (char *prevPath);
	int			(*FS_Read) (void *buffer, int len, int fileNum);
	int			(*FS_Write) (void *buffer, int size, int fileNum);
	void		(*FS_Seek) (int fileNum, int offset, int seekOrigin);
	int			(*FS_OpenFile) (char *fileName, int *fileNum, int mode);
	void		(*FS_CloseFile) (int fileNum);

	void		(*GetConfigString) (int i, char *str, int size);

	char		*(*Key_GetBindingBuf) (int binding);
	qBool		(*Key_IsDown) (int keyNum);
	void		(*Key_ClearStates) (void);
	qBool		(*Key_KeyDest) (int keyDest);
	char		*(*Key_KeynumToString) (int keyNum);
	void		(*Key_SetBinding) (int keyNum, char *binding);
	void		(*Key_SetKeyDest) (int keyDest);

	void		*(*Mem_Alloc) (size_t size, char *fileName, int fileLine);
	void		(*Mem_Free) (const void *ptr, char *fileName, int fileLine);

	int			(*MSG_ReadChar) (void);
	int			(*MSG_ReadByte) (void);
	int			(*MSG_ReadShort) (void);
	int			(*MSG_ReadLong) (void);
	float		(*MSG_ReadFloat) (void);
	void		(*MSG_ReadDir) (vec3_t dir);
	void		(*MSG_ReadPos) (vec3_t pos);
	char		*(*MSG_ReadString) (void);

	int			(*NET_GetCurrentUserCmdNum) (void);
	void		(*NET_GetSequenceState) (int *outgoingSequence, int *incomingAcknowledged);
	void		(*NET_GetUserCmd) (int frame, userCmd_t *cmd);
	int			(*NET_GetUserCmdTime) (int frame);

	void		(*R_AddEntity) (entity_t *ent);
	void		(*R_AddPoly) (poly_t *poly);
	void		(*R_AddLight) (vec3_t org, float intensity, float r, float g, float b);
	void		(*R_AddLightStyle) (int style, float r, float g, float b);

	void		(*R_ClearScene) (void);

	qBool		(*R_CullBox) (vec3_t mins, vec3_t maxs, int clipFlags);
	qBool		(*R_CullSphere) (const vec3_t origin, const float radius, int clipFlags);
	qBool		(*R_CullVisNode) (struct mBspNode_s *node);

	void		(*R_DrawFill) (float x, float y, int w, int h, vec4_t color);
	void		(*R_DrawPic) (struct shader_s *shader, float x, float y, int w, int h, float s1, float t1, float s2, float t2, vec4_t color);

	uInt		(*R_GetClippedFragments) (vec3_t origin, float radius, vec3_t axis[3], int maxfverts, vec3_t *fverts, int maxfragments, fragment_t *fragments);
	void		(*R_GetImageSize) (struct shader_s *shader, int *width, int *height);

	void		(*R_RegisterMap) (char *mapName);
	struct		model_s *(*R_RegisterModel) (char *name);
	void		(*R_ModelBounds) (struct model_s *model, vec3_t mins, vec3_t maxs);

	void		(*R_UpdateScreen) (void);
	void		(*R_RenderScene) (refDef_t *rd);
	void		(*R_BeginFrame) (float cameraSeparation);
	void		(*R_EndFrame) (void);

	struct shader_s *(*R_RegisterPic) (char *name);
	struct shader_s *(*R_RegisterPoly) (char *name);
	struct shader_s *(*R_RegisterSkin) (char *name);
	struct shader_s *(*R_RegisterTexture) (char *name, int surfParams);

	void		(*R_LightPoint) (vec3_t point, vec3_t light);
	void		(*R_TransformToScreen) (vec3_t in, vec2_t out);
	void		(*R_SetSky) (char *name, float rotate, vec3_t axis);

	struct sfx_s *(*Snd_RegisterSound) (char *sample);
	void		(*Snd_StartLocalSound) (struct sfx_s *sfx);
	void		(*Snd_StartSound) (vec3_t origin, int entNum, int entChannel, struct sfx_s *sfx, float volume, float attenuation, float timeOffset);
	void		(*Snd_Update) (vec3_t viewOrigin, vec3_t forward, vec3_t right, vec3_t up, qBool underWater);

	void		(*Sys_FindClose) (void);
	char		*(*Sys_FindFirst) (char *path, uInt mustHave, uInt cantHave);
	char		*(*Sys_GetClipboardData) (void);
	int			(*Sys_Milliseconds) (void);
	void		(*Sys_SendKeyEvents) (void);
} cgImport_t;

typedef cgExport_t (*GetCGameAPI_t) (cgImport_t);

#endif // __CGAMEAPI_H__
