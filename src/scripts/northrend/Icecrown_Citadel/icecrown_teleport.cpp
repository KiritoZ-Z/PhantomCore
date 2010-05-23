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

/*
Complete: 60%
Created by Lordronn
*/


#include "ScriptedPch.h"
#include "icecrown_citadel.h"

#define LIGHTS_HAMMER 70781
#define ORATORY       70856
#define RAMPART       70857
#define DEATHBRINGER  70858
#define PLAGUEWORKS   9995
#define CRIMSONHALL   9996
#define FWHALLS     9997
#define SINDRAGOSA  70861
#define LICHKING    70860

/* Dont know if this is right, but we're going
Lights Hammer auto-unlocked
Kill Marrowgar opens Lady Deathwhisper
Kill LD unlocks Rampart of Skulls and Saurfang(since the Gunship Battle doesnt work we'll skip it)
Killing Saurfang opens Crimson Halls and Plagueworks
Killing Putricide & Blood-Queen Lana'thel opens Frostwing Halls
Killing Sindragosa opens the Lich King's Lair
*/

bool GoHello_icecrown_teleporter( Player *pPlayer, GameObject *pGO )
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pGO->GetInstanceData();
    if(!pInstance) return true;


    pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to Light's Hammer", GOSSIP_SENDER_MAIN, LIGHTS_HAMMER);
    if(pInstance->GetData(DATA_MARROWGAR_EVENT) == DONE || pPlayer->isGameMaster())
	{
        pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to the Oratory of the Damned", GOSSIP_SENDER_MAIN, ORATORY);
        if(pInstance->GetData(DATA_MARROWGAR_EVENT) == DONE || pPlayer->isGameMaster())
		{
            pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to the Rampart of Skulls", GOSSIP_SENDER_MAIN, RAMPART);
            pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to Deathbringer's Rise", GOSSIP_SENDER_MAIN, DEATHBRINGER);
            if(pInstance->GetData(DATA_SAURFANG_EVENT) == DONE || pPlayer->isGameMaster())
			{
                  pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to the Plagueworks", GOSSIP_SENDER_MAIN, PLAGUEWORKS);
                  pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to the Crimson Halls", GOSSIP_SENDER_MAIN, CRIMSONHALL);
                  if(pInstance->GetData(DATA_PROFESSOR_PUTRICIDE_EVENT) == DONE &&
					 pInstance->GetData(DATA_BLOOD_QUEEN_LANATHEL_EVENT) == DONE || pPlayer->isGameMaster())
				  {
                         pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to the Frostwing Halls", GOSSIP_SENDER_MAIN, FWHALLS);
                         pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to Sindragosa Lair", GOSSIP_SENDER_MAIN, SINDRAGOSA);
                         if(pInstance->GetData(DATA_SINDRAGOSA_EVENT) == DONE || pPlayer->isGameMaster())
						 { 
                            pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to the Frozen Throne", GOSSIP_SENDER_MAIN, LICHKING);
						    
						 }
				  }
			}
		}
	}
	
    pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pGO->GetGUID());

    return true;
};
						 

bool GOSelect_icecrown_teleporter( Player *pPlayer, GameObject *pGO, uint32 sender, uint32 action )
{
    if(sender != GOSSIP_SENDER_MAIN) return true;
    if(!pPlayer->getAttackers().empty()) return true;


    switch(action)
    {
    case LIGHTS_HAMMER:
        pPlayer->TeleportTo(631, -17.071068, 2211.468750, 30.054554, 3.12149);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case ORATORY:
        pPlayer->TeleportTo(631, -503.593414, 2211.468750, 62.762070, 3.139313);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case RAMPART:
        pPlayer->TeleportTo(631, -615.146118, 2211.471924, 199.908508, 6.26832);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case DEATHBRINGER:
        pPlayer->TeleportTo(631, -549.073486, 2211.289307, 539.223450, 6.275452);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case PLAGUEWORKS:
        pPlayer->TeleportTo(631, 4357.033691, 2864.192627, 349.337006, 1.559445);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case CRIMSONHALL:
        pPlayer->TeleportTo(631, 4452.79785, 2769.291504, 349.350342, 0.023817);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case FWHALLS:
        pPlayer->TeleportTo(631, 4357.003906, 2674.260010, 349.342346, 4.736377);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case SINDRAGOSA:
        pPlayer->TeleportTo(631, 4356.581543, 2565.748291, 220.401520, 4.886216);
        pPlayer->CLOSE_GOSSIP_MENU(); 
		break;

    case LICHKING:
        pPlayer->TeleportTo(631, 529.3969, -2124.879883, 1040.959961, 0.120937);
        pPlayer->CLOSE_GOSSIP_MENU(); 
		break;
    }

    return true;
}


void AddSC_icecrown_teleporter()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "icecrown_teleporter";
    newscript->pGOHello = &GoHello_icecrown_teleporter;
    newscript->pGOSelect = &GOSelect_icecrown_teleporter;
    newscript->RegisterSelf();
}