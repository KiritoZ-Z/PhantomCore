#include "ScriptedPch.h"
#include "def.h"

enum Spells
{
acidicspew = 66819,
slimepool  = 66883,
paralyticbite = 66824,
acidspit = 66880,
sweep = 66794,
paralyticspray = 66901,
enrage = 68335,
Emerge = 449,
SubMerge = 373,
burningbite = 66879,
moltenspew = 66820,
firespit = 66796,
burningspray = 66902
};
 
struct  npc_dreadscaleAI : public ScriptedAI
{
	npc_dreadscaleAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
	pInstance = pCreature->GetInstanceData();
	//me->GetMotionMaster()->MovePoint(0, 563.539551, 213.565628, 395.098206);
	Reset();
	}
	ScriptedInstance* pInstance;
 
	uint32 acidicspew_timer; //phase 1  stationary first
	uint32 slimepool_timer;
	uint32 paralyticbite_timer;
	uint32 acidspit_timer;//phase 2
	uint32 sweep_timer;
	uint32 paralyticspray_timer;
	uint32 SpawnTimer;
	bool once;
	bool Swap;
	bool Asidmaw;

 
	void Reset()
{
	if (pInstance)
            pInstance->SetData(DATA_BOSS_DREADSCALE, NOT_STARTED);
	me->GetMotionMaster()->MovePoint(0, 563.771, 172.368, 394.315);
	acidicspew_timer = 5400;
	slimepool_timer = 10000;
	paralyticbite_timer =  4000;
 
	paralyticspray_timer = 12000;
	sweep_timer =  7000;
	acidspit_timer = 16000;
	SpawnTimer = 15000;
	Asidmaw = true;

 
 
 
}
 
void Aggro(Unit *who)
	{
	if (pInstance)
            pInstance->SetData(DATA_BOSS_DREADSCALE, IN_PROGRESS);
	}
 
void UpdateAI(const uint32 diff )
	{
	if (!UpdateVictim())
		return;
		
		if (Asidmaw && SpawnTimer < diff)
		{
		me->MonsterSay("А вот и мой напарничек!", LANG_UNIVERSAL, 0);
		if (Creature *pBoss =me->SummonCreature(BOSS_ASIDMAW,538.347, 165.509, 394.6500, 5.431659,TEMPSUMMON_CORPSE_DESPAWN, 4000))
		{
		if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
		pBoss->AI()->AttackStart(pTarget);
		}
		Asidmaw = false;
		SpawnTimer = 15000;
		}else SpawnTimer -=diff;

		if(paralyticspray_timer < diff)      //para spray
		{
		me->MonsterSay("SPRAYYYYYYY", LANG_UNIVERSAL, 0);
		DoCast(me->getVictim(),paralyticspray);
		paralyticspray_timer = 12000;
		}else paralyticspray_timer -= diff;
 
		if(sweep_timer < diff)           //sweep
		{	
		DoCast(me->getVictim(),sweep);
		sweep_timer =   7000;
		me->MonsterSay("SWEEP!!", LANG_UNIVERSAL, 0);
		}else sweep_timer -= diff;
 
		if(acidspit_timer < diff)           //acid spit
		{	
		DoCast(me->getVictim(),acidspit);
		acidspit_timer = 16000;
		me->MonsterSay("Acid Spit!!", LANG_UNIVERSAL, 0);
		}else acidspit_timer -= diff;

	DoMeleeAttackIfReady();
	}
    void JustDied(Unit* Killer)
    {
        if (pInstance)
            pInstance->SetData(DATA_BOSS_DREADSCALE, DONE);
    }
};
 
 
struct  npc_acidmawAI : public ScriptedAI
{
npc_acidmawAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 EndPhaseTwo;
	uint32 acidicspew_timer; //phase 1  stationary first
	uint32 slimepool_timer;
	uint32 paralyticbite_timer;
	uint32 acidspit_timer;//phase 2
	uint32 sweep_timer;
	uint32 paralyticspray_timer;
 
	void Reset()
{
 
	acidicspew_timer = 5400;
	slimepool_timer = 10000;
	paralyticbite_timer =  4000;
 
	paralyticspray_timer = 12000;
	acidspit_timer = 16000;
 
}
 
void Aggro(Unit *who)
	{
		me->MonsterSay("Me Hungry!", LANG_UNIVERSAL, 0);
 
	}
 
void UpdateAI(const uint32 diff )
	{
	if (!UpdateVictim())
		return;
 
				if( paralyticbite_timer < diff)      //para bite
				{
					me->MonsterSay("BITTTTEEEEE", LANG_UNIVERSAL, 0);
					DoCast(me->getVictim(), paralyticbite);
					 paralyticbite_timer = 30000;
				}else  paralyticbite_timer -= diff;
 
				if(slimepool_timer < diff)           //slimepool
				{	
					DoCast(me->getVictim(),slimepool);
					slimepool_timer = 10000;
					me->MonsterSay("POOOOLLLL OF SLIME!!!", LANG_UNIVERSAL, 0);
				}else slimepool_timer -= diff;
 
				if(acidicspew_timer < diff)           //acidicspew
				{	
					DoCast(me->getVictim(),acidicspew);
					acidicspew_timer = 10000;
					me->MonsterSay("Acid spewwwwww!!", LANG_UNIVERSAL, 0);
				}else acidicspew_timer -= diff;

				if(paralyticspray_timer < diff)      //para spray
				{
					me->MonsterSay("SPRAYYYYYYY", LANG_UNIVERSAL, 0);
					DoCast(me->getVictim(),paralyticspray);
					paralyticspray_timer = 12000;
				}else paralyticspray_timer -= diff;
 
				if(sweep_timer < diff)           //sweep
				{	
					DoCast(me->getVictim(),sweep);
					sweep_timer =   7000;
					me->MonsterSay("SWEEP!!", LANG_UNIVERSAL, 0);
				}else sweep_timer -= diff;
 
				if(acidspit_timer < diff)           //acid spit
				{	
					DoCast(me->getVictim(),acidspit);
					acidspit_timer = 16000;
					me->MonsterSay("Acid Spit!!", LANG_UNIVERSAL, 0);
				}else acidspit_timer -= diff;


	DoMeleeAttackIfReady();
	}
};
 
 
CreatureAI* GetAI_npc_dreadscale(Creature* pCreature)
{
    return new npc_dreadscaleAI(pCreature);
}
 
CreatureAI* GetAI_npc_acidmaw(Creature* pCreature)
{
    return new npc_acidmawAI(pCreature);
}
 
void AddSC_the_two_jormungar()
{
    Script* newscript;
 
    newscript = new Script;
    newscript->Name = "npc_dreadscale";
    newscript->GetAI = &GetAI_npc_dreadscale;
    newscript->RegisterSelf();
 
    newscript = new Script;
    newscript->Name = "npc_acidmaw";
    newscript->GetAI = &GetAI_npc_acidmaw;
    newscript->RegisterSelf();
}
