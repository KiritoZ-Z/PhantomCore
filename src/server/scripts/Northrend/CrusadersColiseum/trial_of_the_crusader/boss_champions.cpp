#include "ScriptPCH.h"
#include "def.h"

enum
{
    //yells

    //dark knight
    SPELL_FROST_STRIKE          = 67937,
    SPELL_CHAINS_OF_ICE         = 66020,
    SPELL_DEATH_COIL            = 67930,
    SPELL_ICEBOUND_FORTITUDE    = 66023,
    SPELL_STRANGULATE           = 66018,
    SPELL_DEATH_GRIP            = 68755,
    //all boses
    SPELL_PVP_TRINKET           = 65547,

    //warrior
    SPELL_MORTAL_STRIKE            = 68783,
    SPELL_MORTAL_STRIKE_H        = 68784,
    SPELL_BLADESTORM            = 63784,
    SPELL_INTERCEPT                = 67540,
    SPELL_ROLLING_THROW            = 47115, //need core support for spell 67546, using 47115 instead
    //mage
    SPELL_FIREBALL                = 66042,
    SPELL_FIREBALL_H            = 68310,
    SPELL_BLAST_WAVE            = 66044,
    SPELL_BLAST_WAVE_H            = 68312,
    SPELL_HASTE                    = 66045,
    SPELL_POLYMORPH                = 66043,
    SPELL_POLYMORPH_H            = 68311,
    //shaman
    SPELL_CHAIN_LIGHTNING        = 67529,
    SPELL_CHAIN_LIGHTNING_H        = 68319,
    SPELL_EARTH_SHIELD            = 67530,
    SPELL_HEALING_WAVE            = 67528,
    SPELL_HEALING_WAVE_H        = 68318,
    SPELL_HEX_OF_MENDING        = 67534,
    //hunter
    SPELL_DISENGAGE                = 68340,
    SPELL_LIGHTNING_ARROWS        = 66083,
    SPELL_MULTI_SHOT            = 66081,
    SPELL_SHOOT                    = 66079,
    //rogue
    SPELL_EVISCERATE            = 67709,
    SPELL_EVISCERATE_H            = 68317,
    SPELL_FAN_OF_KNIVES            = 67706,
    SPELL_POISON_BOTTLE            = 67701,

    //paladin retro 
    SPELL_AVENGING_WRATH        = 66011,
    SPELL_CRUSUADER_STRIKE      = 66003,
    SPELL_DIVINE_SHIELD         = 66010,
    SPELL_DIVINE_STORM          = 66006,
    SPELL_HUMMER_OF_JUSTICE     = 66007,
    SPELL_HAND_OF_PROTECTION    = 66009,
    SPELL_JUNDGEMENT_OF_COMMAND  = 68019,
    SPELL_REPENTANCE             = 66008,
    SPELL_SEAL_OF_COMMAND        = 68021,

    //paladin holy
    SPELL_CLEANCE                = 68623,
    SPELL_FLASH_OF_LIGHT         = 68010,
    SPELL_HAND_OF_FREEDOM        = 68758,
    SPELL_HOLY_LIGHT             = 68012,
    SPELL_HOLY_SHOCK             = 68016,

    //druid balans
    SPELL_BARKSKIN              = 65860,
    SPELL_CYCLONE               = 65859,
    SPELL_ENTANGLING_ROOTS      = 65857,
    SPELL_FAERIE_FIRE           = 65863,
    SPELL_FORCE_OF_NATURE       = 65861,
    SPELL_INSECT_SWARM          = 67943,
    SPELL_MOONFIRE              = 67946,
    SPELL_STARFIRE              = 67949,
    SPELL_WRATH                 = 67952,

    //druid restor
    SPELL_LIFEBLOOM             = 67959,
    SPELL_NATURE_GRASP          = 66071,
    SPELL_NOURISH               = 67967,
    SPELL_REGROWTH              = 67970,
    SPELL_REJUVENATION          = 67973,
    SPELL_THORNS                = 66068,
    SPELL_TRANQUILITY           = 67976,

    //shaman restor
    SPELL_CLEANSE_SPIRIT       = 68629,
    SPELL_EARTH_SHIRLD         = 66063,
    SPELL_EARTH_SHOCK          = 68102,
    SPELL_HEROISM              = 65983,
    SPELL_HEX                  = 66054,
    SPELL_LESSER_HEALING_WAVE  = 68116,
    SPELL_RIPTIDE              = 68120,

    //priest disc
    SPELL_DISPELL_MAGIC        = 68626,
    SPELL_FLASH_HEAL           = 68024,
    SPELL_MANA_BURN            = 68028,
    SPELL_PENANCE              = 68030,
    SPELL_POWER_WORLD_SHIELD   = 68034,
    SPELL_RSYCHIC_SCREAM       = 65543,
    SPELL_RENEW                = 68037,
    
    //priest shadow
    SPELL_DISPERSION           = 65544,
    SPELL_MIND_BLAST           = 68040,
    SPELL_MIND_FLAY            = 68044,
    SPELL_PSYCHIC_HORROR       = 65545,
    SPELL_SHADOW_WORLD_PAIN    = 68089,
    SPELL_SILENCE              = 65542,
    SPELL_VAMPIRIC_TOUCH       = 68092,

    //warlock
    SPELL_CORRUPTION           = 68135,
    SPELL_CURSE_OF_AGONY       = 68138,
    SPELL_CURSE_OF_EXHAUSTION  = 65815,
    SPELL_DEATH_COIL_W           = 68139,
    SPELL_FEAR                 = 65809,
    SPELL_HELLFIRE             = 68147,
    SPELL_SEARING_PAIN         = 68150,
    SPELL_SHADOW_BOLT          = 68152,
    SPELL_UNSTABLE_AFFLICTION  = 68155

};

// Warrior
struct  npc_warriorAI : public ScriptedAI
{
    npc_warriorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Mortal_Strike_Timer;
    uint32 Bladestorm_Timer;
    uint32 Rolling_Throw_Timer;
    uint32 Intercept_Cooldown;
    uint32 intercept_check;

    void Reset()
    {
        me->SetRespawnDelay(999999999);
        Mortal_Strike_Timer = 6000;
        Bladestorm_Timer = 20000;
        Rolling_Throw_Timer = 30000;
        Intercept_Cooldown = 0;
        intercept_check = 1000;
    }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);
                    
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Mortal_Strike_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_MORTAL_STRIKE_H);
            Mortal_Strike_Timer = 6000;
        }else Mortal_Strike_Timer -= diff;  

        if (Rolling_Throw_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_ROLLING_THROW);
            Rolling_Throw_Timer = 30000;
        }else Rolling_Throw_Timer -= diff;

        if (Bladestorm_Timer < diff)
        {
            DoCast(me, SPELL_BLADESTORM);
            Bladestorm_Timer = 90000;
        }else Bladestorm_Timer -= diff;

        if (intercept_check < diff)
        {
            if (!me->IsWithinDistInMap(me->getVictim(), 8) && me->IsWithinDistInMap(me->getVictim(), 25) && Intercept_Cooldown < diff)
            {
                DoCast(me->getVictim(), SPELL_INTERCEPT);
                Intercept_Cooldown = 15000;
            }
            intercept_check = 1000;
        }
        else 
        {
            intercept_check -= diff;
            Intercept_Cooldown -= diff;
        }
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_warrior(Creature* pCreature)
{
    return new npc_warriorAI(pCreature);
}

// Mage
struct  npc_mageAI : public ScriptedAI
{
    npc_mageAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Fireball_Timer;
    uint32 Blast_Wave_Timer;
    uint32 Haste_Timer;
    uint32 Polymorph_Timer;

    void Reset()
    {
        me->SetRespawnDelay(999999999);
        Fireball_Timer = 0;
        Blast_Wave_Timer = 20000;
        Haste_Timer = 9000;
        Polymorph_Timer = 15000;
    }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);
 
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Fireball_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_FIREBALL_H);
            Fireball_Timer = 3000;
        }else Fireball_Timer -= diff;  

        if (Blast_Wave_Timer < diff)
        {
            DoCast(me, SPELL_BLAST_WAVE_H);
            Blast_Wave_Timer = 20000;
        }else Blast_Wave_Timer -= diff;

        if (Haste_Timer < diff)
        {
            DoCast(me, SPELL_HASTE);
            Haste_Timer = 10000;
        }else Haste_Timer -= diff;

        if (Polymorph_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1))
                DoCast(target, SPELL_POLYMORPH_H);
            Polymorph_Timer = 15000;
        }else Polymorph_Timer -= diff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_mage(Creature* pCreature)
{
    return new npc_mageAI(pCreature);
}

// Shaman ench
struct  npc_shaman_enchAI : public ScriptedAI
{
    npc_shaman_enchAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Chain_Lightning_Timer;
    uint32 Earth_Shield_Timer;
    uint32 Healing_Wave_Timer;
    uint32 Hex_Timer;

    float mob1_health;
    float mob2_health;
    float mob3_health;

    void Reset()
    {
        me->SetRespawnDelay(999999999);
        Chain_Lightning_Timer = 1000;
        Earth_Shield_Timer = 5000;
        Healing_Wave_Timer = 13000;
        Hex_Timer = 10000;
    }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
                me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);

    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Chain_Lightning_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_CHAIN_LIGHTNING_H);
            Chain_Lightning_Timer = 10000;
        }else Chain_Lightning_Timer -= diff;  

        if (Hex_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_HEX_OF_MENDING);
            Hex_Timer = 20000;
        }else Hex_Timer -= diff;

    if (Healing_Wave_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_HEALING_WAVE);
                    Healing_Wave_Timer = 8000;
                }
        }else Healing_Wave_Timer -= diff;

    if (Earth_Shield_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(!target->HasAura(SPELL_EARTH_SHIELD,me->GetGUID()) && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_EARTH_SHIELD);
                    Earth_Shield_Timer = 25000;
                }
        }else Earth_Shield_Timer -= diff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_shaman_ench(Creature* pCreature)
{
    return new npc_shaman_enchAI(pCreature);
}

// Hunter
struct  npc_hunterAI : public ScriptedAI
{
    npc_hunterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Shoot_Timer;
    uint32 Lightning_Arrows_Timer;
    uint32 Multi_Shot_Timer;
    uint32 Disengage_Cooldown;
    uint32 enemy_check;
    uint32 disengage_check;

    void Reset()
    {
        me->SetRespawnDelay(999999999);
        Shoot_Timer = 0;
        Lightning_Arrows_Timer = 13000;
        Multi_Shot_Timer = 10000;
        Disengage_Cooldown = 0;
        enemy_check = 1000;
        disengage_check;
    }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);           
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (enemy_check < diff)
        {
            if (!me->IsWithinDistInMap(me->getVictim(), 8) && me->IsWithinDistInMap(me->getVictim(), 30))
            {
                me->SetSpeed(MOVE_RUN, 0.0001);
            }
            else
            {
                me->SetSpeed(MOVE_RUN, 1);
            }
            enemy_check = 100;
        }else enemy_check -= diff;

        if (Disengage_Cooldown>0)
            Disengage_Cooldown -= diff;

        if (Shoot_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_SHOOT);
            Shoot_Timer = 3000;
        }else Shoot_Timer -= diff;  

        if (Multi_Shot_Timer < diff)
        {
            me->CastStop(SPELL_SHOOT);
            DoCast(me->getVictim(), SPELL_MULTI_SHOT);
            Multi_Shot_Timer = 10000;
        }else Multi_Shot_Timer -= diff;

        if (Lightning_Arrows_Timer < diff)
        {
            me->CastStop(SPELL_SHOOT);
            DoCast(me, SPELL_LIGHTNING_ARROWS);
            Lightning_Arrows_Timer = 25000;
        }else Lightning_Arrows_Timer -= diff;

        if (disengage_check < diff)
        {
            if (me->IsWithinDistInMap(me->getVictim(), 5) && Disengage_Cooldown == 0)
            {
                DoCast(me, SPELL_DISENGAGE);
                Disengage_Cooldown = 15000;
            }
            disengage_check = 1000;
        }else disengage_check -= diff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_hunter(Creature* pCreature)
{
    return new npc_hunterAI(pCreature);
}

// Rogue
struct  npc_rogueAI : public ScriptedAI
{
    npc_rogueAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Eviscerate_Timer;
    uint32 FoK_Timer;
    uint32 Poison_Timer;

    void Reset()
    {
        me->SetRespawnDelay(999999999);
        Eviscerate_Timer = 15000;
        FoK_Timer = 10000;
        Poison_Timer = 7000;
    }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);            
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Eviscerate_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_EVISCERATE_H);
            Eviscerate_Timer = 10000;
        }else Eviscerate_Timer -= diff;  

        if (FoK_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_FAN_OF_KNIVES);
            FoK_Timer = 7000;
        }else FoK_Timer -= diff;

        if (Poison_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(me, SPELL_POISON_BOTTLE);
            Poison_Timer = 6000;
        }else Poison_Timer -= diff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_rogue(Creature* pCreature)
{
    return new npc_rogueAI(pCreature);
}

//paladin retro
struct  npc_paladin_retroAI : public ScriptedAI
{
    npc_paladin_retroAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Aventure_Wrath_Timer;
    uint32 Crusuader_Strike_Timer;
    uint32 Divine_Shield_Timer;
    uint32 Devine_Storm_Timer;
    uint32 Hummer_of_Justice_Timer;
    uint32 Hand_of_Protection_Timer;
    uint32 Jundgement_of_Command_Timer;
    uint32 Repentance_Timer;
    /*uint32 Seal_of_Command_Timer;*/

    /*SPELL_AVENGING_WRATH        = 66011,
    SPELL_CRUSUADER_STRIKE      = 66003,
    SPELL_DIVINE_SHIELD         = 66010,
    SPELL_DIVINE_STORM          = 66006,
    SPELL_HUMMER_OF_JUSTICE     = 66007,
    SPELL_HAND_OF_PROTECTION    = 66009,
    SPELL_JUNDGEMENT_OF_COMMAND  = 68019,
    SPELL_REPENTANCE             = 66008,
    SPELL_SEAL_OF_COMMAND        = 68021,*/

     void Reset()
     {
        me->SetRespawnDelay(999999999);
         Aventure_Wrath_Timer = 180000;
         Crusuader_Strike_Timer = 6000;
         Divine_Shield_Timer = 300000;
         Devine_Storm_Timer = 10000;
         Hummer_of_Justice_Timer = 40000;
         Hand_of_Protection_Timer = 300000;
         Jundgement_of_Command_Timer = 8000;
         Repentance_Timer = 60000;
         /*Seal_of_Command_Timer = 0; */
     }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);           
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Crusuader_Strike_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_CRUSUADER_STRIKE);
            Crusuader_Strike_Timer = 6000;
        }else Crusuader_Strike_Timer -= diff;  

        if (Devine_Storm_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_DIVINE_STORM);
            Devine_Storm_Timer = 10000;
        }else Devine_Storm_Timer -= diff;

        if (Hummer_of_Justice_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_HUMMER_OF_JUSTICE);
            Hummer_of_Justice_Timer = 40000;
        }else Hummer_of_Justice_Timer -= diff;

        if (Jundgement_of_Command_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_JUNDGEMENT_OF_COMMAND);
            Jundgement_of_Command_Timer = 8000;
        }else Jundgement_of_Command_Timer -= diff;

        if(Repentance_Timer < diff)
          {
             DoCast(me->getVictim(), SPELL_REPENTANCE);
             Repentance_Timer = 60000;
          }else Repentance_Timer -= diff; 

        if(Divine_Shield_Timer < diff)
          {
              if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() <= 1500 && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_DIVINE_SHIELD);
                    Divine_Shield_Timer = 300000;
                }
          }else Divine_Shield_Timer -= diff;

        if(Hand_of_Protection_Timer < diff)
          {
              if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() <= 3000 && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_HAND_OF_PROTECTION);
                    Hand_of_Protection_Timer = 300000;
                }
          }else Hand_of_Protection_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_paladin_retro(Creature* pCreature)
{
    return new npc_paladin_retroAI(pCreature);
}


//death knight
struct  npc_death_knightAI : public ScriptedAI
{
    npc_death_knightAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Chain_of_Ice_Timer;
    uint32 Frost_Strike_Timer;
    uint32 Death_Coil_Timer;
    uint32 Icebond_Fortitude_Timer;
    uint32 Strangulate_Timer;
    uint32 Death_Grip_Timer;

    /*SPELL_FROST_STRIKE          = 67937,
    SPELL_CHAINS_OF_ICE         = 66020,
    SPELL_DEATH_COIL            = 67930,
    SPELL_ICEBOUND_FORTITUDE    = 66023,
    SPELL_STRANGULATE           = 66018,
    SPELL_DEATH_GRIP            = 68755,*/

     void Reset()
     {
        me->SetRespawnDelay(999999999);
        Chain_of_Ice_Timer = 8000;
        Frost_Strike_Timer = 6000;
        Death_Coil_Timer = 5000;
        Icebond_Fortitude_Timer = 60000;
        Strangulate_Timer = 120000;
        Death_Grip_Timer = 20000; 
     }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);            
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Chain_of_Ice_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_CHAINS_OF_ICE);
            Chain_of_Ice_Timer = 8000;
        }else Chain_of_Ice_Timer -= diff;  

        if (Frost_Strike_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_FROST_STRIKE);
            Frost_Strike_Timer = 6000;
        }else Frost_Strike_Timer -= diff;

        if (Death_Coil_Timer < diff)
        {
            DoCast(me, SPELL_DEATH_COIL);
            Death_Coil_Timer = 5000;
        }else Death_Coil_Timer -= diff;

        if (Death_Grip_Timer < diff)
        {
            DoCast(me, SPELL_DEATH_GRIP);
            Death_Grip_Timer = 20000;
        }else Death_Grip_Timer -= diff;

        /*if(Strangulate_Timer < diff)
          {
             if(target->CastSpell())
             {
                 DoCast(target, SPELL_STRANGULATE);
                 Strangulate_Timer = 180000;
             }
          }else Strangulate_Timer -= diff; */

        /*if(Icebond_Fortitude_Timer < diff)
          {
             //need cheak of target cast for self 
             DoCast(me, SPELL_ICEBOUND_FORTITUDE);
             Icebond_Fortitude_Timer = 60000;
          }else Icebond_Fortitude_Timer -= diff; */

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_death_knight(Creature* pCreature)
{
    return new npc_death_knightAI(pCreature);
}

//paladin holy
struct  npc_paladin_holyAI : public ScriptedAI
{
    npc_paladin_holyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Cleance_Timer;
    uint32 Flash_of_Light_Timer;
    uint32 Divine_Shield_Timer;
    uint32 Hand_of_Freedom_Timer;
    uint32 Hummer_of_Justice_Timer;
    uint32 Hand_of_Protection_Timer;
    uint32 Holy_Light_Timer;
    uint32 Holy_Shock_Timer;

    /*SPELL_CLEANCE                = 68623,
    SPELL_FLASH_OF_LIGHT         = 68010,
    SPELL_HAND_OF_FREEDOM        = 68758,
    SPELL_HOLY_LIGHT             = 68012,
    SPELL_HOLY_SHOCK             = 68016,*/

     void Reset()
     {
        me->SetRespawnDelay(999999999);
         Cleance_Timer = 0;
         Flash_of_Light_Timer = 0;
         Divine_Shield_Timer = 300000;
         Hand_of_Freedom_Timer = 15000;
         Hummer_of_Justice_Timer = 40000;
         Hand_of_Protection_Timer = 300000;
         Holy_Light_Timer = 0;
         Holy_Shock_Timer = 6000;
     }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);            
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Flash_of_Light_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me))
                {
                    DoCast(me->getVictim(), SPELL_FLASH_OF_LIGHT);
                    Flash_of_Light_Timer = 0;
                }
        }else Flash_of_Light_Timer -= diff;  

        if (Holy_Light_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me))
                {
                    DoCast(me->getVictim(), SPELL_HOLY_LIGHT);
                    Holy_Light_Timer = 0;
                }
        }else Holy_Light_Timer -= diff;

        if (Hummer_of_Justice_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_HUMMER_OF_JUSTICE);
            Hummer_of_Justice_Timer = 40000;
        }else Hummer_of_Justice_Timer -= diff;

        if (Holy_Shock_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_HOLY_SHOCK);
            Holy_Shock_Timer = 6000;
        }else Holy_Shock_Timer -= diff;

        if(Divine_Shield_Timer < diff)
          {
              if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() <= 1500 && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_DIVINE_SHIELD);
                    Divine_Shield_Timer = 300000;
                }
          }else Divine_Shield_Timer -= diff;

        if(Hand_of_Protection_Timer < diff)
          {
              if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() <= 3000 && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_HAND_OF_PROTECTION);
                    Hand_of_Protection_Timer = 300000;
                }
          }else Hand_of_Protection_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_paladin_holy(Creature* pCreature)
{
    return new npc_paladin_holyAI(pCreature);
}

//druid balans
struct  npc_druid_balansAI : public ScriptedAI
{
    npc_druid_balansAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Barkskin_Timer;
    uint32 Cyclone_Timer;
    uint32 Entangling_Roots_Timer;
    uint32 Faerie_Fire_Timer;
    uint32 Force_of_Nature_Timer;
    uint32 Insect_Swarm_Timer;
    uint32 Moonfire_Timer;
    uint32 Starfire_Timer;
    uint32 Wrath_Timer;

    /*SPELL_BARKSKIN              = 65860,
    SPELL_CYCLONE               = 65859,
    SPELL_ENTANGLING_ROOTS      = 65857,
    SPELL_FAERIE_FIRE           = 65863,
    SPELL_FORCE_OF_NATURE       = 65861,
    SPELL_INSECT_SWARM          = 67943,
    SPELL_MOONFIRE              = 67946,
    SPELL_STARFIRE              = 67949,
    SPELL_WRATH                 = 67952,*/

     void Reset()
     {
        me->SetRespawnDelay(999999999);
          Barkskin_Timer = 60000;
          Cyclone_Timer = 6000;
          Entangling_Roots_Timer = 10000;
          Faerie_Fire_Timer = 40000;
          Force_of_Nature_Timer = 180000;
          Insect_Swarm_Timer = 12000;
          Moonfire_Timer = 5000;
          Starfire_Timer = 2000;
          Wrath_Timer = 1500 ;
     }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);            
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Cyclone_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_CYCLONE);
            Cyclone_Timer = 6000;
        }else Cyclone_Timer -= diff;  

        if (Entangling_Roots_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_ENTANGLING_ROOTS);
            Entangling_Roots_Timer = 10000;
        }else Entangling_Roots_Timer -= diff;

        if (Faerie_Fire_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_FAERIE_FIRE);
            Faerie_Fire_Timer = 40000;
        }else Faerie_Fire_Timer -= diff;

        if (Force_of_Nature_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_FORCE_OF_NATURE);
            Force_of_Nature_Timer = 180000;
        }else Force_of_Nature_Timer -= diff;

        if(Insect_Swarm_Timer < diff)
          {
              DoCast(me, SPELL_INSECT_SWARM);
              Insect_Swarm_Timer = 12000;
          }else Insect_Swarm_Timer -= diff;

        if(Starfire_Timer < diff)
          {
              DoCast(me, SPELL_STARFIRE);
              Starfire_Timer = 2000;
          }else Starfire_Timer -= diff;

        if(Wrath_Timer < diff)
          {
              DoCast(me, SPELL_WRATH);
              Wrath_Timer = 1500;
          }else Wrath_Timer -= diff;

        if(Moonfire_Timer < diff)
          {
              if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() <= 2000)
                {
                    DoCast(target, SPELL_MOONFIRE);
                    Moonfire_Timer = 5000;
                }
          }else Moonfire_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_druid_balans(Creature* pCreature)
{
    return new npc_druid_balansAI(pCreature);
}

//druid restor
struct  npc_druid_restorAI : public ScriptedAI
{
    npc_druid_restorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Barkskin_Timer;
    uint32 Lifebloom_Timer;
    uint32 Nature_Grasp_Timer;
    uint32 Nourish_Timer;
    uint32 Regrowth_Timer;
    uint32 Rejuvenation_Timer;
    uint32 Thorns_Timer;
    uint32 Tranquility_Timer;

    /*SPELL_LIFEBLOOM             = 67959,
    SPELL_NATURE_GRASP          = 66071,
    SPELL_NOURISH               = 67967,
    SPELL_REGROWTH              = 67970,
    SPELL_REJUVENATION          = 67973,
    SPELL_THORNS                = 66068,
    SPELL_TRANQUILITY           = 67976,*/

     void Reset()
     {
          me->SetRespawnDelay(999999999);
          Barkskin_Timer = 60000;
          Lifebloom_Timer = 0;
          Nature_Grasp_Timer = 60000;
          Nourish_Timer = 1500;
          Regrowth_Timer = 2000;
          Rejuvenation_Timer = 0;
          Thorns_Timer = 0;
          Tranquility_Timer = 600000;
     }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);           
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Nourish_Timer < diff)
        {
           if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me) )
                {
                    DoCast(target, SPELL_NOURISH);
                    Nourish_Timer = 1500;
                }
        }else Nourish_Timer -= diff;  

        if (Regrowth_Timer < diff)
        {
           if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me) )
                {
                    DoCast(target, SPELL_REGROWTH);
                    Regrowth_Timer = 2000;
                }
        }else Regrowth_Timer -= diff;  

        if (Tranquility_Timer < diff)
        {
           if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == 2500 && target->IsFriendlyTo(me) )
                {
                    DoCast(target, SPELL_TRANQUILITY);
                    Tranquility_Timer = 2000;
                }
        }else Tranquility_Timer -= diff;  

        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_druid_restor(Creature* pCreature)
{
    return new npc_druid_restorAI(pCreature);
}

//shaman restor
struct  npc_shaman_restorAI : public ScriptedAI
{
    npc_shaman_restorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Cleans_Spirit_Timer;
    uint32 Earth_Shield_Timer;
    uint32 Earth_Shock_Timer;
    uint32 Heroism_Timer;
    uint32 Hex_Timer;
    uint32 Leser_Healing_Wave_Timer;
    uint32 Riptide_Timer;


    /*SPELL_CLEANSE_SPIRIT       = 68629,
    SPELL_EARTH_SHIRLD         = 66063,
    SPELL_EARTH_SHOCK          = 68102,
    SPELL_HEROISM              = 65983,
    SPELL_HEX                  = 66054,
    SPELL_LESSER_HEALING_WAVE  = 68116,
    SPELL_RIPTIDE              = 68120,*/

     void Reset()
     {
          me->SetRespawnDelay(999999999);
          Cleans_Spirit_Timer = 0;
          Earth_Shield_Timer = 0;
          Earth_Shock_Timer = 6000;
          Heroism_Timer = 300000;
          Hex_Timer = 42000;
          Leser_Healing_Wave_Timer = 1500;
          Riptide_Timer = 6000;
     }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);            
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Earth_Shock_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_EARTH_SHOCK);
            Earth_Shock_Timer = 6000;

        }else Earth_Shock_Timer -= diff;  

        if (Hex_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_HEX_OF_MENDING);
            Hex_Timer = 20000;
        }else Hex_Timer -= diff;

        if (Leser_Healing_Wave_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_LESSER_HEALING_WAVE);
                    Leser_Healing_Wave_Timer = 3000;
                }
        }else Leser_Healing_Wave_Timer -= diff;

        if (Earth_Shield_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(!target->HasAura(SPELL_EARTH_SHIELD,me->GetGUID()) && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_EARTH_SHIELD);
                    Earth_Shield_Timer = 25000;
                }
        }else Earth_Shield_Timer -= diff;
      
        if (Riptide_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_RIPTIDE);
                    Riptide_Timer = 6000;
                }
        }else Riptide_Timer -= diff;

        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_shaman_restor(Creature* pCreature)
{
    return new npc_shaman_restorAI(pCreature);
}

//priest disc
struct  npc_priest_discAI : public ScriptedAI
{
    npc_priest_discAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Dispell_Magic_Timer;
    uint32 Flash_Hill_Timer;
    uint32 Mana_Burn_Timer;
    uint32 Penance_Timer;
    uint32 Power_World_Shield_Timer;
    uint32 Psychic_Scream_Timer;
    uint32 Renew_Timer;


    /*SPELL_DISPELL_MAGIC        = 68626,
    SPELL_FLASH_HEAL           = 68024,
    SPELL_MANA_BURN            = 68028,
    SPELL_PENANCE              = 68030,
    SPELL_POWER_WORLD_SHIELD   = 68034,
    SPELL_RSYCHIC_SCREAM       = 65543,
    SPELL_RENEW                = 68037,*/

     void Reset()
     {
          me->SetRespawnDelay(999999999);
          Dispell_Magic_Timer = 0;
          Flash_Hill_Timer = 2000;
          Mana_Burn_Timer = 10000;
          Penance_Timer = 2000;
          Power_World_Shield_Timer = 40000;
          Psychic_Scream_Timer = 30000;
          Renew_Timer = 4000;

     }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);           
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Flash_Hill_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_FLASH_HEAL);
                    Flash_Hill_Timer = 2000;
                }
        }else Flash_Hill_Timer -= diff;
      
        if (Penance_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_PENANCE);
                    Penance_Timer = 2000;
                }
        }else Penance_Timer -= diff;

        if (Psychic_Scream_Timer < diff)
        {
                if (me->IsWithinDistInMap(me->getVictim(), 2))
                {
                    DoCast(me->getVictim(), SPELL_RSYCHIC_SCREAM);
                    Psychic_Scream_Timer = 30000;
                }
        }else Psychic_Scream_Timer -= diff;

        if (Renew_Timer < diff)
        {
            if(Unit* target = me->SelectNearestTarget(40))
                if(target->GetHealth() == target->GetHealth() * 100 / target->GetMaxHealth() && target->IsFriendlyTo(me))
                {
                    DoCast(target, SPELL_PENANCE);
                    Renew_Timer = 4000;
                }
        }else Renew_Timer -= diff;

        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_priest_disc(Creature* pCreature)
{
    return new npc_priest_discAI(pCreature);
}

//priest shadow
struct  npc_priest_shadowAI : public ScriptedAI
{
    npc_priest_shadowAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Dispersion_Timer;
    uint32 Mind_Blast_Timer;
    uint32 Mind_Flay_Timer;
    uint32 Psychic_Horror_Timer;
    uint32 Shadow_World_Pain_Timer;
    uint32 Psychic_Scream_Timer;
    uint32 Vampiric_Touch_Timer;
    uint32 Silence_Timer;


    /*SPELL_DISPERSION           = 65544,
    SPELL_MIND_BLAST           = 68040,
    SPELL_MIND_FLAY            = 68044,
    SPELL_PSYCHIC_HORROR       = 65545,
    SPELL_SHADOW_WORLD_PAIN    = 68089,
    SPELL_SILENCE              = 65542,
    SPELL_VAMPIRIC_TOUCH       = 68092,*/

     void Reset()
     {
          me->SetRespawnDelay(999999999);
          Dispersion_Timer = 180000;
          Mind_Blast_Timer = 8000;
          Mind_Flay_Timer = 3000;
          Psychic_Horror_Timer = 120000;
          Shadow_World_Pain_Timer = 21000;
          Psychic_Scream_Timer = 30000;
          Vampiric_Touch_Timer = 16000;
          Silence_Timer = 45000;

     }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);           
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Mind_Blast_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_MIND_BLAST);
            Mind_Blast_Timer = 8000;
        }else Mind_Blast_Timer -= diff;

        if (Mind_Flay_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_MIND_FLAY);
            Mind_Flay_Timer = 3000;
        }else Mind_Flay_Timer -= diff;


        if (Psychic_Scream_Timer < diff)
        {
               if (me->IsWithinDistInMap(me->getVictim(), 2))
               {
                    DoCast(me->getVictim(), SPELL_RSYCHIC_SCREAM);
                    Psychic_Scream_Timer = 30000;
               }
        }else Psychic_Scream_Timer -= diff;

        if (Psychic_Horror_Timer < diff)
        {
               if (me->IsWithinDistInMap(me->getVictim(), 2))
               {
                    DoCast(me->getVictim(), SPELL_PSYCHIC_HORROR);
                    Psychic_Horror_Timer = 120000;
               }
        }else Psychic_Horror_Timer -= diff;

        if (Shadow_World_Pain_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_SHADOW_WORLD_PAIN);
            Shadow_World_Pain_Timer = 21000;
        }else Shadow_World_Pain_Timer -= diff;

        if (Vampiric_Touch_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_VAMPIRIC_TOUCH);
            Vampiric_Touch_Timer = 16000;
        }else Vampiric_Touch_Timer -= diff;


        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_priest_shadow(Creature* pCreature)
{
    return new npc_priest_shadowAI(pCreature);
}

//warlock
struct  npc_warlockAI : public ScriptedAI
{
    npc_warlockAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 Corruption_Timer;
    uint32 Curse_of_Agony_Timer;
    uint32 Death_Coil_Timer;
    uint32 Fear_Timer;
    uint32 Hellfire_Timer;
    uint32 Searing_Pain_Timer;
    uint32 Shadow_Bolt_Timer;
    uint32 Unstable_Affliction_Timer;


    /*SPELL_CORRUPTION           = 68135,
    SPELL_CURSE_OF_AGONY       = 68138,
    SPELL_CURSE_OF_EXHAUSTION  = 65815,
    SPELL_DEATH_COIL           = 68139,
    SPELL_FEAR                 = 65809,
    SPELL_HELLFIRE             = 68147,
    SPELL_SEARING_PAIN         = 68150,
    SPELL_SHADOW_BOLT          = 68152,
    SPELL_UNSTABLE_AFFLICTION  = 68155*/

     void Reset()
     {
          me->SetRespawnDelay(999999999);
          Corruption_Timer = 18000;
          Curse_of_Agony_Timer = 24000;
          Death_Coil_Timer = 120000;
          Fear_Timer = 8000;
          Hellfire_Timer = 15000;
          Searing_Pain_Timer = 2000;
          Shadow_Bolt_Timer = 3000;
          Unstable_Affliction_Timer = 30000;
     }

    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_CHAMPIONS) == DONE)
            me->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
        {
        if (!m_pInstance)
        return;

        if(!me->FindNearestCreature(34458, 200, true) && !me->FindNearestCreature(34451, 200, true) && !me->FindNearestCreature(34459, 200, true)
            && !me->FindNearestCreature(34448, 200, true) && !me->FindNearestCreature(34449, 200, true) && !me->FindNearestCreature(34445, 200, true)
            && !me->FindNearestCreature(34456, 200, true) && !me->FindNearestCreature(34447, 200, true) && !me->FindNearestCreature(34441, 200, true)
            && !me->FindNearestCreature(34454, 200, true) && !me->FindNearestCreature(34455, 200, true) && !me->FindNearestCreature(34444, 200, true)
            && !me->FindNearestCreature(34450, 200, true) && !me->FindNearestCreature(34461, 200, true) && !me->FindNearestCreature(34460, 200, true)
            && !me->FindNearestCreature(34469, 200, true) && !me->FindNearestCreature(34467, 200, true) && !me->FindNearestCreature(34468, 200, true)
            && !me->FindNearestCreature(34465, 200, true) && !me->FindNearestCreature(34471, 200, true) && !me->FindNearestCreature(34466, 200, true)
            && !me->FindNearestCreature(34473, 200, true) && !me->FindNearestCreature(34472, 200, true) && !me->FindNearestCreature(34463, 200, true)
            && !me->FindNearestCreature(34470, 200, true) && !me->FindNearestCreature(34474, 200, true) && !me->FindNearestCreature(34475, 200, true)
            && !me->FindNearestCreature(34453, 200, true))
                m_pInstance->SetData(TYPE_CHAMPIONS, DONE);            
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Corruption_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_CORRUPTION);
            Corruption_Timer = 18000;
        }else Corruption_Timer -= diff;

        if (Curse_of_Agony_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_CURSE_OF_AGONY);
            Curse_of_Agony_Timer = 24000;
        }else Curse_of_Agony_Timer -= diff;


        if (Death_Coil_Timer < diff)
        {
               if (me->IsWithinDistInMap(me->getVictim(), 2))
               {
                    DoCast(me->getVictim(), SPELL_DEATH_COIL_W);
                    Death_Coil_Timer = 120000;
               }
        }else Death_Coil_Timer -= diff;

        if (Fear_Timer < diff)
        {
               if (me->IsWithinDistInMap(me->getVictim(), 2))
               {
                    DoCast(me->getVictim(), SPELL_FEAR);
                    Fear_Timer = 8000;
               }
        }else Fear_Timer -= diff;

        if (Searing_Pain_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_SEARING_PAIN);
            Searing_Pain_Timer = 2000;
        }else Searing_Pain_Timer -= diff;

        if (Shadow_Bolt_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_SHADOW_BOLT);
            Shadow_Bolt_Timer = 3000;
        }else Shadow_Bolt_Timer -= diff;

        if (Unstable_Affliction_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_UNSTABLE_AFFLICTION);
            Unstable_Affliction_Timer = 30000;
        }else Unstable_Affliction_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_warlock(Creature* pCreature)
{
    return new npc_warlockAI(pCreature);
}

void AddSC_boss_champions()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_warrior";
    NewScript->GetAI = &GetAI_npc_warrior;
    NewScript->RegisterSelf();
 
    NewScript = new Script;
    NewScript->Name = "npc_mage";
    NewScript->GetAI = &GetAI_npc_mage;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_shaman_ench";
    NewScript->GetAI = &GetAI_npc_shaman_ench;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_hunter";
    NewScript->GetAI = &GetAI_npc_hunter;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_rogue";
    NewScript->GetAI = &GetAI_npc_rogue;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_paladin_retro";
    NewScript->GetAI = &GetAI_npc_paladin_retro;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_death_knight";
    NewScript->GetAI = &GetAI_npc_death_knight;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_paladin_holy";
    NewScript->GetAI = &GetAI_npc_paladin_holy;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_druid_balans";
    NewScript->GetAI = &GetAI_npc_druid_balans;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_druid_restor";
    NewScript->GetAI = &GetAI_npc_druid_restor;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_priest_disc";
    NewScript->GetAI = &GetAI_npc_priest_disc;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_priest_shadow";
    NewScript->GetAI = &GetAI_npc_priest_shadow;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_warlock";
    NewScript->GetAI = &GetAI_npc_warlock;
    NewScript->RegisterSelf();

}