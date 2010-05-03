/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
 
/* ScriptData
SDName: boss_festergut
SD%Complete: 1%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */
 
#include "ScriptedPch.h"
#include "def_spire.h"
enum
{
    TYPE_BOSS = TYPE_FESTERGUT,
 
};
enum BossSpells
{
    SPELL_GASEOUS_BLIGHT_0,
    SPELL_GASEOUS_BLIGHT_1,
    SPELL_GASEOUS_BLIGHT_2,
    SPELL_INHALE_BLIGHT,
    SPELL_INHALED_BLIGHT,
    SPELL_PUNGENT_BLIGHT,
    SPELL_GAS_SPORE,
    SPELL_INOCULATE,
    SPELL_GASTRIC_BLOAT,
    SPELL_GASTRIC_EXPLOSION,
    SPELL_VILE_GAS,
    SPELL_BERSERK,
    BOSS_SPELL_COUNT
};
static SpellTable m_BossSpell[]=
{
// Name                  10     25     10H    25H
{SPELL_GASEOUS_BLIGHT_0, 70138, 70140, 70140, 70137, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, false, false},
{SPELL_GASEOUS_BLIGHT_1, 69161, 70139, 70139, 70140, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, false, false},
{SPELL_GASEOUS_BLIGHT_2, 70468, 69161, 69161, 70139, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, false, false},
{SPELL_INHALE_BLIGHT,    69165, 69165, 69165, 69165, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, false, false},
{SPELL_INHALED_BLIGHT,   71912, 71912, 71912, 71912, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, true,  false},
{SPELL_PUNGENT_BLIGHT,   69195, 73031, 71219, 73032, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, false, false},
{SPELL_GAS_SPORE,        69279, 69279, 69279, 69279, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, true,  false},
{SPELL_INOCULATE,        72103, 72103, 72103, 72103, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, false, false},
{SPELL_GASTRIC_BLOAT,    72219, 72551, 72551, 72553, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, false, false},
{SPELL_GASTRIC_EXPLOSION,72227, 72227, 72229, 72230, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, false, false},
{SPELL_VILE_GAS,         72272, 72273, 72273, 73020, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 65535, CAST_ON_RANDOM, false, false},
{SPELL_BERSERK,          47008, 47008, 47008, 47008, 300000, 300000, 300000, 300000, 300000, 300000, 300000, 300000, 65535, CAST_ON_SELF, false, false},
};
 
struct boss_festergutAI : public ScriptedAI
{
    boss_festergutAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
 
    uint8 Difficulty;
    ScriptedInstance *pInstance;
    uint32 m_uiSpell_Timer[BOSS_SPELL_COUNT];
    uint8 stage;
    uint8 substage;
 
    void Reset()
    {
        if(!pInstance) return;
        Difficulty = pInstance->GetData(DIFFICULTY);
        pInstance->SetData(TYPE_BOSS, NOT_STARTED);
        if (!Difficulty) Difficulty = 0;
        memset(&m_uiSpell_Timer, 0, sizeof(m_uiSpell_Timer));
    }
 
    bool QuerySpellPeriod(uint32 m_uiSpellIdx, uint32 diff)
    {
    if(!pInstance) return false;
    bool result;
    SpellTable* pSpell = &m_BossSpell[m_uiSpellIdx];
        if (m_uiSpellIdx != pSpell->id) return false;
        if (m_uiSpell_Timer[m_uiSpellIdx] < diff && m_uiSpell_Timer[m_uiSpellIdx] != 0) {
            m_uiSpell_Timer[m_uiSpellIdx]=urand(pSpell->m_uiSpellTimerMin[Difficulty],pSpell->m_uiSpellTimerMax[Difficulty]);
            result = true;
            } else {
            m_uiSpell_Timer[m_uiSpellIdx] -= diff;
            result = false;
            };
        return result;
    }
 
    bool CastBossSpell(uint32 m_uiSpellIdx)
    {
    if(!pInstance) return false;
    Unit* pTarget;
    SpellTable* pSpell = &m_BossSpell[m_uiSpellIdx];
        // Find spell index - temporary direct insert from spelltable
        if (m_uiSpellIdx != pSpell->id) return false;
 
        switch (pSpell->m_CastTarget) 
			{
            case CAST_ON_SUMMONS:
                   pTarget = me->getVictim(); //CHANGE IT!!!
                   break;
            case CAST_ON_VICTIM:
                   pTarget = me->getVictim();
                   break;
            case CAST_ON_RANDOM:
                   pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                   break;
            case CAST_ON_BOTTOMAGGRO:
                   pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1);
                   break;
            };

					

            if (pTarget) DoCast(pTarget,pSpell->m_uiSpellEntry[Difficulty]); 			
			
return false;	
			
    }
 
    void Aggro(Unit *who) 
    {
        if(pInstance) pInstance->SetData(TYPE_BOSS, IN_PROGRESS);
    }
 
    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_BOSS, DONE);
    }
 
    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;
 
        switch(stage)
        {
            case 0: {
            if (QuerySpellPeriod(SPELL_GASEOUS_BLIGHT_0, diff))
                    CastBossSpell(SPELL_GASEOUS_BLIGHT_0);
                    break;}
            case 1: {
                    break;}
        }
 
        DoMeleeAttackIfReady();
    }
};
 
 
CreatureAI* GetAI_boss_festergut(Creature* pCreature)
{
    return new boss_festergutAI(pCreature);
}
 
 
void AddSC_boss_festergut()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_festergut";
    newscript->GetAI = &GetAI_boss_festergut;
    newscript->RegisterSelf();
}
