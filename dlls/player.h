/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

#include "weapons.h"
#include "pm_materials.h"
#include "hintmessage.h"
#include "unisignals.h"

#define SOUND_FLASHLIGHT_ON  "items/flashlight1.wav"
#define SOUND_FLASHLIGHT_OFF "items/flashlight1.wav"

const int MAX_PLAYER_NAME_LENGTH    = 32;
const int MAX_AUTOBUY_LENGTH        = 256;
const int MAX_REBUY_LENGTH          = 256;

const int MAX_RECENT_PATH           = 20;
const int MAX_HOSTAGE_ICON          = 4;	// the maximum number of icons of the hostages in the HUD

const int MAX_SUIT_NOREPEAT         = 32;
const int MAX_SUIT_PLAYLIST         = 4;	// max of 4 suit sentences queued up at any time

const int MAX_BUFFER_MENU           = 175;
const int MAX_BUFFER_MENU_BRIEFING  = 50;

const float SUIT_UPDATE_TIME        = 3.5f;
const float SUIT_FIRST_UPDATE_TIME  = 0.1f;

const float MAX_PLAYER_FATAL_FALL_SPEED = 1100.0f;
const float MAX_PLAYER_SAFE_FALL_SPEED  = 500.0f;
const float MAX_PLAYER_USE_RADIUS       = 64.0f;

const float ARMOR_RATIO = 0.5f;			// Armor Takes 50% of the damage
const float ARMOR_BONUS = 0.5f;			// Each Point of Armor is work 1/x points of health

const float FLASH_DRAIN_TIME = 1.2f;	// 100 units/3 minutes
const float FLASH_CHARGE_TIME = 0.2f;	// 100 units/20 seconds (seconds per unit)

// damage per unit per second.
const float DAMAGE_FOR_FALL_SPEED   = 100.0f / (MAX_PLAYER_FATAL_FALL_SPEED - MAX_PLAYER_SAFE_FALL_SPEED);
const float PLAYER_MIN_BOUNCE_SPEED = 350.0f;

// won't punch player's screen/make scrape noise unless player falling at least this fast.
const float PLAYER_FALL_PUNCH_THRESHHOLD = 250.0f;

// Money blinks few of times on the freeze period
// NOTE: It works for CZ
const int MONEY_BLINK_AMOUNT = 30;

// Player time based damage
#define AIRTIME                 12		// lung full of air lasts this many seconds
#define PARALYZE_DURATION       2		// number of 2 second intervals to take damage
#define PARALYZE_DAMAGE         1.0f	// damage to take each 2 second interval

#define NERVEGAS_DURATION       2
#define NERVEGAS_DAMAGE         5.0f

#define POISON_DURATION         5
#define POISON_DAMAGE           2.0f

#define RADIATION_DURATION      2
#define RADIATION_DAMAGE        1.0f

#define ACID_DURATION           2
#define ACID_DAMAGE             5.0f

#define SLOWBURN_DURATION       2
#define SLOWBURN_DAMAGE         1.0f

#define SLOWFREEZE_DURATION     2
#define SLOWFREEZE_DAMAGE       1.0f

// Player physics flags bits
// CBasePlayer::m_afPhysicsFlags
#define PFLAG_ONLADDER          BIT(0)
#define PFLAG_ONSWING           BIT(0)
#define PFLAG_ONTRAIN           BIT(1)
#define PFLAG_ONBARNACLE        BIT(2)
#define PFLAG_DUCKING           BIT(3) // In the process of ducking, but not totally squatted yet
#define PFLAG_USING             BIT(4) // Using a continuous entity
#define PFLAG_OBSERVER          BIT(5) // Player is locked in stationary cam mode. Spectators can move, observers can't.

#define TRAIN_OFF               0x00
#define TRAIN_NEUTRAL           0x01
#define TRAIN_SLOW              0x02
#define TRAIN_MEDIUM            0x03
#define TRAIN_FAST              0x04
#define TRAIN_BACK              0x05

#define TRAIN_ACTIVE            0x80
#define TRAIN_NEW               0xc0

const bool SUIT_GROUP           = true;
const bool SUIT_SENTENCE        = false;

const int  SUIT_REPEAT_OK       = 0;
const int  SUIT_NEXT_IN_30SEC	= 30;
const int  SUIT_NEXT_IN_1MIN    = 60;
const int  SUIT_NEXT_IN_5MIN    = 300;
const int  SUIT_NEXT_IN_10MIN   = 600;
const int  SUIT_NEXT_IN_30MIN   = 1800;
const int  SUIT_NEXT_IN_1HOUR   = 3600;

const int MAX_TEAM_NAME_LENGTH  = 16;

const auto AUTOAIM_2DEGREES     = 0.0348994967025;
const auto AUTOAIM_5DEGREES     = 0.08715574274766;
const auto AUTOAIM_8DEGREES     = 0.1391731009601;
const auto AUTOAIM_10DEGREES    = 0.1736481776669;

// custom enum
enum RewardType
{
	RT_NONE,
	RT_ROUND_BONUS,
	RT_PLAYER_RESET,
	RT_PLAYER_JOIN,
	RT_PLAYER_SPEC_JOIN,
	RT_PLAYER_BOUGHT_SOMETHING,
	RT_HOSTAGE_TOOK,
	RT_HOSTAGE_RESCUED,
	RT_HOSTAGE_DAMAGED,
	RT_HOSTAGE_KILLED,
	RT_TEAMMATES_KILLED,
	RT_ENEMY_KILLED,
	RT_INTO_GAME,
	RT_VIP_KILLED,
	RT_VIP_RESCUED_MYSELF
};

enum PLAYER_ANIM
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
	PLAYER_ATTACK2,
	PLAYER_FLINCH,
	PLAYER_LARGE_FLINCH,
	PLAYER_RELOAD,
	PLAYER_HOLDBOMB
};

enum _Menu
{
	Menu_OFF,
	Menu_ChooseTeam,
	Menu_IGChooseTeam,
	Menu_ChooseAppearance,
	Menu_Buy,
	Menu_BuyPistol,
	Menu_BuyRifle,
	Menu_BuyMachineGun,
	Menu_BuyShotgun,
	Menu_BuySubMachineGun,
	Menu_BuyItem,
	Menu_Radio1,
	Menu_Radio2,
	Menu_Radio3,
	Menu_ClientBuy
};

enum TeamName
{
	UNASSIGNED,
	TERRORIST,
	CT,
	SPECTATOR,
};

enum ModelName
{
	MODEL_UNASSIGNED,
	MODEL_URBAN,
	MODEL_TERROR,
	MODEL_LEET,
	MODEL_ARCTIC,
	MODEL_GSG9,
	MODEL_GIGN,
	MODEL_SAS,
	MODEL_GUERILLA,
	MODEL_VIP,
	MODEL_MILITIA,
	MODEL_SPETSNAZ,
	MODEL_AUTO
};

enum JoinState
{
	JOINED,
	SHOWLTEXT,
	READINGLTEXT,
	SHOWTEAMSELECT,
	PICKINGTEAM,
	GETINTOGAME
};

enum TrackCommands
{
	CMD_SAY = 0,
	CMD_SAYTEAM,
	CMD_FULLUPDATE,
	CMD_VOTE,
	CMD_VOTEMAP,
	CMD_LISTMAPS,
	CMD_LISTPLAYERS,
	CMD_NIGHTVISION,
	COMMANDS_TO_TRACK,
};

enum IgnoreChatMsg : int
{
	IGNOREMSG_NONE,  // Nothing to do
	IGNOREMSG_ENEMY, // To ignore any chat messages from the enemy
	IGNOREMSG_TEAM   // Same as IGNOREMSG_ENEMY but ignore teammates
};

struct RebuyStruct
{
	int m_primaryWeapon;
	int m_primaryAmmo;
	int m_secondaryWeapon;
	int m_secondaryAmmo;
	int m_heGrenade;
	int m_flashbang;
	int m_smokeGrenade;
	int m_defuser;
	int m_nightVision;
	ArmorType m_armor;
};

enum ThrowDirection
{
	THROW_NONE,
	THROW_FORWARD,
	THROW_BACKWARD,
	THROW_HITVEL,
	THROW_BOMB,
	THROW_GRENADE,
	THROW_HITVEL_MINUS_AIRVEL
};

const float MAX_ID_RANGE            = 2048.0f;
const float MAX_SPEC_ID_RANGE       = 8192.0f;
const int   MAX_SBAR_STRING         = 128;

const int SBAR_TARGETTYPE_TEAMMATE  = 1;
const int SBAR_TARGETTYPE_ENEMY     = 2;
const int SBAR_TARGETTYPE_HOSTAGE   = 3;

enum sbar_data
{
	SBAR_ID_TARGETTYPE = 1,
	SBAR_ID_TARGETNAME,
	SBAR_ID_TARGETHEALTH,
	SBAR_END
};

enum MusicState { SILENT, CALM, INTENSE };

class CCSPlayer;

class CStripWeapons: public CPointEntity {
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
};

// Multiplayer intermission spots.
class CInfoIntermission: public CPointEntity {
public:
	virtual void Spawn() = 0;
	virtual void Think() = 0;
};

// Dead HEV suit prop
class CDeadHEV: public CBaseMonster {
public:
	virtual void Spawn() = 0;
	virtual void KeyValue(KeyValueData *pkvd) = 0;
	virtual int Classify() = 0;
public:
	int m_iPose;				// which sequence to display -- temporary, don't need to save
	static char *m_szPoses[4];
};

class CSprayCan: public CBaseEntity {
public:
	virtual void Think() = 0;
	virtual int ObjectCaps() = 0;
};

class CBloodSplat: public CBaseEntity {
public:
};

class CBasePlayer: public CBaseMonster {
public:
	virtual void Spawn() = 0;
	virtual void Precache() = 0;
	virtual int Save(CSave &save) = 0;
	virtual int Restore(CRestore &restore) = 0;
	virtual int ObjectCaps() = 0;
	virtual int Classify() = 0;
	virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) = 0;
	virtual BOOL TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) = 0;
	virtual BOOL TakeHealth(float flHealth, int bitsDamageType) = 0;
	virtual void Killed(entvars_t *pevAttacker, int iGib) = 0;
	virtual void AddPoints(int score, BOOL bAllowNegativeScore) = 0;
	virtual void AddPointsToTeam(int score, BOOL bAllowNegativeScore) = 0;
	virtual BOOL AddPlayerItem(CBasePlayerItem *pItem) = 0;
	virtual BOOL RemovePlayerItem(CBasePlayerItem *pItem) = 0;
	virtual int GiveAmmo(int iAmount, const char *szName, int iMax = -1) = 0;
	virtual void StartSneaking() = 0;
	virtual void UpdateOnRemove() = 0;
	virtual BOOL IsSneaking() = 0;
	virtual BOOL IsAlive() = 0;
	virtual BOOL IsPlayer() = 0;
	virtual BOOL IsNetClient() = 0;
	virtual const char *TeamID() = 0;
	virtual BOOL FBecomeProne() = 0;
	virtual Vector BodyTarget(const Vector &posSrc) = 0;
	virtual int Illumination() = 0;
	virtual BOOL ShouldFadeOnDeath() = 0;
	virtual void ResetMaxSpeed() = 0;
	virtual void Jump() = 0;
	virtual void Duck() = 0;
	virtual void PreThink() = 0;
	virtual void PostThink() = 0;
	virtual Vector GetGunPosition() = 0;
	virtual BOOL IsBot() = 0;
	virtual void UpdateClientData() = 0;
	virtual void ImpulseCommands() = 0;
	virtual void RoundRespawn() = 0;
	virtual Vector GetAutoaimVector(float flDelta) = 0;
	virtual void Blind(float flUntilTime, float flHoldTime, float flFadeTime, int iAlpha) = 0;
	virtual void OnTouchingWeapon(CWeaponBox *pWeapon) = 0;
public:
	static CBasePlayer *Instance(edict_t *pent) { return (CBasePlayer *)GET_PRIVATE(pent ? pent : ENT(0)); }
	static CBasePlayer *Instance(entvars_t *pev) { return Instance(ENT(pev)); }
	static CBasePlayer *Instance(int offset) { return Instance(ENT(offset)); }

	int IsObserver() { return pev->iuser1; }
	void SetWeaponAnimType(const char *szExtention) { strcpy(m_szAnimExtention, szExtention); }
	bool IsProtectedByShield() { return m_bOwnsShield && m_bShieldDrawn; }
	bool IsReloading() const;
	bool IsBlind() const { return (m_blindUntilTime > gpGlobals->time); }
	bool IsAutoFollowAllowed() const { return (gpGlobals->time > m_allowAutoFollowTime); }
	void InhibitAutoFollow(float duration) { m_allowAutoFollowTime = gpGlobals->time + duration; }
	void AllowAutoFollow() { m_allowAutoFollowTime = 0; }
	void SetObserverAutoDirector(bool val) { m_bObserverAutoDirector = val; }
	bool CanSwitchObserverModes() const { return m_canSwitchObserverModes; }
	CCSPlayer *CSPlayer() const;

	// templates
	template<typename T = CBasePlayerItem, typename Functor>
	T *ForEachItem(int slot, const Functor &func)
	{
		auto item = m_rgpPlayerItems[ slot ];
		while (item)
		{
			if (func(static_cast<T *>(item)))
				return static_cast<T *>(item);

			item = item->m_pNext;
		}

		return nullptr;
	}

	template<typename T = CBasePlayerItem, typename Functor>
	T *ForEachItem(const Functor &func)
	{
		for (auto item : m_rgpPlayerItems)
		{
			while (item)
			{
				if (func(static_cast<T *>(item)))
					return static_cast<T *>(item);

				item = item->m_pNext;
			}
		}

		return nullptr;
	}

	template<typename T = CBasePlayerItem, typename Functor>
	T *ForEachItem(const char *pszItemName, const Functor &func)
	{
		if (!pszItemName) {
			return nullptr;
		}

		for (auto item : m_rgpPlayerItems)
		{
			while (item)
			{
				if (FStrEq(pszItemName, STRING(item->pev->classname)) && func(static_cast<T *>(item))) {
					return static_cast<T *>(item);
				}

				item = item->m_pNext;
			}
		}

		return nullptr;
	}

public:
	enum { MaxLocationLen = 32 };

	int random_seed;
	unsigned short m_usPlayerBleed;
	EHANDLE m_hObserverTarget;
	float m_flNextObserverInput;
	int m_iObserverWeapon;
	int m_iObserverC4State;
	bool m_bObserverHasDefuser;
	int m_iObserverLastMode;
	float m_flFlinchTime;
	float m_flAnimTime;
	bool m_bHighDamage;
	float m_flVelocityModifier;
	int m_iLastZoom;
	bool m_bResumeZoom;
	float m_flEjectBrass;
	ArmorType m_iKevlar;
	bool m_bNotKilled;
	TeamName m_iTeam;
	int m_iAccount;
	bool m_bHasPrimary;
	float m_flDeathThrowTime;
	int m_iThrowDirection;
	float m_flLastTalk;
	bool m_bJustConnected;
	bool m_bContextHelp;
	JoinState m_iJoiningState;
	CBaseEntity *m_pIntroCamera;
	float m_fIntroCamTime;
	float m_fLastMovement;
	bool m_bMissionBriefing;
	bool m_bTeamChanged;
	ModelName m_iModelName;
	int m_iTeamKills;
	IgnoreChatMsg m_iIgnoreGlobalChat;
	bool m_bHasNightVision;
	bool m_bNightVisionOn;
	Vector m_vRecentPath[MAX_RECENT_PATH];
	float m_flIdleCheckTime;
	float m_flRadioTime;
	int m_iRadioMessages;
	bool m_bIgnoreRadio;
	bool m_bHasC4;
	bool m_bHasDefuser;
	bool m_bKilledByBomb;
	Vector m_vBlastVector;
	bool m_bKilledByGrenade;
	CHintMessageQueue m_hintMessageQueue;
	int m_flDisplayHistory;
	_Menu m_iMenu;
	int m_iChaseTarget;
	CBaseEntity *m_pChaseTarget;
	float m_fCamSwitch;
	bool m_bEscaped;
	bool m_bIsVIP;
	float m_tmNextRadarUpdate;
	Vector m_vLastOrigin;
	int m_iCurrentKickVote;
	float m_flNextVoteTime;
	bool m_bJustKilledTeammate;
	int m_iHostagesKilled;
	int m_iMapVote;
	bool m_bCanShoot;
	float m_flLastFired;
	float m_flLastAttackedTeammate;
	bool m_bHeadshotKilled;
	bool m_bPunishedForTK;
	bool m_bReceivesNoMoneyNextRound;
	int m_iTimeCheckAllowed;
	bool m_bHasChangedName;
	char m_szNewName[MAX_PLAYER_NAME_LENGTH];
	bool m_bIsDefusing;
	float m_tmHandleSignals;
	CUnifiedSignals m_signals;
	edict_t *m_pentCurBombTarget;
	int m_iPlayerSound;
	int m_iTargetVolume;
	int m_iWeaponVolume;
	int m_iExtraSoundTypes;
	int m_iWeaponFlash;
	float m_flStopExtraSoundTime;
	float m_flFlashLightTime;
	int m_iFlashBattery;
	int m_afButtonLast;
	int m_afButtonPressed;
	int m_afButtonReleased;
	edict_t *m_pentSndLast;
	float m_flSndRoomtype;
	float m_flSndRange;
	float m_flFallVelocity;
	int m_rgItems[MAX_ITEMS];
	int m_fNewAmmo;
	unsigned int m_afPhysicsFlags;
	float m_fNextSuicideTime;
	float m_flTimeStepSound;
	float m_flTimeWeaponIdle;
	float m_flSwimTime;
	float m_flDuckTime;
	float m_flWallJumpTime;
	float m_flSuitUpdate;
	int m_rgSuitPlayList[MAX_SUIT_PLAYLIST];
	int m_iSuitPlayNext;
	int m_rgiSuitNoRepeat[MAX_SUIT_NOREPEAT];
	float m_rgflSuitNoRepeatTime[MAX_SUIT_NOREPEAT];
	int m_lastDamageAmount;
	float m_tbdPrev;
	float m_flgeigerRange;
	float m_flgeigerDelay;
	int m_igeigerRangePrev;
	int m_iStepLeft;
	char m_szTextureName[CBTEXTURENAMEMAX];
	char m_chTextureType;
	int m_idrowndmg;
	int m_idrownrestored;
	int m_bitsHUDDamage;
	BOOL m_fInitHUD;
	BOOL m_fGameHUDInitialized;
	int m_iTrain;
	BOOL m_fWeapon;
	EHANDLE m_pTank;
	float m_fDeadTime;
	BOOL m_fNoPlayerSound;
	BOOL m_fLongJump;
	float m_tSneaking;
	int m_iUpdateTime;
	int m_iClientHealth;
	int m_iClientBattery;
	int m_iHideHUD;
	int m_iClientHideHUD;
	int m_iFOV;
	int m_iClientFOV;
	int m_iNumSpawns;
	CBaseEntity *m_pObserver;
	CBasePlayerItem *m_rgpPlayerItems[MAX_ITEM_TYPES];
	CBasePlayerItem *m_pActiveItem;
	CBasePlayerItem *m_pClientActiveItem;
	CBasePlayerItem *m_pLastItem;
	int m_rgAmmo[MAX_AMMO_SLOTS];
	int m_rgAmmoLast[MAX_AMMO_SLOTS];
	Vector m_vecAutoAim;
	BOOL m_fOnTarget;
	int m_iDeaths;
	int m_izSBarState[SBAR_END];
	float m_flNextSBarUpdateTime;
	float m_flStatusBarDisappearDelay;
	char m_SbarString0[MAX_SBAR_STRING];
	int m_lastx;
	int m_lasty;
	int m_nCustomSprayFrames;
	float m_flNextDecalTime;
	char m_szTeamName[MAX_TEAM_NAME_LENGTH];
	int m_modelIndexPlayer;
	char m_szAnimExtention[32];
	int m_iGaitsequence;
	float m_flGaitframe;
	float m_flGaityaw;
	Vector m_prevgaitorigin;
	float m_flPitch;
	float m_flYaw;
	float m_flGaitMovement;
	int m_iAutoWepSwitch;
	bool m_bVGUIMenus;
	bool m_bShowHints;
	bool m_bShieldDrawn;
	bool m_bOwnsShield;
	bool m_bWasFollowing;
	float m_flNextFollowTime;
	float m_flYawModifier;
	float m_blindUntilTime;
	float m_blindStartTime;
	float m_blindHoldTime;
	float m_blindFadeTime;
	int m_blindAlpha;
	float m_allowAutoFollowTime;
	char m_autoBuyString[MAX_AUTOBUY_LENGTH];
	char *m_rebuyString;
	RebuyStruct m_rebuyStruct;
	bool m_bIsInRebuy;
	float m_flLastUpdateTime;
	char m_lastLocation[MaxLocationLen];
	float m_progressStart;
	float m_progressEnd;
	bool m_bObserverAutoDirector;
	bool m_canSwitchObserverModes;
	float m_heartBeatTime;
	float m_intenseTimestamp;
	float m_silentTimestamp;
	MusicState m_musicState;
	float m_flLastCommandTime[COMMANDS_TO_TRACK];
};

class CWShield: public CBaseEntity {
public:
	virtual void Spawn() = 0;
	virtual void Touch(CBaseEntity *pOther) = 0;
public:
	void SetCantBePickedUpByUser(CBasePlayer *pPlayer, float time) { m_hEntToIgnoreTouchesFrom = pPlayer; m_flTimeToIgnoreTouches = gpGlobals->time + time; }
public:
	EntityHandle<CBasePlayer> m_hEntToIgnoreTouchesFrom;
	float m_flTimeToIgnoreTouches;
};

inline bool CBasePlayer::IsReloading() const
{
	CBasePlayerWeapon *weapon = static_cast<CBasePlayerWeapon *>(m_pActiveItem);
	if (weapon && weapon->m_fInReload)
		return true;

	return false;
}

inline CCSPlayer *CBasePlayer::CSPlayer() const {
	return reinterpret_cast<CCSPlayer *>(this->m_pEntity);
}

// returns a CBaseEntity pointer to a player by index.  Only returns if the player is spawned and connected otherwise returns NULL
// Index is 1 based
inline CBasePlayer *UTIL_PlayerByIndex(int playerIndex)
{
	return (CBasePlayer *)GET_PRIVATE(INDEXENT(playerIndex));
}

inline CBasePlayer *UTIL_PlayerByIndexSafe(int playerIndex)
{
	CBasePlayer *player = nullptr;
	if (likely(playerIndex > 0 && playerIndex <= gpGlobals->maxClients))
		player = UTIL_PlayerByIndex(playerIndex);

	return player;
}
