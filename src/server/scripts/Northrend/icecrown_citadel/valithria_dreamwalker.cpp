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

enum Yells
{
	SAY_AGGRO                     = -1666063,  
	SAY_BELOW_25                  = -1666066,
	SAY_ABOVE_75                  = -1666065, 
	SAY_DEATH                     = -1666067, 
	SAY_PDEATH                    = -1666068, 
	SAY_END                       = -1666070,
	SAY_BERSERK                   = -1666069,
	SAY_OPEN_PORTAL               = -1666064,
};

enum Spells 
{
	SPELL_CORRUPTION                 = 70904, 
	SPELL_DREAM_SLIP                 = 71196, 
	SPELL_RAGE                       = 71189,
	SPELL_VOLLEY                     = 70759, 
	SPELL_COLUMN                     = 70702, 
	SPELL_MANA_VOID                  = 71085,
	SPELL_CORRUPTING                 = 70602,
	SPELL_WASTE                      = 69325, 
	SPELL_FIREBALL                   = 70754,
	SPELL_SUPRESSION                 = 70588, 
	SPELL_CORROSION                  = 70751, 
	SPELL_BURST                      = 70744, 
	SPELL_SPRAY                      = 71283,  
	SPELL_ROT                        = 72963, 
	SPELL_DREAM_STATE                = 70766, 
	SPELL_PORTAL_VISUAL              = 71304,
	SPELL_VIGOR                      = 70873, 
	SPELL_CLOUD_VISUAL               = 70876, 
};

enum Creatures
{      
	CREATURE_WORM                       = 37907, 
	CREATURE_PORTAL                     = 37945, 
	CREATURE_CLOUD                      = 37985,   
	CREATURE_VOID                       = 38068,  
	CREATURE_ABOMINATION                = 37886, 
	CREATURE_SKELETON                   = 36791, 
	CREATURE_ARCHMAGE                   = 37868, 
	CREATURE_SUPPRESSER                 = 37863, 
	CREATURE_ZOMBIE                     = 37934,  
	CREATURE_COMBAT_TRIGGER             = 38752, 
};

const Position Pos[4] =
{
	{4166.216797f, 2564.197266f, 364.873047f},
	{4240.688477f, 2405.794678f, 364.868591f},
	{4165.112305f, 2405.872559f, 364.872925f},
	{4239.579102f, 2566.753418f, 364.868439f},

};

Unit* pValithria;  
Unit* pPlayer; 
Unit* pBuff;  

Creature* combat_trigger= NULL;

struct boss_valithriaAI : public ScriptedAI
{
	boss_valithriaAI(Creature* pCreature) : ScriptedAI(pCreature), summons(me)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 Phase;
	uint32 m_uiPortalTimer;  
	uint32 m_uiEndTimer; 
	uint32 m_uiSummonTimer;
	uint32 m_uiEnd2Timer;

	SummonList summons;

	bool valithria;
	bool end;  
	bool ABOVEHP; 
	bool BELOWHP;

	void InitializeAI()
	{    
		Phase = 0;
		DoCast(SPELL_CORRUPTION);   
		me->SetHealth(me->GetMaxHealth()/2);  

		m_uiSummonTimer = 999999999;
		m_uiPortalTimer = 999999999; 
		valithria = false;
		end = false;
		ABOVEHP = false; 
		BELOWHP = false;

		ScriptedAI::InitializeAI();
	} 

	void MoveInLineOfSight(Unit *who)
	{
		if (!valithria && who->IsWithinDistInMap(me, 40.0f))
		{
			DoScriptText(SAY_AGGRO, me); 
			combat_trigger = me->SummonCreature(CREATURE_COMBAT_TRIGGER, me->GetPositionX(), me->GetPositionY(),me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);  
			me->AddThreat(combat_trigger, 10000000.0f); 
			combat_trigger->AddThreat(me, 10000000.0f);
			me->AI()->AttackStart(combat_trigger);  
			combat_trigger->AI()->AttackStart(me);
			end = false;
			valithria = true;  
			ABOVEHP = false; 
			BELOWHP = false;

			ScriptedAI::MoveInLineOfSight(who);
		}
	}

	void EnterCombat(Unit *who) 
	{
		me->SetHealth(me->GetMaxHealth() * 0.50);                 
		m_uiSummonTimer = 15000;
		m_uiPortalTimer = 30000;  
		Phase = 1;
	}

	void JustSummoned(Creature* pSummoned)
	{
		if (pSummoned && !pSummoned->HasAura(SPELL_PORTAL_VISUAL))
			pSummoned->AI()->AttackStart(me); 
	} 

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (!m_pInstance || m_pInstance->GetData(DATA_VALITHRIA_DREAMWALKER_EVENT) != IN_PROGRESS) 
			summons.DespawnAll();

		if (Phase == 1)
		{
			DoStartNoMovement(me->getVictim()); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
			m_uiEndTimer = 999999999; 

			if (m_uiSummonTimer <= diff) 
			{
				if (getDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
				{
					DoSummon(CREATURE_ZOMBIE, Pos[RAND(0,4)]);  
					DoSummon(CREATURE_SKELETON, Pos[RAND(0,4)]);  
					DoSummon(CREATURE_ARCHMAGE, Pos[RAND(0,4)]);  
					DoSummon(CREATURE_SUPPRESSER, Pos[RAND(0,4)]);  
					DoSummon(CREATURE_ABOMINATION, Pos[RAND(0,4)]);
				}

				if (getDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
				{
					DoSummon(CREATURE_ZOMBIE, Pos[RAND(0,1,2,3)]);  
					DoSummon(CREATURE_SKELETON, Pos[RAND(0,1,2,3)]);  
					DoSummon(CREATURE_ARCHMAGE, Pos[RAND(0,1,2,3)]);  
					DoSummon(CREATURE_SUPPRESSER, Pos[RAND(0,1,2,3)]);  
					DoSummon(CREATURE_ABOMINATION, Pos[RAND(0,1,2,3)]);
				}

				m_uiSummonTimer = 20000;
			} else m_uiSummonTimer -= diff;

			if (m_uiPortalTimer <= diff) 
			{
				DoScriptText(SAY_OPEN_PORTAL, me);
				me->SummonCreature(CREATURE_PORTAL, me->GetPositionX()+15, me->GetPositionY()+15, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000);  
				me->SummonCreature(CREATURE_PORTAL, me->GetPositionX()+10, me->GetPositionY()+25, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000);  
				me->SummonCreature(CREATURE_PORTAL, me->GetPositionX()+15, me->GetPositionY()-25, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000);   
				m_uiPortalTimer = 30000;
			} else m_uiPortalTimer -= diff;

			if (!ABOVEHP && (me->GetHealth()*100 / me->GetMaxHealth()) > 75) 
			{
				DoScriptText(SAY_ABOVE_75, me); 
				ABOVEHP = true; 
			}

			if (!BELOWHP && (me->GetHealth()*100 / me->GetMaxHealth()) < 25) 
			{
				DoScriptText(SAY_BELOW_25, me); 
				BELOWHP = true; 
			}

			if ((me->GetHealth()*100 / me->GetMaxHealth()) > 99)
			{
				Phase = 2; 
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);  
				me->SetReactState(REACT_PASSIVE);
				me->RemoveAurasDueToSpell(SPELL_CORRUPTION);

				end = true; 
			} 
		}

		if (Phase == 2)
		{
			Phase = 3; 
			m_uiEndTimer = 1000; 
			m_uiEnd2Timer = 6000;
			DoScriptText(SAY_END, me); 
		}

		if (Phase == 3) 
		{
			if (m_uiEndTimer <= diff) 
			{
				Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0);
				if(pTarget && !pTarget->IsFriendlyTo(me))
				{
					DoScriptText(SAY_BERSERK , me); 
					DoCast(pTarget, SPELL_RAGE); 
				}
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				m_uiEndTimer = 50000;         
			} else m_uiEndTimer -= diff; 

			if (m_uiEnd2Timer <= diff)
			{
				combat_trigger->ForcedDespawn(); 
				me->CastSpell(me, SPELL_DREAM_SLIP, true, 0, 0, 0); 
				m_uiEnd2Timer = 50000; 
			} else m_uiEnd2Timer -= diff;
		}

		if (me->HasAura(SPELL_DREAM_SLIP))
		{
			me->ForcedDespawn(); 

			if (m_pInstance)
				m_pInstance->SetData(DATA_VALITHRIA_DREAMWALKER_EVENT, DONE);  
		}
	}

	void JustDied(Unit* killer)
	{
		DoScriptText(SAY_DEATH, me);

		if (m_pInstance)
			m_pInstance->SetData(DATA_VALITHRIA_DREAMWALKER_EVENT, FAIL);  
	}
}; 

struct npc_dreamportal_iccAI : public ScriptedAI
{
	npc_dreamportal_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiStateTimer; 

	void InitializeAI() 
	{ 
		DoCast(SPELL_PORTAL_VISUAL);      
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);  

		ScriptedAI::InitializeAI();
	} 

	void MoveInLineOfSight(Unit *who)
	{ 
		if (who->IsControlledByPlayer())
		{
			if (me->IsWithinDistInMap(who,5.0f))
			{   
				pPlayer = who; 
				pPlayer->CastSpell(pPlayer, SPELL_DREAM_STATE, false, 0, 0, 0);
				pPlayer->AddUnitMovementFlag(MOVEMENTFLAG_FLY_MODE);
				pPlayer->SendMovementFlagUpdate();  
				m_uiStateTimer = 15000;    
				me->ForcedDespawn();
			}  
		}
	} 

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;    

		if (m_uiStateTimer <= diff) 
		{  
			pPlayer->RemoveAurasDueToSpell(SPELL_DREAM_STATE);
			pPlayer->RemoveUnitMovementFlag(MOVEMENTFLAG_FLY_MODE); 
			pPlayer->SendMovementFlagUpdate();
		} else m_uiStateTimer -= diff;   
	}
};

struct npc_skellmage_iccAI : public ScriptedAI
{
	npc_skellmage_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;    

	uint32 m_uiVolleyTimer; 
	uint32 m_uiColumnTimer;  
	uint32 m_uiVoidTimer;

	void EnterCombat(Unit* who)
	{  
		m_uiVolleyTimer = 12000; 
		m_uiColumnTimer = 20000; 
		m_uiVoidTimer = 30000;

		if (m_pInstance && m_pInstance->GetData(DATA_VALITHRIA_DREAMWALKER_EVENT != IN_PROGRESS))
			m_pInstance->SetData(DATA_VALITHRIA_DREAMWALKER_EVENT, IN_PROGRESS);  
	}

	void KilledUnit(Unit* victim)
	{
		DoScriptText(SAY_PDEATH, pValithria);
	}

	void Reset()
	{
		if (m_pInstance) 
			m_pInstance->SetData(DATA_VALITHRIA_DREAMWALKER_EVENT, NOT_STARTED);   
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;   

		if (m_uiVolleyTimer <= diff)
		{
			Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
			DoCast(pTarget, SPELL_VOLLEY);
			m_uiVolleyTimer = 15000; 
		} else m_uiVolleyTimer -= diff;  

		if (m_uiVoidTimer <= diff)
		{
			if(Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
			{
			    me->SummonCreature(CREATURE_VOID, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000);   
			    m_uiVoidTimer = 30000; 
			}
		} else m_uiVoidTimer -= diff; 

		if (m_uiColumnTimer <= diff)
		{
			Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
			DoCast(pTarget, SPELL_COLUMN);
			m_uiColumnTimer = 20000; 
		} else m_uiColumnTimer -= diff; 

		DoMeleeAttackIfReady(); 
	} 
};

struct npc_fireskell_iccAI : public ScriptedAI
{
	npc_fireskell_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiWasteTimer; 
	uint32 m_uiFireballTimer;  

	void EnterCombat(Unit* who)
	{  
		m_uiWasteTimer = 20000; 
		m_uiFireballTimer = 5000; 
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;   

		if (m_uiWasteTimer <= diff)
		{
			DoCast(SPELL_WASTE);
			m_uiWasteTimer = 20000; 
		} else m_uiWasteTimer -= diff; 

		if (m_uiFireballTimer <= diff)
		{
			Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
			DoCast(pTarget, SPELL_FIREBALL);
			m_uiFireballTimer = 5000; 
		} else m_uiFireballTimer -= diff; 

		DoMeleeAttackIfReady(); 
	}

	void KilledUnit(Unit* pVictim)
	{
		DoScriptText(SAY_PDEATH, pValithria);
	}
}; 
struct npc_suppressor_iccAI : public ScriptedAI
{
	npc_suppressor_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiCheckTimer; 


	void EnterCombat(Unit* who)
	{ 
		me->SetReactState(REACT_PASSIVE);  
		m_uiCheckTimer = 2500;
	} 

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;   

		if (m_uiCheckTimer <= diff) 
		{ 
			me->CastSpell(pValithria, SPELL_SUPRESSION, true, 0, 0, 0);  
			m_uiCheckTimer = 100000;  
		} else m_uiCheckTimer -= diff; 
	}
}; 

struct npc_manavoid_iccAI : public ScriptedAI
{
	npc_manavoid_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	void InitializeAI() 
	{   
		DoCast(SPELL_MANA_VOID);
	}

	void EnterCombat(Unit* who)
	{   
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		DoStartNoMovement(me->getVictim()); 
	}
};  
struct npc_glutabomination_iccAI : public ScriptedAI
{  
	npc_glutabomination_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiSprayTimer;  

	void EnterCombat(Unit* who)
	{ 
		m_uiSprayTimer = 10000; 
	} 

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;   

		if (m_uiSprayTimer <= diff)
		{
			DoCast(me, SPELL_SPRAY);
			m_uiSprayTimer = 20000;
		} else m_uiSprayTimer -= diff;  

		DoMeleeAttackIfReady();  
	}   

	void KilledUnit(Unit* pVictim)
	{
		DoScriptText(SAY_PDEATH, pValithria);
	}

	void JustDied(Unit* killer)  
	{
		uint32 count = RAID_MODE(8,8,8,8);
		for (uint8 i = 1; i <= count; i++)
		{
			me->SummonCreature(CREATURE_WORM, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN);       
		}
	}

}; 
struct npc_blistzombie_iccAI : public ScriptedAI
{
	npc_blistzombie_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiBurstTTimer;  
	uint32 m_uiDelayTimer;  

	void EnterCombat(Unit* who) 
	{ 
		m_uiBurstTTimer = 20000;  
		m_uiDelayTimer = 99999; 
	} 

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;     

		if (m_uiBurstTTimer <= diff)
		{
			DoCast(SPELL_BURST); 
			m_uiBurstTTimer = 20000;
			m_uiDelayTimer  = 1000;
		} else m_uiBurstTTimer -= diff;   

		if (m_uiDelayTimer  <= diff)
		{
			me->ForcedDespawn();
			m_uiDelayTimer  = 100000;
		} else m_uiDelayTimer  -= diff;  

		DoMeleeAttackIfReady();  
	}  

	void KilledUnit(Unit* victim)
	{
		DoScriptText(SAY_PDEATH, pValithria);
	}
};
struct npc_dreamcloud_iccAI : public ScriptedAI
{
	npc_dreamcloud_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiSpawnTimer;
	uint32 m_uiDelayTimer;

	void InitializeAI() 
	{
		DoCast(SPELL_CLOUD_VISUAL);
		me->AddUnitMovementFlag(MOVEMENTFLAG_FLY_MODE);  
		me->SendMovementFlagUpdate();   
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
	} 

	void JustRespawned() 
	{ 
		DoCast(SPELL_CLOUD_VISUAL);
		me->AddUnitMovementFlag(MOVEMENTFLAG_FLY_MODE);  
		me->SendMovementFlagUpdate();   
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
	}
	void MoveInLineOfSight(Unit *who)
	{  
		if (me->IsWithinDistInMap(who, 5.0f))
		{
			DoCast(SPELL_VIGOR);  
			m_uiDelayTimer = 100;
		}
	} 

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;    

		if (m_uiDelayTimer <= diff) 
		{
			me->ForcedDespawn(); 
		} else m_uiDelayTimer -= diff;
	}

};

CreatureAI* GetAI_boss_valithria(Creature* pCreature)
{
	return new boss_valithriaAI (pCreature);
}  

CreatureAI* GetAI_npc_dreamcloud_icc(Creature* pCreature)
{
	return new npc_dreamcloud_iccAI (pCreature);
}

CreatureAI* GetAI_npc_skellmage_icc(Creature* pCreature)
{
	return new npc_skellmage_iccAI (pCreature);
} 

CreatureAI* GetAI_npc_fireskell_icc(Creature* pCreature)
{
	return new npc_fireskell_iccAI (pCreature);
} 

CreatureAI* GetAI_npc_dreamportal_icc(Creature* pCreature)
{
	return new npc_dreamportal_iccAI (pCreature);
} 

CreatureAI* GetAI_npc_suppressor_icc(Creature* pCreature)
{
	return new npc_suppressor_iccAI (pCreature);
}  

CreatureAI* GetAI_npc_glutabomination_icc(Creature* pCreature)
{
	return new npc_glutabomination_iccAI (pCreature);
} 

CreatureAI* GetAI_npc_manavoid_icc(Creature* pCreature)
{
	return new npc_manavoid_iccAI (pCreature);
} 

CreatureAI* GetAI_npc_blistzombie_icc(Creature* pCreature)
{
	return new npc_blistzombie_iccAI (pCreature);
}

void AddSC_boss_valithria()
{
	Script *newscript;
	newscript = new Script;
	newscript->Name="boss_valithria";
	newscript->GetAI = &GetAI_boss_valithria;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="npc_skellmage_icc";
	newscript->GetAI = &GetAI_npc_skellmage_icc;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="npc_fireskell_icc";
	newscript->GetAI = &GetAI_npc_fireskell_icc;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="npc_dreamportal_icc";
	newscript->GetAI = &GetAI_npc_dreamportal_icc;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="npc_suppressor_icc";
	newscript->GetAI = &GetAI_npc_suppressor_icc;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="npc_manavoid_icc";
	newscript->GetAI = &GetAI_npc_manavoid_icc;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="npc_glutabomination_icc";
	newscript->GetAI = &GetAI_npc_glutabomination_icc;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="npc_blistzombie_icc";
	newscript->GetAI = &GetAI_npc_blistzombie_icc;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="npc_dreamcloud_icc";
	newscript->GetAI = &GetAI_npc_dreamcloud_icc;
	newscript->RegisterSelf();
}