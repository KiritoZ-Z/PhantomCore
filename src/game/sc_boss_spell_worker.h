/*Phantomcore*/
/* Copyright (C) 2009 - 2010 by /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BOSS_SPELL_WORKER_H
#define DEF_BOSS_SPELL_WORKER_H

#include "Player.h"
#include "SpellAuras.h"
#include "Unit.h"
#include "ScriptedPch.h"
#include "Database/DatabaseEnv.h"
#include "ScriptMgr.h"

enum
{
  DIFFICULTY_LEVELS = 4,
  MAX_BOSS_SPELLS   = 16,
  SPELL_INDEX_ERROR = 255,
  INSTANCE_MAX_PLAYERS = 40,
};

enum BossSpellFlag
{
  CAST_NORMAL,
  CAST_REMOVE,
  CAST_OVERRIDE,
  SPELLFLAG_NUMBER
};

enum BossSpellTableParameters
{
  DO_NOTHING                =  0,
  CAST_ON_SELF              =  1,
  CAST_ON_SUMMONS           =  2,
  CAST_ON_VICTIM            =  3,
  CAST_ON_RANDOM            =  4,
  CAST_ON_BOTTOMAGGRO       =  5,
  CAST_ON_TARGET            =  6,
  APPLY_AURA_SELF           =  7,
  APPLY_AURA_TARGET         =  8,
  SUMMON_NORMAL             =  9,
  SUMMON_INSTANT            = 10,
  SUMMON_TEMP               = 11,
  CAST_ON_ALLPLAYERS        = 12,
  CAST_ON_FRENDLY           = 13,
  CAST_ON_FRENDLY_LOWHP     = 14,
  CAST_ON_RANDOM_POINT      = 15,
  CAST_ON_RANDOM_PLAYER     = 16,
  SPELLTABLEPARM_NUMBER


};

struct Locations
{
    float x, y, z;
    int32 id;
};

struct WayPoints
{
    WayPoints(int32 _id, float _x, float _y, float _z)
    {
        id = _id;
        x = _x;
        y = _y;
        z = _z;
    }
    int32 id;
    float x, y, z;
};

struct SpellTable
{
    uint32 id;
    uint32 m_uiSpellEntry[DIFFICULTY_LEVELS];          // Stores spell entry for difficulty levels
    uint32 m_uiSpellTimerMin[DIFFICULTY_LEVELS];       // The timer (min) before the next spell casting, in milliseconds
    uint32 m_uiSpellTimerMax[DIFFICULTY_LEVELS];       // The timer (max) before the next spell casting
    uint32 m_uiSpellData[DIFFICULTY_LEVELS];           // Additional data for spell casting or summon
    Locations LocData;                                 // Float data structure for locations
    int32 varData;                                     // Additional data for spell
    uint32 StageMaskN;                                 // Stage mask for this spell (normal)
    uint32 StageMaskH;                                 // Stage mask for this spell (heroic)
    BossSpellTableParameters  m_CastTarget;            // Target on casting spell
    bool   m_IsVisualEffect;                           // Spellcasting is visual effect or real effect
    bool   m_IsBugged;                                 // Need override for this spell
    int32  textEntry;                                  // Text entry from script_text for this spell
};

class BossSpellWorker
{
    public:
        explicit BossSpellWorker(ScriptedAI* bossAI);
        ~BossSpellWorker();

        Unit*  currentTarget;

        void Reset(uint8 _Difficulty = 0);

        void resetTimer(uint32 SpellID)
             {
             return _resetTimer(FindSpellIDX(SpellID));
             };

        void resetTimers()
             {
             for (uint8 i = 0; i < _bossSpellCount; ++i)
                  _resetTimer(i);
             };

        bool timedQuery(uint32 SpellID, uint32 diff)
             {
             return _QuerySpellPeriod(FindSpellIDX(SpellID), diff);
             };

        CanCastResult timedCast(uint32 SpellID, uint32 diff, Unit* pTarget = NULL)
             {
             uint8 m_uiSpellIdx = FindSpellIDX(SpellID);
             if (!_QuerySpellPeriod(FindSpellIDX(SpellID), diff)) return CAST_FAIL_STATE;
                  else return _BSWSpellSelector(m_uiSpellIdx, pTarget);
             };

        CanCastResult doCast(uint32 SpellID, Unit* pTarget = NULL)
             {
                  uint8 m_uiSpellIdx = FindSpellIDX(SpellID);
                  if ( m_uiSpellIdx != SPELL_INDEX_ERROR) return _BSWSpellSelector(m_uiSpellIdx, pTarget);
                  else return CAST_FAIL_OTHER;
             };

        CanCastResult doCast(Unit* pTarget, uint32 SpellID)
             {
                  if (!pTarget) return CAST_FAIL_OTHER;
                  uint8 m_uiSpellIdx = FindSpellIDX(SpellID);
                  if ( m_uiSpellIdx != SPELL_INDEX_ERROR) return _BSWCastOnTarget(pTarget, m_uiSpellIdx);
                  else return CAST_FAIL_OTHER;
             };

        bool doRemove(uint32 SpellID, Unit* pTarget = NULL, SpellEffectIndex index = EFFECT_INDEX_0)
             {
             return _doRemove(FindSpellIDX(SpellID),pTarget, index);
             };

        bool hasAura(uint32 SpellID, Unit* pTarget = NULL)
             {
             if (!pTarget) pTarget = boss;
             return _hasAura(FindSpellIDX(SpellID),pTarget);
             };

        Unit* doSummon(uint32 SpellID, TempSummonType type = TEMPSUMMON_CORPSE_TIMED_DESPAWN, uint32 delay = 60000)
             {
             return _doSummon(FindSpellIDX(SpellID), type, delay);
             };

        Unit* doSummon(uint32 SpellID, float fPosX, float fPosY, float fPosZ, TempSummonType type = TEMPSUMMON_CORPSE_TIMED_DESPAWN, uint32 delay = 60000)
             {
             return _doSummonAtPosition(FindSpellIDX(SpellID), type, delay, fPosX, fPosY, fPosZ);
             };

        CanCastResult BSWSpellSelector(uint32 SpellID, Unit* pTarget = NULL)
             {
             return _BSWSpellSelector(FindSpellIDX(SpellID), pTarget);
             };

        CanCastResult BSWDoCast(uint32 SpellID, Unit* pTarget)
             {
             return _BSWDoCast(FindSpellIDX(SpellID), pTarget);
             };

        Unit*  SelectUnit(SelectAggroTarget target = SELECT_TARGET_RANDOM, uint32 uiPosition = 0)
             {
             return  _SelectUnit(target, uiPosition);
             };

        Unit* SelectLowHPFriendly(float fRange = 40.0f, uint32 uiMinHPDiff = 0);

        uint8 bossSpellCount()
             {
             return _bossSpellCount;
             };

    private:

        BossSpellTableParameters getBSWCastType(uint32 pTemp);

        uint8         FindSpellIDX(uint32 SpellID);

        void          LoadSpellTable();

        void          _resetTimer(uint8 m_uiSpellIdx);

        bool          isSummon(uint8 m_uiSpellIdx);

        Unit*         _doSummon(uint8 m_uiSpellIdx, TempSummonType type = TEMPSUMMON_CORPSE_TIMED_DESPAWN, uint32 delay = 60000);

        Unit*         _doSelect(uint32 SpellID, bool spellsearchtype = false, float range = 100.0f);

        Unit*         _doSummonAtPosition(uint8 m_uiSpellIdx, TempSummonType type, uint32 delay, float fPosX, float fPosY, float fPosZ);

        CanCastResult _BSWDoCast(uint8 m_uiSpellIdx, Unit* pTarget);

        CanCastResult _BSWSpellSelector(uint8 m_uiSpellIdx, Unit* pTarget = NULL);

        CanCastResult _BSWCastOnTarget(Unit* pTarget, uint8 m_uiSpellIdx);

        Difficulty    setDifficulty(uint8 _Difficulty = 0);

        bool          _QuerySpellPeriod(uint8 m_uiSpellIdx, uint32 diff);

        CanCastResult _DoCastSpellIfCan(Unit* pTarget, uint32 uiSpell, uint32 uiCastFlags = 0, uint64 uiOriginalCasterGUID = 0);

        CanCastResult _CanCastSpell(Unit* pTarget, const SpellEntry *pSpell, bool isTriggered = false);

        Unit*         _SelectUnit(SelectAggroTarget target, uint32 uiPosition);

        bool          _doRemove(uint8 m_uiSpellIdx, Unit* pTarget = NULL, SpellEffectIndex index = EFFECT_INDEX_0);

        bool          _hasAura(uint8 m_uiSpellIdx, Unit* pTarget);

        void          _fillEmptyDataField();

// Constants from CreatureAI()
           ScriptedAI* bossAI;
           Creature* boss;
           uint32 bossID;
           uint8 _bossSpellCount;
           Difficulty currentDifficulty;
           uint32 m_uiSpell_Timer[MAX_BOSS_SPELLS];
           SpellTable m_BossSpell[MAX_BOSS_SPELLS];
           Map* pMap;
};

#endif