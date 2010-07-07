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

#ifndef DEF_ICECROWN_CITADEL_H
#define DEF_ICECROWN_CITADEL_H

enum Data
{
    DATA_MARROWGAR_EVENT,
    DATA_DEATHWHISPER_EVENT,
    DATA_GUNSHIP_BATTLE_EVENT,
    DATA_SAURFANG_EVENT,
    DATA_FESTERGURT_EVENT,
    DATA_ROTFACE_EVENT,
    DATA_PROFESSOR_PUTRICIDE_EVENT, 
    DATA_BLOOD_PRINCE_COUNCIL_EVENT,
    DATA_BLOOD_QUEEN_LANATHEL_EVENT,
    DATA_VALITHRIA_DREAMWALKER_EVENT,
    DATA_SINDRAGOSA_EVENT,
    DATA_LICH_KING_EVENT,
};

enum Data64
{
    DATA_MARROWGAR,
    DATA_DEATHWHISPER,
    DATA_SAURFANG,
    DATA_GUNSHIP_BATTLE,
    DATA_FESTERGURT,
    DATA_ROTFACE,
    DATA_PROFESSOR_PUTRICIDE,
    DATA_PRINCE_VALANAR_ICC,
    DATA_PRINCE_KELESETH_ICC,
    DATA_PRINCE_TALDARAM_ICC,
    DATA_BLOOD_QUEEN_LANATHEL,
    DATA_VALITHRIA_DREAMWALKER,
    DATA_SINDRAGOSA,
    DATA_LICH_KING,
};

enum mainCreatures
{
    CREATURE_MARROWGAR               = 36612,
    CREATURE_DEATHWHISPER            = 36855,
    CREATURE_GUNSHIP                 = 30343,
    CREATURE_SAURFANG                = 37813,
    CREATURE_FESTERGURT              = 36626,
    CREATURE_ROTFACE                 = 36627,
    CREATURE_PROFESSOR_PUTRICIDE     = 36678,
    CREATURE_PRINCE_VALANAR_ICC      = 37970,
    CREATURE_PRINCE_KELESETH_ICC     = 37972,
    CREATURE_PRINCE_TALDARAM_ICC     = 37973,
    CREATURE_BLOOD_QUEEN_LANATHEL    = 37955,
    CREATURE_VALITHRIA_DREAMWALKER   = 36789,
    CREATURE_SINDRAGOSA              = 36853,
    CREATURE_LICH_KING               = 36597,
};

enum GameObjects
{
    GO_PROFCOLLISIONDOOR            = 201612,
    GO_ORANGEPIPE                   = 201617,
    GO_GREENPIPE                    = 201618,
    GO_OOZEVALVE                    = 201615,
    GO_GASVALVE                     = 201616,
    GO_PROFDOORORANGE               = 201613,
    GO_PROFDOORGREEN                = 201614,
};

#endif
