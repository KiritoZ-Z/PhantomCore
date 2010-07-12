#include "ScriptPCH.h"
#include "def.h"

// Spells LIGHT Fjola
#define SPELL_TWIN_SPIKE_LIGHT    RAID_MODE(66075,67313)
#define SPELL_LIGHT_SOURGE    RAID_MODE(65767,67274)
#define SPELL_SHIELD_OF_LIGHT    RAID_MODE(65858,67259) // to pact ++
#define SPELL_TWINS_PACT_LIGHT    65876 // to sheild ++
#define SPELL_LIGHT_VORTEX    RAID_MODE(66046,67204)

#define SPELL_DARK_ESSENCE    65684    // for player
#define SPELL_LIGHT_ESSENCE    65686   // for player
#define SPELL_POWER_OF_THE_TWINS 65916 // for player

//Eydis Darkbane
#define SPELL_TWINS_SPIKE_DARCK    RAID_MODE(66069,67311)
#define SPELL_DARK_SOURGE    RAID_MODE(65769,67265)        
#define SPELL_SHIELDN_OF_DARKNESS RAID_MODE(65874,67256) // to pact ++    
#define SPELL_TWINS_PACT_DARCK    65875 // to sheild ++
#define SPELL_DARCK_VORTEX    66058

enum
{
SAY_LI_VOR    = -1900538,
SAY_BLIZN    = -1900539,
SAY_AGGRO    = -1900541,
SAY_AGGRO1    = -1900541,
SAY_POGLOT_LI    = -1900542,
SAY_YOU_BEAD    = -1900544,
SAY_HAOS    = -1900545,
SAY_POGLOT_NI    = -1900546,
SAY_DEAD    = -1900547,
SAY_DEAD1    = -1900547,
// NPC
NPC_LIGHT    = 34568,
NPC_BLACK    = 34567,
NPC_CONCENTRATED_LIGHT = 34630,
NPC_CONCENTRATED_DARCNESS = 34628,
// Position
MIN_X    = 524,
MAX_X    = 600,
MIN_Y    = 120,
MAX_Y    = 170,
POSIT_Z    = 395
};

const Position Pos [4] =
{
{593.59,125.66,394.8,3.2},
{587.03,160.64,394.8,1.4},
{544.81,158.36,394.8,5.3},
{567.94,121.70,394.8,1.1}
};

struct boss_twin_fjolaAI : public ScriptedAI
{
    boss_twin_fjolaAI(Creature *c): ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance *pInstance;
    
    bool DontYellWhenDead;

virtual bool IAmVeklor() = 0;
virtual void Reset() = 0;

void TwinReset()
    {
        DontYellWhenDead = false;
    }


Creature *GetOtherBoss()
    {
        if (pInstance)
            return Unit::GetCreature(*me, pInstance->GetData64(IAmVeklor() ? DATA_EYDIS : DATA_FJOLA));
        else
            return NULL;
    }


void DamageTaken(Unit *done_by, uint32 &damage)
    {
        Unit *pOtherBoss = GetOtherBoss();
        if (pOtherBoss)
        {
            float dPercent = ((float)damage) / ((float)me->GetMaxHealth());
            int odmg = (int)(dPercent * ((float)pOtherBoss->GetMaxHealth()));
            int ohealth = pOtherBoss->GetHealth()-odmg;
            pOtherBoss->SetHealth(ohealth > 0 ? ohealth : 0);
            if (ohealth <= 0)
            {
                pOtherBoss->setDeathState(JUST_DIED);
                pOtherBoss->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
        }
    }
    
void KilledUnit(Unit* victim) 
{
}

void JustDied(Unit* Killer)
    {
        Creature *pOtherBoss = GetOtherBoss();
        if (pOtherBoss)
        {
            pOtherBoss->SetHealth(0);
            pOtherBoss->setDeathState(JUST_DIED);
            pOtherBoss->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            CAST_AI(boss_twin_fjolaAI, pOtherBoss->AI())->DontYellWhenDead = true;
        }
        if (!DontYellWhenDead)                              // I hope AI is not threaded
            DoPlaySoundToSet(me, IAmVeklor() ? SAY_DEAD : SAY_DEAD1);
        if (pInstance)
            pInstance->SetData(TYPE_FJOLA, DONE);
    }
    void EnterCombat(Unit *who)
    {

        if (pInstance)
            pInstance->SetData(TYPE_FJOLA, IN_PROGRESS);
        DoPlaySoundToSet(me, IAmVeklor() ? SAY_AGGRO : SAY_AGGRO1);
    }
  
void SpellHit(Unit *caster, const SpellEntry *entry)
    {
        if (caster == me)
            return;

        Creature *pOtherBoss = GetOtherBoss();
        if (!pOtherBoss)
            return;

        // add health so we keep same percentage for both brothers
        uint32 mytotal = me->GetMaxHealth(), histotal = pOtherBoss->GetMaxHealth();
        float mult = ((float)mytotal) / ((float)histotal);
        if (mult < 1)
            mult = 1.0f/mult;
        #define HEAL_BROTHER_AMOUNT 30000.0f
        uint32 largerAmount = (uint32)((HEAL_BROTHER_AMOUNT * mult) - HEAL_BROTHER_AMOUNT);

        uint32 myh = me->GetHealth();
        uint32 hish = pOtherBoss->GetHealth();
        if (mytotal > histotal)
        {
            uint32 h = me->GetHealth()+largerAmount;
            me->SetHealth(std::min(mytotal, h));
        }
        else
        {
            uint32 h = pOtherBoss->GetHealth()+largerAmount;
            pOtherBoss->SetHealth(std::min(histotal, h));
        }
    }
};

struct boss_eydisAI : public boss_twin_fjolaAI
{
    bool IAmVeklor() {return false;}
    boss_eydisAI(Creature *c) : boss_twin_fjolaAI(c) {}

    ScriptedInstance* m_pInstance;

    uint32 TwinsSpikeLightTimer;
    uint32 LightSourgeTimer;
    uint32 ShieldofLightTimer;
    uint32 PoweroftheTwins;
    uint32 LightVortexTimer;
    uint32 SummonLightPortalTimer;
    uint32 SummonLightTimer;
    bool Portallight;

  void Reset()
  {
        pInstance->SetData(TYPE_FJOLA, NOT_STARTED);
        me->GetMotionMaster()->MovePoint(0, 568.174, 172.803, 394.590);
    TwinReset();
    TwinsSpikeLightTimer = 11000;
    LightSourgeTimer = 20000;
    ShieldofLightTimer = 90000;
    PoweroftheTwins    = 89000;
    LightVortexTimer = 65000;
    SummonLightPortalTimer = 1000;
    SummonLightTimer = 10000;
    Portallight = true; 
  }

  void UpdateAI(const uint32 diff)
  {
    if (Portallight && SummonLightPortalTimer <= diff)
    {
     if (Creature* pTrash = me->SummonCreature(NPC_LIGHT, Pos[RAND(0,2)],TEMPSUMMON_CORPSE_DESPAWN,90000))
      {
        pTrash->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTrash->SetReactState(REACT_PASSIVE);
        Portallight = false;
      }
      SummonLightPortalTimer = 1000;
    } else SummonLightPortalTimer -= diff;
    
    if (!UpdateVictim())
     return;
      
     if (SummonLightTimer <= diff)
     {
    Map *pMap = me->GetMap();
 
     if(!pMap)
     return;
 
       Map::PlayerList const &lPlayers = pMap->GetPlayers();
        
         if (lPlayers.isEmpty())
             return;
        
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
          {
            if (Player* pPlayer = itr->getSource())
              {
                float x = pPlayer->GetPositionX() + irand(-30,40);
            float y = pPlayer->GetPositionY() + irand(-30,40);
                        float z = pPlayer->GetPositionZ();
          if (Creature* pLight = me->SummonCreature(NPC_CONCENTRATED_LIGHT,x,y,z,0,TEMPSUMMON_TIMED_DESPAWN,30000))
          {
              pLight->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
              pLight->SetReactState(REACT_PASSIVE);
              pLight->SetSpeed(MOVE_WALK, 0.8f, true);
              pLight->SetSpeed(MOVE_RUN, 0.8f, true);
              pLight->SetSpeed(MOVE_FLIGHT, 0.8f, true);
              pLight->AI()->AttackStart(pPlayer);
              }
          }
 
        }
     SummonLightTimer = 40000;
     } else SummonLightTimer -= diff;
    
    if (TwinsSpikeLightTimer < diff)
    {
      if (Unit *target = SelectUnit(SELECT_TARGET_TOPAGGRO, 1))
      {
         if(target && target->isAlive())
        DoCast(target, SPELL_TWIN_SPIKE_LIGHT);
      }
         TwinsSpikeLightTimer = 12000;
    } else TwinsSpikeLightTimer -= diff;
    
    if (LightSourgeTimer <= diff)
    {
      if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
          DoCast(pTarget, SPELL_LIGHT_SOURGE);
          LightSourgeTimer = 20000;
    }else LightSourgeTimer -= diff;

    if (ShieldofLightTimer < diff)
    {
      DoScriptText(SAY_BLIZN, me);
      DoCast(me, SPELL_SHIELD_OF_LIGHT);
      DoCast(me, SPELL_TWINS_PACT_DARCK);
      ShieldofLightTimer = 100000;
    } else ShieldofLightTimer -= diff;

    if (LightVortexTimer < diff)
    {
      DoScriptText(SAY_POGLOT_LI,me);
      DoCastAOE(SPELL_LIGHT_VORTEX);
      LightVortexTimer = 100000;
    } else LightVortexTimer -= diff;

    DoMeleeAttackIfReady();
  }
};

struct boss_fjolaAI : public boss_twin_fjolaAI
{
    bool IAmVeklor() {return true;}
    boss_fjolaAI(Creature *c) : boss_twin_fjolaAI(c) {}
    
    ScriptedInstance* m_pInstance;
    
    uint32 TwinsSpikeDarkTimer;
    uint32 ShieldofDarkTimer;
    uint32 DarkSourgeTimer;
    uint32 PowerTwinsTimer;
    uint32 DarkVortexTimer;
    uint32 SummonBlakTimer;
    uint32 SummonDarkTimer;
    bool Portaldark;

  void Reset()
  {
      // уже определили TYPE_FJOLA, NOT_STARTED выше
      me->GetMotionMaster()->MovePoint(0, 559.647, 173.019, 394.572);
    TwinReset();
    TwinsSpikeDarkTimer = 11000;
    ShieldofDarkTimer = 130000;
    DarkSourgeTimer = 20000;
    DarkVortexTimer = 30000;
    SummonBlakTimer = 1000;
    SummonDarkTimer = 10000;
    Portaldark = true;
  }

    void UpdateAI(const uint32 diff)
    {
    if (Portaldark && SummonBlakTimer <= diff)
    {
       if (Creature* pTrash2 = me->SummonCreature(NPC_BLACK, Pos[RAND(1,3)],TEMPSUMMON_CORPSE_DESPAWN,90000))
        {
        pTrash2->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTrash2->SetReactState(REACT_PASSIVE);
         Portaldark = false;
        }
        SummonBlakTimer = 1000;
    } else SummonBlakTimer -= diff;
    
    if (!UpdateVictim())
            return;
        
    if (SummonDarkTimer <= diff)
     {
    Map *pMap = me->GetMap();
 
     if(!pMap)
     return;
 
       Map::PlayerList const &lPlayers = pMap->GetPlayers();
        
         if (lPlayers.isEmpty())
             return;
        
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
          {
            if (Player* pPlayer = itr->getSource())
              {
                float x = pPlayer->GetPositionX() + irand(-30,40);
            float y = pPlayer->GetPositionY() + irand(-30,40);
                        float z = pPlayer->GetPositionZ();
          if (Creature* pDark = me->SummonCreature(NPC_CONCENTRATED_DARCNESS,x,y,z,0,TEMPSUMMON_TIMED_DESPAWN,30000))
          {
              pDark->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
              pDark->SetReactState(REACT_PASSIVE);
              pDark->SetSpeed(MOVE_WALK, 0.8f, true);
              pDark->SetSpeed(MOVE_RUN, 0.8f, true);
              pDark->SetSpeed(MOVE_FLIGHT, 0.8f, true);
              pDark->AI()->AttackStart(pPlayer);
          }
          }
 
        }
     SummonDarkTimer = 50000;
     } else SummonDarkTimer -= diff;

    if (TwinsSpikeDarkTimer < diff)
    {
     if (Unit *target = SelectUnit(SELECT_TARGET_TOPAGGRO,0))
     {
      if(target && target->isAlive())
      DoCast(target, SPELL_TWINS_SPIKE_DARCK);
     }
      TwinsSpikeDarkTimer = 18000;
    } else TwinsSpikeDarkTimer -= diff;

    if (ShieldofDarkTimer < diff)
    {
      DoScriptText(SAY_BLIZN, me);
      DoCast(me, SPELL_SHIELDN_OF_DARKNESS);
      DoCast(me, SPELL_TWINS_PACT_DARCK);
      ShieldofDarkTimer = 130000;
    } else ShieldofDarkTimer -= diff;

    if (DarkSourgeTimer < diff)
    {
     if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
         DoCast(pTarget, SPELL_DARK_SOURGE);
         DarkSourgeTimer = 25000;
    } else DarkSourgeTimer -= diff;
    
    if (DarkVortexTimer <= diff)
    {
    DoScriptText(SAY_POGLOT_NI, me);
    DoCastAOE(SPELL_DARCK_VORTEX);
    DarkVortexTimer = 60000;
    } else DarkVortexTimer -= diff;

    DoMeleeAttackIfReady();
    }
};

struct mob_power_of_twinAI : public ScriptedAI
{
    mob_power_of_twinAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
 
    ScriptedInstance* m_pInstance;
    
    uint8 m_uiMovePoint;
    uint32 MoveTimer;
    uint32 PowerTimer;
    
void Rest()
  {
  DoNextMovement();
  m_uiMovePoint = 0;
  MoveTimer = 5000;
  PowerTimer = 100;
  }
  
  void AttackStart(Unit *pWho)
  {
    if (me->Attack(pWho, true))
      {
        me->AddThreat(pWho, 1000.0f);
    me->SetInCombatWith(pWho);
    pWho->SetInCombatWith(me);
      }
  }
  
 void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE)
            return;
 
        if(uiPointId == m_uiMovePoint)
            DoNextMovement();
    }
    
 void DoNextMovement()
    {
        m_uiMovePoint++;
        uint32 x = urand(MIN_X, MAX_X);
        uint32 y = urand(MIN_Y, MAX_Y);
        uint32 z = POSIT_Z;
        me->GetMotionMaster()->MoveConfused();
    }
    
 void UpdateAI(const uint32 diff)
    {
    
      if (MoveTimer <= diff)
        {
         DoNextMovement();
     MoveTimer = 5000;
    } else MoveTimer -= diff;

    if (PowerTimer <= diff)
    {
    Map *pMap = me->GetMap();
 
     if(!pMap)
     return;
 
       Map::PlayerList const &lPlayers = pMap->GetPlayers();
        
         if (lPlayers.isEmpty())
             return;
        
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
          {
            if (Player* pPlayer = itr->getSource())
            {
            if (me->IsWithinDistInMap(pPlayer, 1.0f))
            {
                pPlayer->CastSpell(me, SPELL_POWER_OF_THE_TWINS, true);
            me->DisappearAndDie();
            }
            }
          }
        
        if (Creature* pFjola = GetClosestCreatureWithEntry(me, BOSS_FJOLA, 1.0f))
            {
             pFjola->CastSpell(me, SPELL_POWER_OF_THE_TWINS, true);
            me->DisappearAndDie();
            }
            
        if (Creature* pEydis = GetClosestCreatureWithEntry(me, BOSS_EYDIS, 1.0f))
            {
            pEydis->CastSpell(me, SPELL_POWER_OF_THE_TWINS, true);
            me->DisappearAndDie();
            }
          
    PowerTimer = 100;
    } else PowerTimer -= diff;
   }
   
};

bool GossipHello_mob_essence_of_twin(Player* pPlayer, Creature* pCreature)
{
   
   switch(pCreature->GetEntry())
   {
        case 34568:
        if (pPlayer || pPlayer->HasAura(SPELL_DARK_ESSENCE))
          pPlayer->RemoveAura(SPELL_DARK_ESSENCE);
          pPlayer->CastSpell(pPlayer,SPELL_LIGHT_ESSENCE,true);
          pPlayer->CLOSE_GOSSIP_MENU();
          break;

        case 34567:
        if (pPlayer || pPlayer->HasAura(SPELL_LIGHT_ESSENCE))
          pPlayer->RemoveAura(SPELL_LIGHT_ESSENCE);
          pPlayer->CastSpell(pPlayer,SPELL_DARK_ESSENCE,true);
          pPlayer->CLOSE_GOSSIP_MENU();
          break;
    }
   return true;
}

bool GossipSelect_mob_essence_of_twin(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
        pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

CreatureAI* GetAI_mob_power_of_twin(Creature* pCreature)
{
    return new mob_power_of_twinAI (pCreature);
}

CreatureAI* GetAI_boss_eydis(Creature* pCreature)
{
    return new boss_eydisAI (pCreature);
}

CreatureAI* GetAI_boss_fjola(Creature* pCreature)
{
    return new boss_fjolaAI (pCreature);
}

void AddSC_boss_twin_fjola()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_fjola";
    newscript->GetAI = &GetAI_boss_fjola;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_eydis";
    newscript->GetAI = &GetAI_boss_eydis;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "mob_power_of_twin";
    newscript->GetAI = &GetAI_mob_power_of_twin;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "mob_essence_of_twin";
    newscript->pGossipHello = &GossipHello_mob_essence_of_twin;
    newscript->pGossipSelect = &GossipSelect_mob_essence_of_twin;
    newscript->RegisterSelf();
}
