/* Copyright (C) 2010 /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ScriptPCH.h"
#include "ruby_sanctum.h"

enum eTexts
{
        SAY_SPAWN = -1752027,
        SAY_AGGRO = -1752028,
        SAY_SLAY1 = -1752029,
        SAY_SLAY2 = -1752030,
        SAY_DEATH = -1752031,
        SAY_BERSERK = -1752032,
        SAY_SPECIAL1 = -1752033,
        SAY_SPECIAL2 = -1752033,
        SAY_PHASE2 = -1752033,
        SAY_PHASE3 = -1752033

};

enum eHalionSpells
{
        SPELL_FIERYCOMBUSTION = 74562,
        SPELL_METEORSTRIKE = 75877,
        SPELL_FLAMEBREATH = 74525,
        SPELL_FLAMEBREATH_25 = 74526,
        SPELL_TAILLASH = 74531,
        SPELL_BERSERK = 26662
};

struct boss_halionAI : public ScriptedAI
{
        boss_halionAI(Creature *pCreature) : ScriptedAI(pCreature)
        {
                pInstance = me->GetInstanceData();
        }

        InstanceData* pInstance;

        uint32 m_uiFieryCombustionTimer;
        uint32 m_uiMeteorStrikeTimer;
        uint32 m_uiFlameBreathTimer;
        uint32 m_uiBerserkTimer;
        uint32 m_uiTailLashTimer;
        uint32 m_uiFlammenAtemTimer;
        uint32 m_uiBeserkTimer;

        bool bIntro;
        bool isMode25;

        void Reset()
        {
                m_uiFieryCombustionTimer = 15000;
                m_uiMeteorStrikeTimer = 30000;
                m_uiFlameBreathTimer = 15000;
                m_uiBerserkTimer = 6000000;
                m_uiTailLashTimer = 10000;
                
                if(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
                        isMode25 = false;
                else
                        isMode25 = true;

                bIntro = false;
        }

        void EnterCombat(Unit*)
        {
                pInstance->SetData(DATA_HALION_EVENT, IN_PROGRESS);
                
                // -- Gameobject Wall (guid: 7999000) spawn on aggro, despawn on death of halion -- //
                
                // -- First possibility -- "Finding" the GO and changing the "SpawnTimeSec" to a positive value to making the GO visible -- //
                
                                /*
                
                                GameObject *pWall = _unit->GetMapMgr()->GetInterface()->GetObjectNearestCoords<GameObject, TYPEID_GAMEOBJECT> (7999000, 3154.56, 535.418, 72.889);
                                if(pWall == 0)
                                        return;

                                pWall->SetUInt32Value(GAMEOBJECT_SPAWNTIMESEC, 300);

                                */
                
                // -- Second possibility -- Spawn the GO on Aggro and making it despawn on dead somehow -- //
                
                                /*
                
                                pPlayer->SummonGameObject(7999000, 3154.56, 535.418, 72.8889, 4.47206, 0, 0, 0.786772, -0.617243, 300); 
                
                                */
                // ------------------------------------------------------------------------------ //
                
                DoScriptText(SAY_AGGRO, me);
        }

        void UpdateAI(const uint32 diff)
        {
                if(!bIntro)
                        if((pInstance->GetData(DATA_BALTHARUS_EVENT) == DONE && pInstance->GetData(DATA_ZARITHRIAN_EVENT) == DONE && pInstance->GetData(DATA_RAGEFIRE_EVENT) == DONE) || (pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC))
                        {
                                DoScriptText(SAY_SPAWN, me);
                                bIntro = true;
                        }

                if(!UpdateVictim())
                        return;

        // Fiery Combustion     
                if (m_uiFieryCombustionTimer <= diff)
        {
                        DoCast(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_FIERYCOMBUSTION : SPELL_FIERYCOMBUSTION); //Same spell in 25 & 10
                        m_uiFieryCombustionTimer = urand(15000,15000);
                } 
        else 
                m_uiFieryCombustionTimer -= diff;


        // Meteor Strike
        if (m_uiMeteorStrikeTimer <= diff)
                {
                        DoCast(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_METEORSTRIKE/*_25 - not yet implemented*/ : SPELL_METEORSTRIKE);
                        DoScriptText(SAY_SPECIAL1, me);
                        m_uiMeteorStrikeTimer = urand(30000,30000);
                }
        else
                m_uiMeteorStrikeTimer -= diff;


        // Flame Breath
        if (m_uiFlameBreathTimer <= diff)
                {
                        DoCast(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_FLAMEBREATH_25 : SPELL_FLAMEBREATH);
                        DoScriptText(SAY_SLAY2, me);
                        m_uiFlameBreathTimer = urand(10000,15000);
                        
                }
        else
                m_uiFlammenAtemTimer -= diff;
                
        // Tail Lash
        if (m_uiTailLashTimer <= diff)
                {
                        DoCast(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_TAILLASH/*_25 - not yet implemented, dont know if it even exists*/ : SPELL_TAILLASH);
                        m_uiTailLashTimer = urand(10000,15000);
                }
        else
                m_uiTailLashTimer -= diff;
                
        // Enraged
        if (m_uiBerserkTimer <= diff)
                {
                        DoCast(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_BERSERK/*_25 - not yet implemented*/ : SPELL_BERSERK);
                        DoScriptText(SAY_BERSERK, me);
                        m_uiBeserkTimer = urand(6000000,6000000);
                        
                }
        else
                m_uiBerserkTimer -= diff;
                
                DoMeleeAttackIfReady();
        }

        void JustDied(Unit*)
        {
                pInstance->SetData(DATA_HALION_EVENT, DONE);
                DoScriptText(SAY_DEATH, me);
        }

        void KilledUnit(Unit*)
        {
                DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
        }

        void EnterEvadeMode()
        {
                pInstance->SetData(DATA_HALION, NOT_STARTED);
        }
};

CreatureAI* GetAI_boss_halion(Creature *pCreature)
{
        return new boss_halionAI(pCreature);
}

struct boss_twilight_halionAI : public ScriptedAI
{
        boss_twilight_halionAI(Creature *pCreature) : ScriptedAI(pCreature)
        {
                pInstance = me->GetInstanceData();
        }

        InstanceData* pInstance;

        void UpdateAI(const uint32 diff)
        {
                if(!UpdateVictim())
                        return;

                DoMeleeAttackIfReady();
        }
};

CreatureAI* GetAI_boss_twilight_halion(Creature *pCreature)
{
        return new boss_halionAI(pCreature);
}

void AddSC_boss_halion()
{
        Script* newscript;

        newscript = new Script;
        newscript->Name = "boss_halion";
        newscript->GetAI = &GetAI_boss_halion;
        newscript->RegisterSelf();

        newscript = new Script;
        newscript->Name = "boss_twilight_halion";
        newscript->GetAI = &GetAI_boss_twilight_halion;
        newscript->RegisterSelf();
}