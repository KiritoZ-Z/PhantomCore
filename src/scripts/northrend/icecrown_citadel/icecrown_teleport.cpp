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
Complete: 70%
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
#define FWHALLS       9997
#define QUEEN         70861
#define LICHKING      70860

bool GoHello_icecrown_teleporter( Player *pPlayer, GameObject *pGO )
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pGO->GetInstanceData();
    if(!pInstance) return true;


    pPlayer->ADD_GOSSIP_ITEM(0, "Молот света", GOSSIP_SENDER_MAIN, LIGHTS_HAMMER);
    if(pInstance->GetData(DATA_MARROWGAR_EVENT) == DONE || pPlayer->isGameMaster())
	{
        pPlayer->ADD_GOSSIP_ITEM(0, "Молельня проклятых", GOSSIP_SENDER_MAIN, ORATORY);
        if(pInstance->GetData(DATA_DEATHWHISPER_EVENT) == DONE || pPlayer->isGameMaster())
		{
            pPlayer->ADD_GOSSIP_ITEM(0, "Черепной вал", GOSSIP_SENDER_MAIN, RAMPART);
            pPlayer->ADD_GOSSIP_ITEM(0, "Подъем Смертоносного", GOSSIP_SENDER_MAIN, DEATHBRINGER);
            if(pInstance->GetData(DATA_SAURFANG_EVENT) == DONE || pPlayer->isGameMaster())
			{
                  pPlayer->ADD_GOSSIP_ITEM(0, "Чумодельня", GOSSIP_SENDER_MAIN, PLAGUEWORKS);
                  pPlayer->ADD_GOSSIP_ITEM(0, "Святилище крови", GOSSIP_SENDER_MAIN, CRIMSONHALL);
                  pPlayer->ADD_GOSSIP_ITEM(0, "Залы Ледокрылых", GOSSIP_SENDER_MAIN, FWHALLS);
                  if(pInstance->GetData(DATA_VALITHRIA_DREAMWALKER_EVENT) == DONE || pPlayer->isGameMaster())
				  {
                         pPlayer->ADD_GOSSIP_ITEM(0, "Логово Королевы льда", GOSSIP_SENDER_MAIN, QUEEN);
                         if(pInstance->GetData(DATA_SINDRAGOSA_EVENT) == DONE || pPlayer->isGameMaster())
						 { 
                            pPlayer->ADD_GOSSIP_ITEM(0, "Ледяной трон", GOSSIP_SENDER_MAIN, LICHKING);
						    
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
        pPlayer->TeleportTo(631, -17.856115, 2211.640137, 30.115812, 3.12149);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case ORATORY:
        pPlayer->TeleportTo(631, -503.632599, 2211.219971, 62.823246, 3.139313);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case RAMPART:
        pPlayer->TeleportTo(631, -615.098267, 2211.509766, 199.973083, 6.26832);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case DEATHBRINGER:
        pPlayer->TeleportTo(631, -549.151001, 2211.463967, 539.290222, 6.275452);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case PLAGUEWORKS:
        pPlayer->TeleportTo(631, 4356.780273, 2863.636230, 349.337982, 1.559445);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case CRIMSONHALL:
        pPlayer->TeleportTo(631, 4453.248535, 2769.325684, 349.347473, 0.023817);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case FWHALLS:
        pPlayer->TeleportTo(631, 4356.853516, 2674.143311, 349.340118, 4.736377);
        pPlayer->CLOSE_GOSSIP_MENU();
		break;

    case QUEEN:
        pPlayer->TeleportTo(631, 4356.527344, 2402.710205, 220.462723, 4.886216);
        pPlayer->CLOSE_GOSSIP_MENU(); 
		break;

    case LICHKING:
        pPlayer->TeleportTo(631, 529.3969, -2124.879883, 1041.86f, 0.120937);
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