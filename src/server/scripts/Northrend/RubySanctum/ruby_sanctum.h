/* Copyright (C) 2010 /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef DEF_RUBY_SANCTUM_H
#define DEF_RUBY_SANCTUM_H

enum Data
{
	DATA_BALTHARUS_EVENT = 1,
	DATA_ZARITHRIAN_EVENT = 2,
	DATA_RAGEFIRE_EVENT = 3,
	DATA_HALION_EVENT = 4,
	DATA_XERESTRASZA_EVENT = 5
};

enum Data64
{
	DATA_BALTHARUS = 1,
	DATA_ZARITHRIAN = 2,
	DATA_RAGEFIRE = 3,
	DATA_HALION = 4,
	DATA_XERESTRASZA = 5
};

enum Npcs
{
	NPC_BALTHARUS = 39751,
	NPC_ZARITHRIAN = 39746,
	NPC_RAGEFIRE = 39747,
	NPC_HALION = 39863,
	NPC_XERESTRASZA = 40429,
	NPC_ZARITHRIAN_SPAWN_STALKER = 39794,
	NPC_TWILIGHT_HALION = 40142
};

#endif