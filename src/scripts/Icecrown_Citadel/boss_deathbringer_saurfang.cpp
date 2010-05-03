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
SDName: boss_deathbringer_saurfang
SD%Complete: 10%
SDComment: by /dev/rsa
SDCategory: Icecrown Citadel
EndScriptData */

#include "ScriptedPch.h"
#include "def_spire.h"

enum
{
    SAY_AGGRO           = -1631100,
    SAY_FALLENCHAMPION  = -1631101,
    SAY_BLOODBEASTS     = -1631102,
    SAY_KILL1           = -1631103,
    SAY_KILL2           = -1631104,
    SAY_BERSERK         = -1631105,
    SAY_DEATH           = -1631106,

    BLOOD_LINK          = 72178,
    FALLEN_CHAMPION     = 72293,
    FRENZY              = 72737,
    BOILING_BLOOD       = 72385,
    BLOOD_NOVA          = 72380,
    RUNE_OF_BLOOD       = 72408,
    BLOOD_BEAST         = 72173,
};

struct boss_deathbringer_saurfangAI : public ScriptedAI
{
    boss_deathbringer_saurfangAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bRegularMode;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bRegularMode;

    uint32 BloodBeast_Timer;
    uint32 RuneOfBlood_Timer;
    uint32 BoilingBlood_Timer;
    uint32 BloodNova_Timer;

    void Reset()
    {
        BloodBeast_Timer = 40000;
        RuneOfBlood_Timer = 30000;
        BoilingBlood_Timer = 60000;
        BloodNova_Timer = 15000;
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAURFANG, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, me);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAURFANG, DONE);

        DoScriptText(SAY_DEATH, me);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAURFANG, FAIL);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 1))
        {
            case 0: DoScriptText(SAY_KILL1, me); break;
            case 1: DoScriptText(SAY_KILL2, me); break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        if (!me->HasAura(BLOOD_LINK))
            DoCast(me, BLOOD_LINK);

        if ((me->GetHealth()*100 / me->GetMaxHealth() < 30) && (!me->HasAura(FRENZY)))
            DoCast(me, FRENZY);

        /*
        if (me->GetPower(me->getPowerType()) == me->GetMaxPower(me->getPowerType()))
        {
            DoScriptText(SAY_FALLENCHAMPION, me);

            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, FALLEN_CHAMPION);
            me->SetPower(me->getPowerType(),0);
        }
        */

        if (BloodBeast_Timer < uiDiff)
        {
            DoScriptText(SAY_BLOODBEASTS, me);

            DoCast(me, BLOOD_BEAST);
            BloodBeast_Timer = 40000;
        }
        else BloodBeast_Timer -= uiDiff;

        if (RuneOfBlood_Timer < uiDiff)
        {
            DoCast(me->getVictim(), RUNE_OF_BLOOD);
            RuneOfBlood_Timer = 30000;
        }
        else RuneOfBlood_Timer -= uiDiff;

        if (BoilingBlood_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, BOILING_BLOOD);
            BoilingBlood_Timer = 60000;
        }
        else BoilingBlood_Timer -= uiDiff;

        if (BloodNova_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, BLOOD_NOVA);
            BloodNova_Timer = 15000;
        }
        else BloodNova_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_deathbringer_saurfang(Creature* pCreature)
{
    return new boss_deathbringer_saurfangAI(pCreature);
}

void AddSC_boss_deathbringer_saurfang()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_deathbringer_saurfang";
    newscript->GetAI = &GetAI_boss_deathbringer_saurfang;
    newscript->RegisterSelf();
}
