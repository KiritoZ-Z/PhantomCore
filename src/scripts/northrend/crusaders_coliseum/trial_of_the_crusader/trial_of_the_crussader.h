/*
 * Copyright (C) 2008 - 2009 Trinity <http://www.trinitycore.org/>
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

enum
{
    MAX_ENCOUNTER               = 10,


    PHASE_1				        = 0, // Gormok the Impaler
	PHASE_2						= 1, // Acidmaw & Dreadscale
	PHASE_3						= 2, // Icehowl
	PHASE_4						= 3, // Jaraxxus
	PHASE_5						= 4, // Faction Champions
	PHASE_6						= 5, // Val'Kyr Twins
	PHASE_7						= 6, // Anub'Arak 
	DATA_UPDATE_STATE_UI_COUNT  = 7,
	PHASE_SPECHIAL				= 8,
	SPECHIAL_TIMER				= 9, 
	DATA_FJOLA					= 10,
	DATA_EYDIS					= 11,
	

	NPC_GORMOK_IMPALER			= 34796,
	NPC_ACIDMAW					= 35144,
	NPC_DREADSCALE				= 34799,
	NPC_ICEHOWL					= 34797,
	NPC_LORD_JARAXXUS			= 34780,
	NPC_FJOLA					= 34497,
	NPC_EYDIS					= 34496,
	NPC_ANUBARAK				= 34564,
	NPC_LICH_KING				= 35877,
	NPC_ANONSER					= 34816,
	NPC_TIRION					= 34996,
	NPC_VARIAN_WRYNN			= 34990,
	NPC_FIZZLEBANG				= 35458, 
	NPC_GARROSH					= 34995,

	GO_FLOOR					= 195527,
	GO_GATE						= 195647
};

enum uiGifts
{
	CHAMPIONS_CACHE_NORMAL_10	= 195631,
	CHAMPIONS_CACHE_NORMAL_25	= 195632,
	CHAMPIONS_CACHE_HEROIC_10	= 195633,
	CHAMPIONS_CACHE_HEROIC_25	= 195635,

	ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_10_1_24		= 195665,
	ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_10_25_44	= 195666,
	ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_10_45_49	= 195667,
	ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_10_50		= 195668,
	ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_25_1_24		= 195669,
	ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_25_25_44	= 195670,
	ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_25_45_49	= 195671,
	ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_25_50		= 195672,
};

enum uiWorldStates
{
	UPDATE_STATE_UI_SHOW            = 4390,
    UPDATE_STATE_UI_COUNT           = 4389,
};

enum IntroYells
{
	SAY_INTRO_START				=	-1600205,
	SAY_INTRO_BEGINN			=	-1600207, 
	SAY_INTRO_3					=	-1600206,  
	SAY_INTRO_GOMROK_START_1	=	-1600208, 
	SAY_INTRO_DREADSCALE		=	-1600209,
	SAY_INTRO_EISHEULER			=	-1600210,

	SAY_JARAXXUS_INTRO_1		=	-1600211,
	SAY_JARAXXUS_INTRO_2		=	-1600212,
	SAY_JARAXXUS_INTRO_3		=	-1600213, 
	SAY_JARAXXUS_INTRO_4		=	-1600214,
	SAY_JARAXXUS_INTRO_5		=	-1600215,
	SAY_JARAXXUS_INTRO_6		=	-1600216,
	SAY_JARAXXUS_INTRO_7		=	-1600217,

	SAY_INTRO_FACTION_ALLI_1    =	-1600229, 
	SAY_INTRO_FACTION_ALLI_2	=	-1600230, 
	SAY_INTRO_FACTION_ALLI_3	=	-1600231, 
	SAY_INTRO_FACTION_ALLI_4	=	-1600232, 
	SAY_ALLI_SPAWN				=	-1600233,

	SAY_INTRO_FACTION_HORDE_1	=	-1600234, 
	SAY_INTRO_FACTION_HORDE_2   =	-1600235, 
	SAY_INTRO_FACTION_HORDE_3	=	-1600236, 
	SAY_INTRO_FACTION_HORDE_4	=	-1600237, 
	SAY_HORDE_SPAWN				=	-1600238,

	SAY_INTRO_TWINS				=	-1600239, 

	SAY_INTRO_LICHTKING_1		=	-1600240,  
	SAY_INTRO_LICHTKING_2		=	-1600241, 
	SAY_INTRO_LICHTKING_3		=	-1600242, 
	SAY_INTRO_LICHTKING_4		=	-1600243, 
	SAY_INTRO_LICHTKING_5		=	-1600244, 
	SAY_INTRO_ANUBARAK			=	-1600245,  

	SAY_JARAXXUS_OUTRO_1		=	-1600246, 
	SAY_JARAXXUS_OUTRO_2		=	-1600247, 
	SAY_JARAXXUS_OUTRO_3		=	-1600248, 
	SAY_JARAXXUS_OUTRO_4		=	-1600249, 

	SAY_OUTRO_FACTION			=	-1600250,
	SAY_WIPE_BEASTS				=	-1600251,
};