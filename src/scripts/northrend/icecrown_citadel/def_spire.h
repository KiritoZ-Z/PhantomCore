#ifndef DEF_ICECROWN_SPIRE_H
#define DEF_ICECROWN_SPIRE_H

enum
{
    DIFFICULTY = 1001,
    MAP_NUM = 631,
    MAX_ENCOUNTERS = 16,

    TYPE_TELEPORT = 0,
    TYPE_MARROWGAR = 1,
    TYPE_DEATHWHISPER = 2,
    TYPE_SKULLS_PLATO = 3,
    TYPE_FLIGHT_WAR = 4,
    TYPE_SAURFANG = 5,
    TYPE_FESTERGUT = 6,
    TYPE_ROTFACE = 7,
    TYPE_PUTRICIDE = 8,
    TYPE_BLOOD_COUNCIL = 9,
    TYPE_LANATHEL = 10,
    TYPE_VALITHRIA = 11,
    TYPE_SINDRAGOSA = 12,
    TYPE_LICH_KING = 13,
    TYPE_FROSTWIRM_COUNT = 15,

    NPC_LORD_MARROWGAR = 36612,
    NPC_LADY_DEATHWHISPER = 36855,
    NPC_DEATHBRINGER_SAURFANG = 37813,
    NPC_FESTERGUT = 36626,
    NPC_ROTFACE = 36627,
    NPC_PROFESSOR_PUTRICIDE = 36678,
    NPC_TALDARAM = 37973,
    NPC_VALANAR = 37970,
    NPC_KELESETH = 37972,
    NPC_LANATHEL = 37955,
    NPC_VALITHRIA = 36789,
    NPC_SINDRAGOSA = 37755,
    NPC_LICH_KING = 29983,

    GO_TELEPORT_GOSSIP_MESSAGE = 99323,
    TELEPORT_GOSSIP_MESSAGE = 99322,

    GO_ICEWALL_1 = 201911,
    GO_ICEWALL_2 = 201910,

    GO_ORATORY_DOOR = 201563,
    GO_DEATHWHISPER_ELEVATOR = 202220, //5653

    GO_SAURFANG_DOOR = 201825,

    GO_ORANGE_PLAGUE = 201371, //72536
    GO_GREEN_PLAGUE = 201370, //72537

    GO_SCIENTIST_DOOR_GREEN = 201614, //72530
    GO_SCIENTIST_DOOR_ORANGE = 201613, //72531
    GO_SCIENTIST_DOOR = 201372, //72541

    GO_CRIMSON_HALL_DOOR = 201376, //72532
    GO_COUNCIL_DOOR_1 = 201377, //72533
    GO_COUNCIL_DOOR_2 = 201378, //72534

    GO_GREEN_DRAGON_DOOR_1 = 201375, //1202
    GO_GREEN_DRAGON_DOOR_2 = 201374, //1200
    GO_VALITHRIA_DOOR_1 = 201380, //1618
    GO_VALITHRIA_DOOR_2 = 201382, //1482
    GO_VALITHRIA_DOOR_3 = 201383, //1335
    GO_VALITHRIA_DOOR_4 = 201381, //1558

    GO_SINDRAGOSA_DOOR_1 = 201369, //1619

    GO_FROZENTRONE_TR = 202223, //72061

    GO_SAURFANG_CACHE_10 = 202239,
    GO_SAURFANG_CACHE_25 = 202240,
    GO_SAURFANG_CACHE_10_H = 202238,
    GO_SAURFANG_CACHE_25_H = 202241,

    GO_GUNSHIP_ARMORY_A_10 = 201872, //
    GO_GUNSHIP_ARMORY_A_25 = 201873, //
    GO_GUNSHIP_ARMORY_A_10H = 201874, //
    GO_GUNSHIP_ARMORY_A_25H = 201875, //

    GO_GUNSHIP_ARMORY_H_10 = 202177, //
    GO_GUNSHIP_ARMORY_H_25 = 202178, //
    GO_GUNSHIP_ARMORY_H_10H = 202179, //
    GO_GUNSHIP_ARMORY_H_25H = 202180, //
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

enum SpellTableParameters
{
  DIFFICULTY_LEVELS = 4,
  CAST_ON_SELF = 1000,
  CAST_ON_SUMMONS,
  CAST_ON_VICTIM,
  CAST_ON_RANDOM,
  CAST_ON_BOTTOMAGGRO,

};

struct SpellTable
{
    uint32 id;
    uint32 m_uiSpellEntry[DIFFICULTY_LEVELS]; // Stores spell entry for difficulty levels
    uint32 m_uiSpellTimerMin[DIFFICULTY_LEVELS]; // The timer (min) before the next spell casting, in milliseconds
    uint32 m_uiSpellTimerMax[DIFFICULTY_LEVELS]; // The timer (max) before the next spell casting
    uint32 m_uiStageMask; // The mask of stages where spell is casted
    uint32 m_CastTarget; // Target on casting spell
    bool m_IsVisualEffect; // Spellcasting is visual effect or real effect
    bool m_IsBugged; // Need override for this spell
};

#endif

