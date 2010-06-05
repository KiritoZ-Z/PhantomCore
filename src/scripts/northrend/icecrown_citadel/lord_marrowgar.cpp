/*
 * Copyright (C) 2009 - 2010 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 Phantom Project <http://phantom-project.org/>
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
#include "ScriptedPch.h"
#include "icecrown_citadel.h"

enum Yells
{
	SAY_ENTER_ZONE       =    -1665892,
	SAY_AGGRO            =    -1665893,
	SAY_BONE_STORM       =    -1665894,
	SAY_BONESPIKE_1      =    -1665895,
	SAY_BONESPIKE_2      =    -1665896,
	SAY_BONESPIKE_3      =    -1665897,
	SAY_SLAY_1           =    -1665898,
	SAY_SLAY_2           =    -1665899,
	SAY_DEATH            =    -1665900,
	SAY_ENRAGE           =    -1665901,
	SAY_EMOTE_BONE_STORM =	  -1666901,
};

enum Spells
{
	SPELL_SABER_SLASH_10_NORMAL	= 71021,
	SPELL_SABER_SLASH_25_NORMAL = 71021,
	SPELL_SABER_SLASH_10_HEROIC = 70814,
	SPELL_SABER_SLASH_25_HEROIC = 70814,
	SPELL_COLD_FLAME_10_NORMAL = 69146,
	SPELL_COLD_FLAME_25_NORMAL = 70823,
	SPELL_COLD_FLAME_10_HEROIC = 70824,
	SPELL_COLD_FLAME_25_HEROIC = 70825,
	SPELL_COLD_FLAME_SPAWN	   = 69138,
	SPELL_BONE_SPIKE_GRAVEYARD = 69057,
	SPELL_BONE_SPIKE_IMPALING  = 69065,
	SPELL_BONE_STORM_CHANNEL	=	69076,
	SPELL_BONE_STORM_10_NORMAL	=	69075,
	SPELL_BONE_STORM_25_NORMAL	=	70835,
	SPELL_BONE_STORM_10_HEROIC	=	70834,
	SPELL_BONE_STORM_25_HEROIC	=	70836,
	SPELL_BERSERK				=   47008,
};

enum NPC
{
	CREATURE_BONE_SPIKE	=	38711,
	CREATURE_COLD_FLAME	=	36672,
};

struct BoneSpikeAI : public Scripted_NoMovementAI
{
    BoneSpikeAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
    {
        BoneSpikeGUID = 0;
    }

    uint64 BoneSpikeGUID;

    void SetPrisoner(Unit* uPrisoner)
    {
        BoneSpikeGUID = uPrisoner->GetGUID();
    }

    void Reset()
	{ 
		BoneSpikeGUID = 0; 
	}

    void JustDied(Unit *killer)
    {
        if (killer->GetGUID() != me->GetGUID())

        if (BoneSpikeGUID)
        {
            Unit* Bone = Unit::GetUnit((*me), BoneSpikeGUID);
            if (Bone)
                Bone->RemoveAurasDueToSpell(SPELL_BONE_SPIKE_IMPALING);
        }
    }

	void KilledUnit(Unit *victim)
    {
        if (victim->GetGUID() != me->GetGUID())

        if (BoneSpikeGUID)
        {
            Unit* Bone = Unit::GetUnit((*me), BoneSpikeGUID);
            if (Bone)
                Bone->RemoveAurasDueToSpell(SPELL_BONE_SPIKE_IMPALING);
        }
    }

    void UpdateAI(const uint32 diff)
    {
	  Unit* temp = Unit::GetUnit((*me),BoneSpikeGUID);
        if ((temp && temp->isAlive() && !temp->HasAura(SPELL_BONE_SPIKE_IMPALING)) || !temp)
            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }
};

struct Boss_Lord_MarrowgarAI : public ScriptedAI
{
    Boss_Lord_MarrowgarAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
     pInstance = pCreature->GetInstanceData();
    }
    
    ScriptedInstance* pInstance;

	uint32 m_uiSaberSlashTimer;
	uint32 m_uiBoneSpikeGraveyardTimer;
	uint32 m_uiColdFlameTimer;
	uint32 m_uiBoneStormChannelTimer; 
	uint32 m_uiBoneStormDamageTimer;
	uint32 m_uiColdFlameTimer2;
	uint32 m_uiBerserkTimer;

	bool Intro;

    void Reset()
    {
		m_uiBoneSpikeGraveyardTimer	= 15000;
		m_uiColdFlameTimer			= 7000;
		m_uiBoneStormChannelTimer	= 45000;
		m_uiColdFlameTimer2			= 4000;
		m_uiBoneStormDamageTimer = urand(1500,2500);
		m_uiBerserkTimer = 500000;

		me->SetSpeed(MOVE_RUN, 2.0f, true);
		me->SetSpeed(MOVE_WALK, 2.0f, true);

		Intro = false;

		if (pInstance)
            pInstance->SetData(DATA_MARROWGAR_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
		DoScriptText(SAY_AGGRO, me);

		if (pInstance)
            pInstance->SetData(DATA_MARROWGAR_EVENT, IN_PROGRESS);
    }

	void JustDied(Unit* killer)
    {  
		DoScriptText(SAY_DEATH, me);

		if (pInstance)
            pInstance->SetData(DATA_MARROWGAR_EVENT, DONE);
    }

	void KilledUnit(Unit *victim)
    {
        DoScriptText(RAND(SAY_SLAY_1,SAY_SLAY_2), me);
    }

	 void MoveInLineOfSight(Unit *who)
     {
        if (!Intro && who->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(SAY_ENTER_ZONE, me);
            Intro = true;
        }
        ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;
	
		
		if (m_uiBerserkTimer <= uiDiff)
		{
			DoScriptText(SAY_ENRAGE, me);
			DoCast(me, SPELL_BERSERK);
			m_uiBerserkTimer = 600000;
		} 
		else m_uiBerserkTimer -= uiDiff;

		if (!me->HasAura(SPELL_BONE_STORM_CHANNEL))
		{
		if(m_uiSaberSlashTimer <= uiDiff)
		{
			DoCast(me->getVictim(), RAID_MODE(SPELL_SABER_SLASH_10_NORMAL,SPELL_SABER_SLASH_25_NORMAL,SPELL_SABER_SLASH_10_HEROIC,SPELL_SABER_SLASH_10_HEROIC));
			m_uiSaberSlashTimer = urand(7000,14000);
		} 
		else m_uiSaberSlashTimer -= uiDiff;

		if (m_uiColdFlameTimer <= uiDiff)
		{
			if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1))
			{
			    if (!me->IsWithinDistInMap(pTarget, 4))
			    {
				DoCast(pTarget, SPELL_COLD_FLAME_SPAWN);
				m_uiColdFlameTimer = 7000;
			    } 
			}
		} 
		else m_uiColdFlameTimer -= uiDiff;

		if(m_uiBoneStormChannelTimer <= uiDiff)
		{
			DoCastAOE(SPELL_BONE_STORM_CHANNEL);
			if (AuraApplication * aur = me->GetAuraApplication(69076, GetGUID()))
			{
                aur->GetBase()->SetDuration(21000);
                aur->GetBase()->SetMaxDuration(21000);
                aur->ClientUpdate();
			}
			DoScriptText(SAY_BONE_STORM, me);
			DoScriptText(SAY_EMOTE_BONE_STORM, me);
			m_uiBoneStormChannelTimer = 50000;
		} 
		else m_uiBoneStormChannelTimer -= uiDiff;
		
		if (m_uiBoneSpikeGraveyardTimer < uiDiff)
		{
		    switch(urand(0, 2))
		    {
			case 0: DoScriptText(SAY_BONESPIKE_1, me); break;
			case 1: DoScriptText(SAY_BONESPIKE_2, me); break;
			case 2: DoScriptText(SAY_BONESPIKE_3, me); break;
		    }
		    uint32 count = RAID_MODE(1,3,1,3); 
                    for (uint8 i = 1; i <= count; i++)
                    {
			Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1);
			if (pTarget && !pTarget->HasAura(SPELL_BONE_SPIKE_IMPALING))
			{
			    Creature* Bone = me->SummonCreature(CREATURE_BONE_SPIKE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 999999);
				CAST_AI(BoneSpikeAI, Bone->AI())->SetPrisoner(pTarget);
				Bone->CastSpell(pTarget, SPELL_BONE_SPIKE_IMPALING, true);
			}
		    } 
		    m_uiBoneSpikeGraveyardTimer = 15000;
               }
               else m_uiBoneSpikeGraveyardTimer -= uiDiff;

		}

			if (me->HasAura(SPELL_BONE_STORM_CHANNEL))
			{
			    if(m_uiColdFlameTimer2 <= uiDiff)
				{
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1))
					{
					    if (!me->IsWithinDistInMap(pTarget, 4))
					    { 
						me->SummonCreature(CREATURE_COLD_FLAME, me->GetPositionX()+20, me->GetPositionY()+20, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000);
						me->SummonCreature(CREATURE_COLD_FLAME, me->GetPositionX()-20, me->GetPositionY()-20, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000);
						me->SummonCreature(CREATURE_COLD_FLAME, me->GetPositionX()+20, me->GetPositionY()-20, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000);
						me->SummonCreature(CREATURE_COLD_FLAME, me->GetPositionX()-20, me->GetPositionY()+20, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000);
						DoResetThreat();
						me->AddThreat(pTarget, 5000000.0f);
						m_uiColdFlameTimer2 = 4000;
					    }
					}
				} 
				else m_uiColdFlameTimer2 -= uiDiff;

				if (getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
				if (m_uiBoneSpikeGraveyardTimer < uiDiff)
				{
					switch(urand(0, 2))
					{
						case 0: DoScriptText(SAY_BONESPIKE_1, me); break;
						case 1: DoScriptText(SAY_BONESPIKE_2, me); break;
						case 2: DoScriptText(SAY_BONESPIKE_3, me); break;
					}
					uint32 count = RAID_MODE(1,3,1,3);
					for (uint8 i = 1; i <= count; i++)
					{
					Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 200, true);
					if (pTarget && !pTarget->HasAura(SPELL_BONE_SPIKE_IMPALING))
					{
						Creature* Bone = me->SummonCreature(CREATURE_BONE_SPIKE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 999999);
						CAST_AI(BoneSpikeAI, Bone->AI())->SetPrisoner(pTarget);
						Bone->CastSpell(pTarget, SPELL_BONE_SPIKE_IMPALING, true);
					}
				}
				m_uiBoneSpikeGraveyardTimer = 15000;
            }
            else m_uiBoneSpikeGraveyardTimer -= uiDiff;

				if(m_uiBoneStormDamageTimer <= uiDiff)
				{
					DoCast(me, RAID_MODE(SPELL_BONE_STORM_10_NORMAL,SPELL_BONE_STORM_25_NORMAL,SPELL_BONE_STORM_10_HEROIC,SPELL_BONE_STORM_25_HEROIC));
					m_uiBoneStormDamageTimer = 1000;
				} 
				else m_uiBoneStormDamageTimer -= uiDiff;
			}

		DoMeleeAttackIfReady();
	}
};

struct Cold_FlameAI : public ScriptedAI
{
    Cold_FlameAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

	ScriptedInstance* m_pInstance;

	uint32 m_uiColdFlameTimer;
	uint32 despawn_timer, despawn_timer_h;
	uint32 amovie_timer;

    void Reset()
    {
        despawn_timer = 20000;
	despawn_timer_h = 35000;
	amovie_timer = 7000;
        float x, y, z;
        me->GetNearPoint(me, x, y, z, 1, 100, M_PI*2*rand_norm());
        me->GetMotionMaster()->MovePoint(0, x, y, z);
        me->SetVisibility(VISIBILITY_OFF);
		DoCast(me, RAID_MODE(SPELL_COLD_FLAME_10_NORMAL,SPELL_COLD_FLAME_25_NORMAL,SPELL_COLD_FLAME_10_HEROIC,SPELL_COLD_FLAME_25_HEROIC));
		me->SetReactState(REACT_PASSIVE);
		me->SetSpeed(MOVE_WALK, 1.3f, true);
		m_uiColdFlameTimer = 700;
    }

	void UpdateAI(const uint32 diff)
    {
    
	if (amovie_timer <= diff)
	{
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		amovie_timer = 7500;
	}
	else amovie_timer -= diff;
        if (despawn_timer <= diff && (getDifficulty() != RAID_DIFFICULTY_10MAN_HEROIC || getDifficulty() != RAID_DIFFICULTY_25MAN_HEROIC))
        {
	        me->DisappearAndDie();
                me->ForcedDespawn();
	        despawn_timer = 25000;
        } 
        else despawn_timer -= diff;
	    
	    if (getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
	    {
	        if (despawn_timer_h <= diff)
	        {
		        me->DisappearAndDie();
		        me->ForcedDespawn();
		        despawn_timer_h = 30000;
	        } 
	        else
		        despawn_timer_h -= diff;
    	    
	    }
	
		if(m_uiColdFlameTimer <= diff)
		{
			DoCast(me, RAID_MODE(SPELL_COLD_FLAME_10_NORMAL,SPELL_COLD_FLAME_25_NORMAL,SPELL_COLD_FLAME_10_HEROIC,SPELL_COLD_FLAME_25_HEROIC));
			m_uiColdFlameTimer = 1000;
		} 
		else m_uiColdFlameTimer -= diff;
    }
		
};

CreatureAI* GetAI_Cold_Flame(Creature* pCreature)
{
    return new Cold_FlameAI(pCreature);
}

CreatureAI* GetAI_Boss_Lord_Marrowgar(Creature* pCreature)
{
    return new Boss_Lord_MarrowgarAI(pCreature);
}

CreatureAI* GetAI_BoneSpike(Creature* pCreature)
{
    return new BoneSpikeAI(pCreature);
}

void AddSC_boss_marrowgar()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "Boss_Lord_Marrowgar";
    NewScript->GetAI = &GetAI_Boss_Lord_Marrowgar;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "Cold_Flame";
    NewScript->GetAI = &GetAI_Cold_Flame;
    NewScript->RegisterSelf();	

	NewScript = new Script;
    NewScript->Name = "BoneSpike";
    NewScript->GetAI = &GetAI_BoneSpike;
    NewScript->RegisterSelf();	
}
