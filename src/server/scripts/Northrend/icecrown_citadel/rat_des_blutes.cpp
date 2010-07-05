/*
* Copyright (C) 2009 - 2010 TrinityCore <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "ScriptPCH.h"
#include "icecrown_citadel.h"

/*HACK*/
struct boss_blood_elf_valanar_iccAI : public ScriptedAI
{
    boss_blood_elf_valanar_iccAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;


    void Reset()
    {

        if(m_pInstance)
            m_pInstance->SetData(DATA_BLOOD_PRINCE_COUNCIL_EVENT, NOT_STARTED);  
    }

    void JustDied(Unit* pKiller)
    {
        if(m_pInstance)
            m_pInstance->SetData(DATA_BLOOD_PRINCE_COUNCIL_EVENT, DONE);  
    }

    void EnterCombat(Unit* who)
    {
        if(m_pInstance)
            m_pInstance->SetData(DATA_BLOOD_PRINCE_COUNCIL_EVENT, IN_PROGRESS);  
    }

    void JustReachedHome()
    {
        if(m_pInstance)
            m_pInstance->SetData(DATA_BLOOD_PRINCE_COUNCIL_EVENT, FAIL);  
    }

    void KilledUnit(Unit *victim)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct boss_blood_elf_taldaram_iccAI : public ScriptedAI
{
    boss_blood_elf_taldaram_iccAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
    }

    void EnterCombat(Unit* who)
    {
    }

    void KilledUnit(Unit *victim)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct boss_blood_elf_keleset_iccAI : public ScriptedAI
{
    boss_blood_elf_keleset_iccAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
    }

    void EnterCombat(Unit* who)
    {
    }

    void KilledUnit(Unit *victim)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_blood_elf_valanar_icc(Creature* pCreature)
{
    return new boss_blood_elf_valanar_iccAI(pCreature);
}

CreatureAI* GetAI_boss_blood_elf_taldaram_icc(Creature* pCreature)
{
    return new boss_blood_elf_taldaram_iccAI(pCreature);
}

CreatureAI* GetAI_boss_blood_elf_keleset_icc(Creature* pCreature)
{
    return new boss_blood_elf_keleset_iccAI(pCreature);
}

void AddSC_boss_rat_des_blutes()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_blood_elf_valanar_icc";
    newscript->GetAI = &GetAI_boss_blood_elf_valanar_icc;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_blood_elf_taldaram_icc";
    newscript->GetAI = &GetAI_boss_blood_elf_taldaram_icc;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_blood_elf_keleset_icc";
    newscript->GetAI = &GetAI_boss_blood_elf_keleset_icc;
    newscript->RegisterSelf();
}
