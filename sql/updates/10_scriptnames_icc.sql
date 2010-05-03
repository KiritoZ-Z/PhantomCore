UPDATE `creature_template` SET `scriptname`='boss_the_lich_king' WHERE `entry`=36597;
UPDATE `creature_template` SET `scriptname`='boss_festergut' WHERE `entry`=36626;
UPDATE `creature_template` SET `scriptname`='boss_deathbringer_saurfang', `AIName` ='' WHERE `entry` = 37813;
UPDATE `instance_template` SET `script`='instance_icecrown_citadel' WHERE (`map`='631');
UPDATE `gameobject_template` SET `ScriptName` = 'icecrown_teleporter' WHERE `entry` IN (202235,202242,202223, 202244,202243,202245);
UPDATE `creature_template` SET `ScriptName`='boss_Deathwhisper' WHERE (`entry`='36855');
UPDATE `creature_template` SET `ScriptName`='boss_Marrowgar' WHERE (`entry`='36612');
UPDATE `creature_template` SET `ScriptName`='npc_NerubarBroodkeeper' WHERE (`entry`='36725');
UPDATE `creature_template` SET `ScriptName`='npc_TheDamned' WHERE (`entry`='37011');

UPDATE `creature_template` SET `ScriptName`='npc_ServantoftheThrone' WHERE (`entry`='36724');
UPDATE `creature_template` SET `ScriptName`='npc_AncientSkeletalSoldier' WHERE (`entry`='37012');
UPDATE `creature_template` SET `ScriptName`='npc_DeathboundWard' WHERE (`entry`='37007');
UPDATE `creature_template` SET `ScriptName`='npc_DeathspeakerAttedant' WHERE (`entry`='36811');

UPDATE `creature_template` SET `ScriptName`='npc_DeathspeakerDisciple' WHERE (`entry`='36807');
UPDATE `creature_template` SET `ScriptName`='npc_DeathspeakerHighPriest' WHERE (`entry`='36829');
UPDATE `creature_template` SET `ScriptName`='npc_DeathspeakerServant' WHERE (`entry`='36805');
UPDATE `creature_template` SET `ScriptName`='npc_DeathspeakerZealot' WHERE (`entry`='36808');

UPDATE `creature_template` SET `ScriptName`='npc_CultAdherent' WHERE (`entry`='37949');
UPDATE `creature_template` SET `ScriptName`='npc_CultFanatic' WHERE (`entry`='37890');
