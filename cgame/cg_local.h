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
// cg_local.h
// CGame local header
//

#ifndef __CGAME_LOCAL_H__
#define __CGAME_LOCAL_H__

#include "../shared/shared.h"
#include "../ui/keycodes.h"
#include "cg_api.h"

extern cgImport_t cgi;

#define HUD_SCALE		(r_hudscale->value)
#define HUD_FTSIZE		((float)(8.0f * HUD_SCALE))

/*
=============================================================================

	API

=============================================================================
*/

#define CG_MemAlloc(size) cgi.Mem_Alloc((size),__FILE__,__LINE__)
#define CG_MemFree(ptr) cgi.Mem_Free((ptr),__FILE__,__LINE__)

#define CG_FS_FreeFile(buffer) cgi.FS_FreeFile((buffer),__FILE__,__LINE__)
#define CG_FS_FreeFileList(list,num) cgi.FS_FreeFileList((list),(num),__FILE__,__LINE__)

// ==========================================================================

void CGI_Cbuf_AddText (char *text);
void CGI_Cbuf_Execute (void);
void CGI_Cbuf_ExecuteString (char *text);
void CGI_Cbuf_InsertText (char *text);

trace_t CGI_CM_BoxTrace (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs, int headNode, int brushMask);
int CGI_CM_HeadnodeForBox (vec3_t mins, vec3_t maxs);
cBspModel_t *CGI_CM_InlineModel (char *name);
int CGI_CM_PointContents (vec3_t p, int headNode);
trace_t CGI_CM_Trace (vec3_t start, vec3_t end, float size, int contentMask);
trace_t CGI_CM_TransformedBoxTrace (vec3_t start, vec3_t end, vec3_t mins, vec3_t maxs,
						int headNode, int brushMask, vec3_t origin, vec3_t angles);
int CGI_CM_TransformedPointContents (vec3_t p, int headNode, vec3_t origin, vec3_t angles);

cmdFunc_t *CGI_Cmd_AddCommand (char *cmdName, void (*function) (void), char *description);
void CGI_Cmd_RemoveCommand (char *cmdName, cmdFunc_t *command);
int CGI_Cmd_Argc (void);
char *CGI_Cmd_Args (void);
char *CGI_Cmd_Argv (int arg);

int CGI_Com_ServerState (void);

/*
=============================================================================

	CGAME STATE

=============================================================================
*/

typedef struct clientInfo_s {
	char				name[MAX_QPATH];
	char				cInfo[MAX_QPATH];
	struct shader_s		*skin;
	struct shader_s		*icon;
	char				iconName[MAX_QPATH];
	struct model_s		*model;
	struct model_s		*weaponModel[MAX_CLIENTWEAPONMODELS];
} clientInfo_t;

typedef struct cgPredictionState_s {
	float				step;						// for stair up smoothing
	uInt				stepTime;
	short				origins[CMD_BACKUP][3];		// for debug comparing against server
	vec3_t				origin;						// generated by CG_PredictMovement
	vec3_t				angles;
	vec3_t				velocity;
	vec3_t				error;
} cgPredictionState_t;

// ==========================================================================

typedef struct cgState_s {
	qBool				initialized;						// true if cgame was initialized

	// time
	int					netTime;
	int					refreshTime;

	float				lerpFrac;							// between oldFrame and frame

	refDef_t			refDef;
	vec3_t				forwardVec, rightVec, upVec;		// set when refdef.angles is set

	// gloom prediction hacking
	qBool				gloomCheckClass;
	int					gloomClassType;

	// third person camera
	qBool				thirdPerson;
	byte				cameraTrans;

	// the client maintains its own idea of view angles, which are
	// sent to the server each frame. It is cleared to 0 upon entering each level.
	// the server sends a delta each frame which is added to the locally
	// tracked view angles to account for standing on rotating objects,
	// and teleport direction changes
	vec3_t				viewAngles;

	cgPredictionState_t	predicted;

	frame_t				frame;								// received from server
	frame_t				oldFrame;

	qBool				attractLoop;

	int					playerNum;

	char				configStrings[MAX_CONFIGSTRINGS][MAX_QPATH];

	//
	// loading screen
	//
	float				loadingPercent;

	char				loadingString[MAX_QPATH];
	char				loadFileName[MAX_QPATH];

	//
	// video dimensions
	//
	int					vidWidth;
	int					vidHeight;

	//
	// hud and inventory
	//
	char				layout[1024];				// general 2D overlay
	int					inventory[MAX_CS_ITEMS];

	//
	// locally derived information from server state
	//
	struct model_s		*modelDraw[MAX_CS_MODELS];
	struct cBspModel_s	*modelClip[MAX_CS_MODELS];

	clientInfo_t		clientInfo[MAX_CS_CLIENTS];
	clientInfo_t		baseClientInfo;
} cgState_t;

extern cgState_t	cg;

/*
=============================================================================

	CONNECTION

=============================================================================
*/

enum {
	GAME_MOD_DEFAULT,
	GAME_MOD_DDAY,
	GAME_MOD_GLOOM,
	GAME_MOD_ROGUE,
	GAME_MOD_XATRIX
};

typedef struct cgStatic_s {
	int				serverProtocol;

	uLong			frameCount;

	float			netFrameTime;
	float			refreshFrameTime;

	int				realTime;

	int				currGameMod;
} cgStatic_t;

extern cgStatic_t	cgs;

/*
=============================================================================

	LIGHTING

=============================================================================
*/

typedef struct cgDlight_s {
	vec3_t		origin;
	vec3_t		color;

	int			key;				// so entities can reuse same entry

	float		radius;
	float		die;				// stop lighting after this time
	float		decay;				// drop this each second
	float		minlight;			// don't add when contributing less
} cgDLight_t;

//
// cg_light.c
//

void	CG_ClearLightStyles (void);
cgDLight_t *CG_AllocDLight (int key);
void	CG_RunLightStyles (void);
void	CG_SetLightstyle (int num);
void	CG_AddLightStyles (void);

void	CG_ClearDLights (void);
void	CG_RunDLights (void);
void	CG_AddDLights (void);

void	CG_Flashlight (int ent, vec3_t pos);
void	__fastcall CG_ColorFlash (vec3_t pos, int ent, float intensity, float r, float g, float b);
void	CG_WeldingSparkFlash (vec3_t pos);

/*
=============================================================================

	ENTITY

=============================================================================
*/

typedef struct cgEntity_s {
	entityState_t	baseLine;		// delta from this if not from a previous frame
	entityState_t	current;
	entityState_t	prev;			// will always be valid, but might just be a copy of current

	int				serverFrame;		// if not current, this ent isn't in the frame

	vec3_t			lerpOrigin;		// for trails (variable hz)

	int				flyStopTime;

	qBool			muzzleOn;
	int				muzzType;
	qBool			muzzSilenced;
	qBool			muzzVWeap;
} cgEntity_t;

extern cgEntity_t		cg_entityList[MAX_CS_EDICTS];

// the cg_parseEntities must be large enough to hold UPDATE_BACKUP frames of
// entities, so that when a delta compressed message arives from the server
// it can be un-deltad from the original
extern entityState_t	cg_parseEntities[MAX_PARSE_ENTITIES];

//
// cg_entities.c
//

void	CG_BeginFrameSequence (frame_t frame);
void	CG_NewPacketEntityState (int entnum, entityState_t state);
void	CG_EndFrameSequence (int numEntities);

void	CG_AddEntities (void);
void	CG_ClearEntities (void);

void	CG_GetEntitySoundOrigin (int entNum, vec3_t org);

//
// cg_localents.c
//

void	CG_ClearLocalEnts (void);
void	CG_AddLocalEnts (void);

//
// cg_tempents.c
//

void	CG_ExploRattle (vec3_t org, float scale);

void	CG_AddTempEnts (void);
void	CG_ClearTempEnts (void);
void	CG_ParseTempEnt (void);

//
// cg_weapon.c
//

void	CG_AddViewWeapon (void);

void	CG_WeapRegister (void);
void	CG_WeapUnregister (void);

/*
=============================================================================

	PLAYER MOVEMENT

=============================================================================
*/

enum {
	GLM_DEFAULT,

	GLM_OBSERVER,

	GLM_BREEDER,
	GLM_HATCHLING,
	GLM_DRONE,
	GLM_WRAITH,
	GLM_KAMIKAZE,
	GLM_STINGER,
	GLM_GUARDIAN,
	GLM_STALKER,

	GLM_ENGINEER,
	GLM_GRUNT,
	GLM_ST,
	GLM_BIOTECH,
	GLM_HT,
	GLM_COMMANDO,
	GLM_EXTERM,
	GLM_MECH
};

//
// pmove.c
// common between the client and server for consistancy
//
void	Pmove (pMoveNew_t *pMove, float airAcceleration);

//
// cg_predict.c
//
void	CG_CheckPredictionError (void);

trace_t	CG_PMTrace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, qBool entities);

void	CG_PredictMovement (void);

/*
=============================================================================

	EFFECTS

=============================================================================
*/

enum {
	// ----------- PARTICLES ----------
	PT_BFG_DOT,

	PT_BLASTER_BLUE,
	PT_BLASTER_GREEN,
	PT_BLASTER_RED,

	PT_IONTAIL,
	PT_IONTIP,
	PT_ITEMRESPAWN,
	PT_ENGYREPAIR_DOT,
	PT_PHALANXTIP,

	PT_GENERIC,
	PT_GENERIC_GLOW,

	PT_SMOKE,
	PT_SMOKE2,

	PT_SMOKEGLOW,
	PT_SMOKEGLOW2,

	PT_BLUEFIRE,
	PT_FIRE1,
	PT_FIRE2,
	PT_FIRE3,
	PT_FIRE4,
	PT_EMBERS1,
	PT_EMBERS2,
	PT_EMBERS3,

	PT_BLOOD,
	PT_BLOOD2,
	PT_BLOOD3,
	PT_BLOOD4,
	PT_BLOOD5,
	PT_BLOOD6,

	PT_GRNBLOOD,
	PT_GRNBLOOD2,
	PT_GRNBLOOD3,
	PT_GRNBLOOD4,
	PT_GRNBLOOD5,
	PT_GRNBLOOD6,

	PT_BEAM,

	PT_BLDDRIP,
	PT_BLDDRIP_GRN,
	PT_BLDSPURT,
	PT_BLDSPURT2,

	PT_EXPLOFLASH,
	PT_EXPLOWAVE,

	PT_FLARE,
	PT_FLAREGLOW,

	PT_FLY,

	PT_RAIL_CORE,
	PT_RAIL_WAVE,
	PT_SPIRAL,

	PT_SPARK,

	PT_WATERBUBBLE,
	PT_WATERDROPLET,
	PT_WATERIMPACT,
	PT_WATERMIST,
	PT_WATERMIST_GLOW,
	PT_WATERPLUME,
	PT_WATERPLUME_GLOW,
	PT_WATERRING,
	PT_WATERRIPPLE,
	// ----------- ANIMATED -----------
	PT_EXPLO1,
	PT_EXPLO2,
	PT_EXPLO3,
	PT_EXPLO4,
	PT_EXPLO5,
	PT_EXPLO6,
	PT_EXPLO7,

	PT_EXPLOEMBERS1,
	PT_EXPLOEMBERS2,

	// ------------- MAPFX ------------
	MFX_WHITE,
	MFX_CORONA,

	// ------------- TOTAL ------------
	PT_PICTOTAL
};

enum {
	// ------------ DECALS ------------
	DT_BFG_GLOWMARK,

	DT_BLASTER_BLUEMARK,
	DT_BLASTER_BURNMARK,
	DT_BLASTER_GREENMARK,
	DT_BLASTER_REDMARK,

	DT_DRONE_SPIT_GLOW,

	DT_ENGYREPAIR_BURNMARK,
	DT_ENGYREPAIR_GLOWMARK,

	DT_BLOOD,
	DT_BLOOD2,
	DT_BLOOD3,
	DT_BLOOD4,
	DT_BLOOD5,
	DT_BLOOD6,

	DT_GRNBLOOD,
	DT_GRNBLOOD2,
	DT_GRNBLOOD3,
	DT_GRNBLOOD4,
	DT_GRNBLOOD5,
	DT_GRNBLOOD6,

	DT_BULLET,

	DT_EXPLOMARK,
	DT_EXPLOMARK2,
	DT_EXPLOMARK3,

	DT_RAIL_BURNMARK,
	DT_RAIL_GLOWMARK,
	DT_RAIL_WHITE,

	DT_SLASH,
	DT_SLASH2,
	DT_SLASH3,
	// ------------- TOTAL ------------
	DT_PICTOTAL
};

extern vec3_t	avelocities[NUMVERTEXNORMALS];

/*
=============================================================================

	SCRIPTED MAP EFFECTS

=============================================================================
*/

void	CG_AddMapFX (void);
void	CG_ClearMapFX (void);

void	CG_MapFXInit (char *mapName);
void	CG_ShutdownMapFX (void);

/*
=============================================================================

	CGAME MEDIA

=============================================================================
*/

// surface-specific step sounds
typedef struct cgStepMedia_s {
	struct sfx_s	*standard[4];

	struct sfx_s	*concrete[4];
	struct sfx_s	*dirt[4];
	struct sfx_s	*duct[4];
	struct sfx_s	*grass[4];
	struct sfx_s	*gravel[4];
	struct sfx_s	*metal[4];
	struct sfx_s	*metalGrate[4];
	struct sfx_s	*metalLadder[4];
	struct sfx_s	*mud[4];
	struct sfx_s	*sand[4];
	struct sfx_s	*slosh[4];
	struct sfx_s	*snow[6];
	struct sfx_s	*tile[4];
	struct sfx_s	*wade[4];
	struct sfx_s	*wood[4];
	struct sfx_s	*woodPanel[4];
} cgStepMedia_t;

// muzzle flash sounds
typedef struct cgMzMedia_s {
	struct sfx_s	*bfgFireSfx;
	struct sfx_s	*blasterFireSfx;
	struct sfx_s	*etfRifleFireSfx;
	struct sfx_s	*grenadeFireSfx;
	struct sfx_s	*grenadeReloadSfx;
	struct sfx_s	*hyperBlasterFireSfx;
	struct sfx_s	*ionRipperFireSfx;
	struct sfx_s	*machineGunSfx[5];
	struct sfx_s	*phalanxFireSfx;
	struct sfx_s	*railgunFireSfx;
	struct sfx_s	*railgunReloadSfx;
	struct sfx_s	*rocketFireSfx;
	struct sfx_s	*rocketReloadSfx;
	struct sfx_s	*shotgunFireSfx;
	struct sfx_s	*shotgun2FireSfx;
	struct sfx_s	*shotgunReloadSfx;
	struct sfx_s	*superShotgunFireSfx;
	struct sfx_s	*trackerFireSfx;
} cgMzMedia_t;

// monster muzzle flash sounds
typedef struct cgMz2Media_s {
	struct sfx_s	*chicRocketSfx;
	struct sfx_s	*floatBlasterSfx;
	struct sfx_s	*flyerBlasterSfx;
	struct sfx_s	*gunnerGrenadeSfx;
	struct sfx_s	*gunnerMachGunSfx;
	struct sfx_s	*hoverBlasterSfx;
	struct sfx_s	*jorgMachGunSfx;
	struct sfx_s	*machGunSfx;
	struct sfx_s	*makronBlasterSfx;
	struct sfx_s	*medicBlasterSfx;
	struct sfx_s	*soldierBlasterSfx;
	struct sfx_s	*soldierMachGunSfx;
	struct sfx_s	*soldierShotgunSfx;
	struct sfx_s	*superTankRocketSfx;
	struct sfx_s	*tankBlasterSfx;
	struct sfx_s	*tankMachGunSfx[5];
	struct sfx_s	*tankRocketSfx;
} cgMz2Media_t;

// ==========================================================================

typedef struct cgMedia_s {
	qBool				initialized;
	qBool				baseInitialized;
	qBool				loadScreenPrepped;

	// engine generated textures
	struct shader_s		*noTexture;
	struct shader_s		*whiteTexture;
	struct shader_s		*blackTexture;

	// console characters
	struct shader_s		*conCharsShader;

	// load screen images
	struct shader_s		*loadSplash;
	struct shader_s		*loadBarPos;
	struct shader_s		*loadBarNeg;
	struct shader_s		*loadNoMapShot;
	struct shader_s		*loadMapShot;

	// screen shaders
	struct shader_s		*alienInfraredVision;
	struct shader_s		*infraredGoggles;

	// sounds
	cgStepMedia_t		steps;

	struct sfx_s		*ricochetSfx[3];
	struct sfx_s		*sparkSfx[7];

	struct sfx_s		*disruptExploSfx;
	struct sfx_s		*grenadeExploSfx;
	struct sfx_s		*rocketExploSfx;
	struct sfx_s		*waterExploSfx;

	struct sfx_s		*gibSfx;
	struct sfx_s		*gibSplatSfx[3];

	struct sfx_s		*itemRespawnSfx;
	struct sfx_s		*laserHitSfx;
	struct sfx_s		*lightningSfx;

	struct sfx_s		*playerFallSfx;
	struct sfx_s		*playerFallShortSfx;
	struct sfx_s		*playerFallFarSfx;

	struct sfx_s		*playerTeleport;
	struct sfx_s		*bigTeleport;

	// muzzleflash sounds
	cgMzMedia_t			mz;
	cgMz2Media_t		mz2;

	// models
	struct model_s		*parasiteSegmentMOD;
	struct model_s		*grappleCableMOD;

	struct model_s		*lightningMOD;
	struct model_s		*heatBeamMOD;
	struct model_s		*monsterHeatBeamMOD;

	struct model_s		*maleDisguiseModel;
	struct model_s		*femaleDisguiseModel;
	struct model_s		*cyborgDisguiseModel;

	// skins
	struct shader_s		*maleDisguiseSkin;
	struct shader_s		*femaleDisguiseSkin;
	struct shader_s		*cyborgDisguiseSkin;

	struct shader_s		*modelShellGod;
	struct shader_s		*modelShellHalfDam;
	struct shader_s		*modelShellDouble;
	struct shader_s		*modelShellRed;
	struct shader_s		*modelShellGreen;
	struct shader_s		*modelShellBlue;

	// images
	struct shader_s		*crosshairShader;

	struct shader_s		*tileBackShader;

	struct shader_s		*hudFieldShader;
	struct shader_s		*hudInventoryShader;
	struct shader_s		*hudNetShader;
	struct shader_s		*hudNumShaders[2][11];
	struct shader_s		*hudPausedShader;

	// particle/decal media
	struct shader_s		*decalTable[DT_PICTOTAL];
	struct shader_s		*particleTable[PT_PICTOTAL];
} cgMedia_t;

extern cgMedia_t	cgMedia;

/*
=============================================================================

	DECAL SYSTEM

=============================================================================
*/

typedef struct decal_s {
	struct		decal_s *next;

	float		time;

	int			numVerts;
	bvec4_t		*colors;
	vec_t		*coords;
	vec_t		*vertices;

	struct		mBspNode_s *node;

	vec3_t		org;
	vec3_t		dir;

	vec4_t		color;
	vec4_t		colorVel;

	float		size;

	float		lifeTime;

	struct		shader_s *shader;
	int			flags;

	void		(*think)(struct decal_s *d, vec4_t color, int *type, int *flags);
	qBool		thinkNext;

	float		angle;
} decal_t;

enum {
	DF_NOTIMESCALE	= 1 << 7,
	DF_ALPHACOLOR	= 1 << 8,

	DF_AIRONLY		= 1 << 9,
	DF_LAVAONLY		= 1 << 10,
	DF_SLIMEONLY	= 1 << 11,
	DF_WATERONLY	= 1 << 12
};

decal_t	*makeDecal (float org0,					float org1,					float org2,
					float dir0,					float dir1,					float dir2,
					float red,					float green,				float blue,
					float redVel,				float greenVel,				float blueVel,
					float alpha,				float alphaVel,
					float size,
					int type,					int flags,
					void (*think)(struct decal_s *d, vec4_t color, int *type, int *flags),
					qBool thinkNext,
					float lifeTime,				 float angle);

// constants
#define DECAL_GLOWTIME	( cl_decal_burnlife->value )
#define DECAL_INSTANT	-10000.0f

// random texturing
int dRandBloodMark (void);
int dRandGrnBloodMark (void);
int dRandExploMark (void);
int dRandSlashMark (void);

// management
void	CG_ClearDecals (void);
void	CG_AddDecals (void);

/*
=============================================================================

	PARTICLE SYSTEM

=============================================================================
*/

enum {
	PF_SCALED		= 1 << 0,

	PF_BEAM			= 1 << 1,
	PF_DIRECTION	= 1 << 2,
	PF_ANGLED		= 1 << 3
};

#define MAX_PARTICLE_VERTS		4

typedef struct particle_s {
	struct		particle_s *next;

	float		time;

	vec3_t		org;
	vec3_t		oldOrigin;

	vec3_t		angle;
	vec3_t		vel;
	vec3_t		accel;

	vec4_t		color;
	vec4_t		colorVel;

	float		size;
	float		sizeVel;

	struct		shader_s *shader;
	int			flags;

	float		orient;

	void		(*think)(struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
	qBool		thinkNext;

	// Passed to refresh
	bvec4_t		outColor[MAX_PARTICLE_VERTS];
	vec2_t		outCoords[MAX_PARTICLE_VERTS];
	vec3_t		outVertices[MAX_PARTICLE_VERTS];
} particle_t;

enum {
	PF_SHADE		= 1 << 5,
	PF_GRAVITY		= 1 << 6,
	PF_NOCLOSECULL	= 1 << 7,
	PF_NODECAL		= 1 << 8,
	PF_NOSFX		= 1 << 9,
	PF_ALPHACOLOR	= 1 << 10,

	PF_AIRONLY		= 1 << 11,
	PF_LAVAONLY		= 1 << 12,
	PF_SLIMEONLY	= 1 << 13,
	PF_WATERONLY	= 1 << 14
};

void __fastcall makePart (float org0,					float org1,					float org2,
						float angle0,					float angle1,				float angle2,
						float vel0,						float vel1,					float vel2,
						float accel0,					float accel1,				float accel2,
						float red,						float green,				float blue,
						float redVel,					float greenVel,				float blueVel,
						float alpha,					float alphaVel,
						float size,						float sizeVel,
						int type,						int flags,
						void (*think)(struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time),
						qBool thinkNext,
						float orient);

// constants
#define PMAXBLDDRIPLEN	3.25f
#define PMAXSPLASHLEN	2.0f

#define	PART_GRAVITY	110
#define PART_INSTANT	-1000.0f

#define	BEAMLENGTH		16

// random texturing
int pRandBloodMark (void);
int pRandGrnBloodMark (void);
int pRandSmoke (void);
int pRandGlowSmoke (void);
int pRandEmbers (void);
int pRandFire (void);

// management
qBool	pDegree (vec3_t org, qBool lod);
float	__fastcall pDecDegree (vec3_t org, float dec, float scale, qBool lod);
float	__fastcall pIncDegree (vec3_t org, float inc, float scale, qBool lod);

void	CG_ClearParticles (void);
void	CG_AddParticles (void);

//
// GENERIC EFFECTS
//

void	CG_BlasterBlueParticles (vec3_t org, vec3_t dir);
void	CG_BlasterGoldParticles (vec3_t org, vec3_t dir);
void	CG_BlasterGreenParticles (vec3_t org, vec3_t dir);
void	CG_BlasterGreyParticles (vec3_t org, vec3_t dir);
void	CG_BleedEffect (vec3_t org, vec3_t dir, int count);
void	CG_BleedGreenEffect (vec3_t org, vec3_t dir, int count);
void	CG_BubbleEffect (vec3_t origin);
void	CG_ExplosionBFGEffect (vec3_t org);
void	__fastcall CG_FlareEffect (vec3_t origin, int type, float orient, float size, float sizevel, int color, int colorvel, float alpha, float alphavel);
void	CG_ItemRespawnEffect (vec3_t org);
void	CG_LogoutEffect (vec3_t org, int type);

void	__fastcall CG_ParticleEffect (vec3_t org, vec3_t dir, int color, int count);
void	__fastcall CG_ParticleEffect2 (vec3_t org, vec3_t dir, int color, int count);
void	__fastcall CG_ParticleEffect3 (vec3_t org, vec3_t dir, int color, int count);
void	__fastcall CG_ParticleSmokeEffect (vec3_t org, vec3_t dir, int color, int count, int magnitude);
void	__fastcall CG_RicochetEffect (vec3_t org, vec3_t dir, int count);

void	CG_RocketFireParticles (vec3_t org, vec3_t dir);

void	__fastcall CG_SparkEffect (vec3_t org, vec3_t dir, int color, int colorvel, int count, float smokeScale, float lifeScale);
void	__fastcall CG_SplashParticles (vec3_t org, vec3_t dir, int color, int count, qBool glow);
void	__fastcall CG_SplashEffect (vec3_t org, vec3_t dir, int color, int count);

void	CG_BigTeleportParticles (vec3_t org);
void	CG_BlasterTip (vec3_t start, vec3_t end);
void	CG_ExplosionParticles (vec3_t org, float scale, qBool exploonly, qBool inwater);
void	CG_ExplosionBFGParticles (vec3_t org);
void	CG_ExplosionColorParticles (vec3_t org);
void	CG_FlyEffect (cgEntity_t *ent, vec3_t origin);
void	CG_ForceWall (vec3_t start, vec3_t end, int color);
void	CG_MonsterPlasma_Shell (vec3_t origin);
void	CG_PhalanxTip (vec3_t start, vec3_t end);
void	CG_TeleportParticles (vec3_t org);
void	CG_TeleporterParticles (entityState_t *ent);
void	CG_TrackerShell (vec3_t origin);
void	CG_TrapParticles (entity_t *ent);
void	CG_WidowSplash (vec3_t org);

//
// GLOOM EFFECTS
//

void	CG_GloomBlobTip (vec3_t start, vec3_t end);
void	CG_GloomDroneEffect (vec3_t org, vec3_t dir);
void	CG_GloomEmberTrail (vec3_t start, vec3_t end);
void	CG_GloomFlareTrail (vec3_t start, vec3_t end);
void	CG_GloomGasEffect (vec3_t origin);
void	CG_GloomRepairEffect (vec3_t org, vec3_t dir, int count);
void	CG_GloomStingerFire (vec3_t start, vec3_t end, float size, qBool light);

//
// SUSTAINED EFFECTS
//

void	__fastcall CG_ParticleSteamEffect (vec3_t org, vec3_t dir, int color, int count, int magnitude);

void	CG_ParseNuke (void);
void	CG_ParseSteam (void);
void	CG_ParseWidow (void);

void	CG_ClearSustains (void);
void	CG_AddSustains (void);

//
// TRAIL EFFECTS
//

void	__fastcall CG_BeamTrail (vec3_t start, vec3_t end, int color, float size, float alpha, float alphaVel);
void	CG_BfgTrail (entity_t *ent);
void	CG_BlasterGoldTrail (vec3_t start, vec3_t end);
void	CG_BlasterGreenTrail (vec3_t start, vec3_t end);
void	CG_BubbleTrail (vec3_t start, vec3_t end);
void	CG_BubbleTrail2 (vec3_t start, vec3_t end, int dist);
void	CG_DebugTrail (vec3_t start, vec3_t end);
void	CG_FlagTrail (vec3_t start, vec3_t end, int flags);
void	CG_GibTrail (vec3_t start, vec3_t end, int flags);
void	CG_GrenadeTrail (vec3_t start, vec3_t end);
void	CG_Heatbeam (vec3_t start, vec3_t forward);
void	CG_IonripperTrail (vec3_t start, vec3_t end);
void	CG_QuadTrail (vec3_t start, vec3_t end);
void	CG_RailTrail (vec3_t start, vec3_t end);
void	CG_RocketTrail (vec3_t start, vec3_t end);
void	CG_TagTrail (vec3_t start, vec3_t end);
void	CG_TrackerTrail (vec3_t start, vec3_t end);

//
// PARTICLE THINK FUNCTIONS
//

void	pBloodDripThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pBloodThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pBounceThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pDropletThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pExploAnimThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pFastSmokeThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pFireThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pFireTrailThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pFlareThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pRailSpiralThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pRicochetSparkThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pSlowFireThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pSmokeThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pSparkGrowThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);
void	pSplashThink (struct particle_s *p, vec3_t org, vec3_t angle, vec4_t color, float *size, float *orient, float *time);

/*
=============================================================================

	SUSTAINED PARTICLE EFFECTS

=============================================================================
*/

typedef struct cgSustainPfx_s {
	vec3_t		org;
	vec3_t		dir;

	int			id;
	int			type;

	int			endtime;
	int			nextthink;
	int			thinkinterval;

	int			color;
	int			count;
	int			magnitude;

	void		(*think)(struct cgSustainPfx_s *self);
} cgSustainPfx_t;

/*
=============================================================================

	SCREEN

=============================================================================
*/

#define CG_HSCALE (((float)cg.vidWidth / 640.0f))
#define CG_VSCALE (((float)cg.vidHeight / 480.0f))

//
// cg_screen.c
//

void	SCR_DrawLoading (void);
void	CG_PrepLoadScreen (void);
void	CG_IncLoadPercent (float increment);
void	CG_LoadingPercent (float percent);
void	CG_LoadingString (char *str);
void	CG_LoadingFilename (char *str);

void	SCR_UpdatePING (void);

void	SCR_ParseCenterPrint (void);

void	SCR_Register (void);

void	SCR_Draw (void);

//
// cg_view.c
//

void	V_RenderView (int vidWidth, int vidHeight, int realTime, float netFrameTime, float refreshFrameTime, float stereoSeparation, qBool forceRefresh);

void	V_Register (void);
void	V_Unregister (void);

/*
=============================================================================

	INVENTORY / HUD

=============================================================================
*/

//
// cg_hud.c
//

void	HUD_CopyLayout (void);
void	HUD_DrawLayout (void);
void	HUD_DrawStatusBar (void);

//
// cg_inventory.c
//

void	Inv_ParseInventory (void);
void	Inv_DrawInventory (void);

/*
=============================================================================

	CONSOLE VARIABLES

=============================================================================
*/

extern cVar_t	*cg_advInfrared;
extern cVar_t	*cg_mapEffects;

extern cVar_t	*cg_thirdPerson;
extern cVar_t	*cg_thirdPersonAngle;
extern cVar_t	*cg_thirdPersonDist;

extern cVar_t	*cl_add_decals;
extern cVar_t	*cl_decal_burnlife;
extern cVar_t	*cl_decal_life;
extern cVar_t	*cl_decal_lod;
extern cVar_t	*cl_decal_max;

extern cVar_t	*cl_explorattle;
extern cVar_t	*cl_explorattle_scale;
extern cVar_t	*cl_footsteps;
extern cVar_t	*cl_gun;
extern cVar_t	*cl_noskins;
extern cVar_t	*cl_predict;
extern cVar_t	*cl_showmiss;
extern cVar_t	*cl_vwep;

extern cVar_t	*crosshair;

extern cVar_t	*gender_auto;
extern cVar_t	*gender;
extern cVar_t	*hand;
extern cVar_t	*skin;

extern cVar_t	*glm_advgas;
extern cVar_t	*glm_advstingfire;
extern cVar_t	*glm_blobtype;
extern cVar_t	*glm_bluestingfire;
extern cVar_t	*glm_flashpred;
extern cVar_t	*glm_flwhite;
extern cVar_t	*glm_forcecache;
extern cVar_t	*glm_jumppred;
extern cVar_t	*glm_showclass;

extern cVar_t	*cl_railred;
extern cVar_t	*cl_railgreen;
extern cVar_t	*cl_railblue;
extern cVar_t	*cl_railtrail;
extern cVar_t	*cl_spiralred;
extern cVar_t	*cl_spiralgreen;
extern cVar_t	*cl_spiralblue;

extern cVar_t	*cl_add_particles;
extern cVar_t	*cg_particleCulling;
extern cVar_t	*cg_particleGore;
extern cVar_t	*cg_particleLOD;
extern cVar_t	*cg_particleQuality;
extern cVar_t	*cg_particleShading;
extern cVar_t	*cg_particleSmokeLinger;

extern cVar_t	*r_hudscale;

extern cVar_t	*scr_hudalpha;

/*
=============================================================================

	SUPPORTING FUNCTIONS

=============================================================================
*/

//
// cg_api.c
//

struct shader_s	*CG_RegisterPic (char *name);

//
// cg_draw.c
//

void	CG_DrawChar (float x, float y, int flags, float scale, int num, vec4_t color);
int		CG_DrawString (float x, float y, int flags, float scale, char *string, vec4_t color);
int		CG_DrawStringLen (float x, float y, int flags, float scale, char *string, int len, vec4_t color);

void	CG_DrawModel (int x, int y, int w, int h, struct model_s *model, struct shader_s *shader, vec3_t origin, vec3_t angles);

//
// cg_main.c
//

char	*CG_StrDup (const char *in);

void	CG_UpdateCvars (void);

void	CG_Init (int playerNum, int serverProtocol, qBool attractLoop, int vidWidth, int vidHeight);
void	CG_Shutdown (void);

//
// cg_media.c
//

float	palRed (int index);
float	palGreen (int index);
float	palBlue (int index);

float	palRedf (int index);
float	palGreenf (int index);
float	palBluef (int index);

void	CG_CacheGloomMedia (void);
void	CG_InitBaseMedia (void);
void	CG_MediaInit (void);
void	CG_ShutdownMedia (void);

void	CG_SoundMediaInit (void);
void	CG_CrosshairShaderInit (void);

//
// cg_muzzleflash.c
//

void	CG_ParseMuzzleFlash (void);
void	CG_ParseMuzzleFlash2 (void);

//
// cg_players.c
//

extern char	cg_weaponModels[MAX_CLIENTWEAPONMODELS][MAX_QPATH];
extern int	cg_numWeaponModels;

int		CG_GloomClassForModel (char *model, char *skin);
void	CG_LoadClientinfo (clientInfo_t *ci, char *s);

void	CG_FixUpGender (void);

//
// cg_parse.c
//

void	CG_ParseClientinfo (int player);

void	CG_ParseConfigString (int num, char *str);

void	CG_StartServerMessage (void);
void	CG_EndServerMessage (int realTime);
qBool	CG_ParseServerMessage (int command);

#endif // __CGAME_LOCAL_H__
