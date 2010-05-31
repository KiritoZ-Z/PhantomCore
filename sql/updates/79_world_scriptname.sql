-- instance
UPDATE `instance_template` SET `script`='instance_trial_of_the_crusader' WHERE `map`=649;
DELETE FROM `creature` WHERE `map` = 649 AND `id` IN 
(34797,34796,34799,35144,34780,34460,34463,34461,34471,34475,34472,34453,34455,34458,34454,34451,34456,34497,34496,34564,34467,35465,34468,35610,34473,34474,34441,34449,34448,34450,34606, 34605, 34607, 34564);

-- announcers
UPDATE `creature_template` SET `npcflag`=1, `scriptname`='npc_toc_announcer' WHERE `entry`=34816;
DELETE FROM `creature` WHERE `map` = 649 AND `id` = 35766;

DELETE FROM `npc_text` WHERE `ID` IN (724001, 724002, 724003, 724004, 724005, 724006); 
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(724001, 'Greetings $N! Are you ready to be tested in Crusaders Coliseum?'),
(724002, 'Are you ready for the next stage?'),
(724003, 'Are you ready to fight the champions of the Silver vanguard?'),
(724004, 'Are you ready for the next stage?'),
(724005, 'Are you ready to continue battle with Anub-Arak?'),
(724006, 'Today, the arena is closed. Script dungeon designed specifically for server Pandora http://wow.teletoria.ru (c) /dev/rsa 2010');

DELETE FROM `locales_npc_text` WHERE `entry` IN (724001, 724002, 724003, 724004, 724005, 724006); 
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc2`, `Text0_0_loc3`, `Text0_0_loc4`, `Text0_0_loc5`, `Text0_0_loc6`, `Text0_0_loc7`, `Text0_0_loc8`) VALUES
(724001, 'Greetings $N! Are you ready to be tested in Crusaders Coliseum?', NULL, NULL, NULL, NULL, NULL, NULL, 'Приветствую, $N! Вы готовы пройти Испытание Крестоносца?'),
(724002, 'Are you ready for the next stage?', NULL, NULL, NULL, NULL, NULL, NULL, 'Вы готовы к следующему этапу?'),
(724003, 'Are you ready to fight the champions of the Silver vanguard?', NULL, NULL, NULL, NULL, NULL, NULL, 'Вы готовы драться с чемпионами Серебряного авангарда?'),
(724004, 'Are you ready for the next stage?', NULL, NULL, NULL, NULL, NULL, NULL, 'Вы готовы к следующему этапу?'),
(724005, 'Are you ready to continue battle with Anub-Arak?', NULL, NULL, NULL, NULL, NULL, NULL, 'Вы готовы продолжить бой с Ануб-Араком?'),
(724006, 'Today, the arena is closed.', NULL, NULL, NULL, NULL, NULL, NULL, 'На сегодня арена закрыта.');

UPDATE `creature_template` SET `scriptname`='boss_lich_king_toc' WHERE `entry`=35877;
UPDATE `creature_template` SET  `faction_A`= 1770, `faction_H` = 1770,  `scriptname`='npc_fizzlebang_toc' WHERE `entry`=35458;
UPDATE `creature_template` SET `scriptname`='npc_tirion_toc' WHERE `entry`=34996;
UPDATE `creature_template` SET `scriptname`='npc_garrosh_toc' WHERE `entry`=34995;
UPDATE `creature_template` SET `scriptname`='npc_rinn_toc' WHERE `entry`=34990;

-- Grand crusaders
UPDATE `creature_template` SET `scriptname`='mob_toc_warrior', `AIName` ='' WHERE `entry` IN (34475,34453);
UPDATE `creature_template` SET `scriptname`='mob_toc_mage', `AIName` ='' WHERE `entry` IN (34468,34449);
UPDATE `creature_template` SET `scriptname`='mob_toc_shaman', `AIName` ='' WHERE `entry` IN (34463,34455);
UPDATE `creature_template` SET `scriptname`='mob_toc_enh_shaman', `AIName` ='' WHERE `entry` IN (34470,34444);
UPDATE `creature_template` SET `scriptname`='mob_toc_hunter', `AIName` ='' WHERE `entry` IN (34467,34448);
UPDATE `creature_template` SET `scriptname`='mob_toc_rogue', `AIName` ='' WHERE `entry` IN (34472,34454);
UPDATE `creature_template` SET `scriptname`='mob_toc_priest', `AIName` ='' WHERE `entry` IN (34466,34456);
UPDATE `creature_template` SET `scriptname`='mob_toc_shadow_priest', `AIName` ='' WHERE `entry` IN (34473,34441);
UPDATE `creature_template` SET `scriptname`='mob_toc_dk', `AIName` ='' WHERE `entry` IN (34461,34458);
UPDATE `creature_template` SET `scriptname`='mob_toc_paladin', `AIName` ='' WHERE `entry` IN (34465,34445);
UPDATE `creature_template` SET `scriptname`='mob_toc_retro_paladin', `AIName` ='' WHERE `entry` IN (34471,34456);
UPDATE `creature_template` SET `scriptname`='mob_toc_druid', `AIName` ='' WHERE `entry` IN (34460,34451);
UPDATE `creature_template` SET `scriptname`='mob_toc_boomkin', `AIName` ='' WHERE `entry` IN (34469,34459);
UPDATE `creature_template` SET `scriptname`='mob_toc_warlock' WHERE `entry` IN (34474,34450);

UPDATE `creature_template` SET `scriptname`='mob_toc_pet_warlock', `AIName` ='' WHERE `entry` IN (35465);
UPDATE `creature_template` SET `scriptname`='mob_toc_pet_hunter', `AIName` ='' WHERE `entry` IN (35610);

UPDATE `creature_template` SET `lootid`= 0 WHERE  `entry` IN
(34460,34463,34461,34471,34475,34472,34453,34455,34458,34454,34451,34456,34467,35465,34468,35610,34473,34474,34441,34449,34448,34450);
UPDATE `creature_template` SET `lootid`= 0 WHERE  `entry` IN
(12266,12209,12212,12281,12190,12284,12269,12272,12229,12187,12091,12088,12169,12103,12106,12112,12166,12163,12175,12183,12303,12300);
UPDATE `creature_template` SET `lootid`= 0 WHERE  `entry` IN
(12267,12210,12213,12282,12191,12285,12270,12273,12230,12188,12092,12089,12170,12104,12107,12113,12167,12164,12181,12184,12304,12301);
UPDATE `creature_template` SET `lootid`= 0 WHERE  `entry` IN
(12268,12211,12214,12283,12192,12286,12271,12274,12231,12189,12093,12090,12171,12105,12108,12114,12168,12165,12182,12185,12305,12302);

-- N10
DELETE FROM `creature_loot_template` WHERE `entry` IN 
(34460,34463,34461,34471,34475,34472,34453,34455,34458,34454,34451,34456,34467,35465,34468,35610,34473,34474,34441,34449,34448,34450);
-- H10
DELETE FROM `creature_loot_template` WHERE `entry` IN 
(12266,12209,12212,12281,12190,12284,12269,12272,12229,12187,12091,12088,12169,12103,12106,12112,12166,12163,12175,12183,12303,12300);
-- N25
DELETE FROM `creature_loot_template` WHERE `entry` IN 
(12267,12210,12213,12282,12191,12285,12270,12273,12230,12188,12092,12089,12170,12104,12107,12113,12167,12164,12181,12184,12304,12301);
-- H25
DELETE FROM `creature_loot_template` WHERE `entry` IN 
(12268,12211,12214,12283,12192,12286,12271,12274,12231,12189,12093,12090,12171,12105,12108,12114,12168,12165,12182,12185,12305,12302);

-- Nortrend beasts
DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (34796, 34799, 35144, 34797);
UPDATE `creature_template` SET `scriptname`='boss_gormok', `AIName` ='' WHERE `entry`=34796;
UPDATE `creature_template` SET `scriptname`='mob_snobold_vassal', `AIName` ='' WHERE `entry`=34800;

UPDATE `creature_template` SET `scriptname`='boss_dreadscale', `AIName` ='' WHERE `entry`=34799;
UPDATE `creature_template` SET `scriptname`='boss_acidmaw', `AIName` ='' WHERE `entry`=35144;
UPDATE `creature_template` SET `scriptname`='mob_slime_pool', `minlevel` = 80, `maxlevel` = 80, `AIName` ='', `faction_A`= 14, `faction_H` = 14, `modelid1` = 12349, `modelid3` = 12349 WHERE `entry` = 35176;
-- Model id for slime_pool need change!

UPDATE `creature_template` SET `scriptname`='boss_icehowl', `AIName` ='' WHERE `entry`=34797;

UPDATE `creature_template` SET `lootid`= 0 WHERE  `entry` IN (34796,34799,35144);
DELETE FROM `creature_loot_template`  WHERE  `entry` IN (34796,34799,35144);

-- Jaraxxus
UPDATE `creature_template` SET `scriptname`='boss_jaraxxus', `AIName` ='' WHERE `entry`= 34780;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (34780, 34784, 34813, 34815, 34825, 34826);
UPDATE `creature_template` SET `scriptname`='mob_legion_flame', `minlevel` = 82, `maxlevel` = 82, `modelid1` = 11686, `modelid2` = 11686, `modelid3` = 11686, `modelid4` = 11686, `AIName` ='', `faction_A`= 14, `faction_H` = 14 WHERE `entry` = 34784;
UPDATE `creature_template` SET `scriptname`='mob_infernal_volcano', `AIName` ='' WHERE `entry` = 34813;
UPDATE `creature_template` SET `scriptname`='mob_fel_infernal', `AIName` ='' WHERE `entry` = 34815;
UPDATE `creature_template` SET `scriptname`='mob_nether_portal', `AIName` ='' WHERE `entry` = 34825;
UPDATE `creature_template` SET `scriptname`='mob_mistress_of_pain', `AIName` ='' WHERE `entry` = 34826;

-- Valkiries
UPDATE `creature_template` SET `scriptname` = 'boss_fjola', `AIName` ='' WHERE `entry`=34497;
UPDATE `creature_template` SET `scriptname` = 'boss_eydis', `AIName` ='' WHERE `entry`=34496;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (34497, 34496, 34568, 34567);
UPDATE `creature_template` SET `npcflag`=1, `scriptname`='mob_light_essence', `AIName` ='' WHERE entry = 34568;
UPDATE `creature_template` SET `npcflag`=1, `scriptname`='mob_dark_essence', `AIName` ='' WHERE entry = 34567;
UPDATE `creature_template` SET `faction_A` = 14, `minlevel` = 82, `maxlevel` = 82,`faction_H` = 14, `AIName` ='', `scriptname`='mob_unleashed_dark' WHERE entry = 34628;
UPDATE `creature_template` SET `faction_A` = 14, `minlevel` = 82, `maxlevel` = 82,`faction_H` = 14, `AIName` ='', `scriptname`='mob_unleashed_light' WHERE entry = 34630;

-- Anub'arak
UPDATE `creature_template` SET `scriptname`='boss_anubarak_trial', `unit_flags` = 0, `AIName` ='' WHERE `entry`=34564;

DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (34606, 34605, 34607, 34564, 34660);
UPDATE `creature_template` SET `modelid1` = 25144, `modelid2` = 0, `modelid3` = 25144, `modelid4` = 0, `faction_A` = 14, `faction_H` = 14, `AIName` = '', `ScriptName` = 'mob_frost_sphere' WHERE `entry` = 34606;
UPDATE `creature_template` SET `scriptname`='mob_swarm_scarab', `AIName` ='' WHERE `entry`=34605;
UPDATE `creature_template` SET `scriptname`='mob_nerubian_borrower', `AIName` ='' WHERE `entry`=34607;
UPDATE `creature_template` SET `scriptname`='mob_anubarak_spike', `faction_A` = 14, `minlevel` = 80, `maxlevel` = 80,`faction_H` = 14, `AIName` ='' WHERE `entry`=34660;

-- Trial of the crusader spelltable
DELETE FROM `boss_spell_table` WHERE `entry` IN 
(34496,34497,34564,34605,34607,34780,34784,34796,34797,34799, 34800, 34813, 34815, 34826, 35144, 35176, 34606, 34660);

-- Eydis Darkbane
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34496, 64238, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34496, 65768, 67262, 67263, 67264, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34496, 65874, 67256, 67257, 67258, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34496, 65876, 67306, 67307, 67308, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34496, 65879, 67244, 67245, 67246, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34496, 65916, 67248, 67249, 67250, 15000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34496, 66058, 67182, 67183, 67184, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34496, 66069, 67309, 67310, 67311, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL),
(34496, 67282, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(34496, 34628, 40000, 40000, 1, 1, 2, 2, 10, 100, 0, 11);

-- Fjola Lightbane
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34497, 64238, 64238, 64238, 64238, 600000, 600000, 600000, 600000, 600000, 600000, 600000, 600000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34497, 65766, 67270, 67271, 67272, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34497, 65858, 67259, 67260, 67261, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34497, 65875, 67303, 67304, 67305, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34497, 65916, 67248, 67249, 67250, 15000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34497, 66046, 67206, 67207, 67208, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34497, 66075, 67312, 67313, 67314, 10000, 10000, 10000, 10000, 20000, 20000, 20000, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL),
(34497, 67297, 67297, 67298, 67298, 10000, 10000, 10000, 10000, 15000, 15000, 15000, 15000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(34497, 34630, 20000, 20000, 1, 1, 2, 2, 10, 100, 0, 11);

-- AnubArak
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34564, 26662, 26662, 26662, 26662, 600000, 600000, 600000, 600000, 600000, 600000, 600000, 600000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34564, 34605, 0, 0, 0, 90000, 0, 0, 0, 90000, 0, 0, 0, 0, 0, 0, 0, 20.0, 100.0, 0, 0, 0, 0, 9, 1, 0, 0, NULL),
(34564, 34660, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 0, 5.0, 10.0, 0, 0, 0, 0, 9, 1, 0, 0, NULL),
(34564, 34606, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 0, 0, 0, 0, 10.0, 100.0, 0, 0, 0, 0, 9, 1, 0, 0, NULL),
(34564, 34607, 0, 0, 0, 70000, 0, 0, 0, 90000, 0, 0, 0, 0, 0, 0, 0, 20.0, 100.0, 0, 0, 0, 0, 9, 1, 0, 0, NULL),
(34564, 53421, 53421, 53421, 53421, 45000, 45000, 45000, 45000, 60000, 60000, 60000, 60000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, NULL),
(34564, 66169, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34564, 66012, 66012, 66012, 66012, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34564, 66013, 67700, 68509, 68510, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34564, 66339, 66339, 66339, 66339, 5000, 5000, 5000, 5000, 10000, 10000, 10000, 10000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 0, 0, NULL),
(34564, 67574, 0, 0, 0, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1, 0, NULL),
(34564, 66118, 67630, 68646, 68647, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34564, 66240, 0, 0, 0, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34564, 66125, 0, 0, 0, 10000, 0, 0, 0, 10000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34564, 67730, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL);

-- Anub'arak scarab
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34605, 66092, 66092, 66092, 66092, 5000, 5000, 5000, 5000, 20000, 20000, 20000, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34605, 67861, 66092, 66092, 66092, 5000, 5000, 5000, 5000, 20000, 20000, 20000, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL);

-- Cold sphere
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34606, 66193, 67855, 67856, 67857, 5000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL);

-- Anub'arak spike
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34660, 67574, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, NULL),
(34660, 66193, 67855, 67856, 67857, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34660, 65920, 65921, 65922, 65923, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL);

-- Nerubian Borrower
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34607, 66129, 66129, 66129, 66129, 10000, 10000, 10000, 10000, 20000, 20000, 20000, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, NULL),
(34607, 67322, 67322, 67322, 67322, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, NULL),
(34607, 67847, 67847, 67847, 67847, 5000, 5000, 5000, 5000, 20000, 20000, 20000, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL);

-- Jaraxxus
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34780, 26662, 26662, 26662, 26662, 600000, 600000, 600000, 600000, 600000, 600000, 600000, 600000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34780, 66197, 68123, 68124, 68125, 30000, 30000, 30000, 30000, 45000, 45000, 45000, 45000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34780, 66237, 67049, 67050, 67051, 40000, 40000, 40000, 40000, 90000, 90000, 40000, 90000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, NULL),
(34780, 66242, 67060, 67060, 67060, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34780, 66264, 66264, 68405, 68405, 60000, 60000, 60000, 60000, 60000, 60000, 60000, 60000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 0, NULL),
(34780, 66528, 66528, 67029, 67029, 15000, 15000, 15000, 15000, 25000, 25000, 25000, 25000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34780, 66532, 66963, 66964, 66965, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34780, 67108, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34780, 66255, 0, 0, 0, 30000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34780, 34825, 0, 0, 0, 60000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 0, 20.0, 80.0, 0, 0, 0, 0, 10, 0, 0, 0, NULL),
(34780, 34813, 0, 0, 0, 60000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 0, 20.0, 60.0, 0, 0, 0, 0, 10, 0, 0, 0, NULL);

-- NPC Legion flame
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34784, 66199, 68127, 68126, 68128, 30000, 30000, 30000, 30000, 45000, 45000, 45000, 45000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL);

-- Gormok
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34796, 34800, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 20.0, 80.0, 0, 0, 0, 0, 9, 0, 0, 0, NULL),
(34796, 66331, 67477, 67478, 67479, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34796, 66636, 0, 0, 0, 15000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 1, 0, NULL),
(34796, 67648, 0, 0, 0, 15000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL);

-- Icehowl
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34797, 66683, 67660, 67661, 67662, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34797, 66689, 67650, 67651, 67652, 25000, 25000, 25000, 25000, 40000, 40000, 40000, 40000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34797, 66734, 0, 0, 0, 4000, 4000, 3000, 3000, 4000, 4000, 3000, 3000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, NULL),
(34797, 66770, 67654, 67655, 67656, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL),
(34797, 66758, 0, 0, 0, 15000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34797, 68667, 0, 0, 0, 8000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 0, 200.0, 0, 0, 0, 0, 0, 12, 0, 1, 0, NULL),
(34797, 66759, 0, 0, 0, 300000, 0, 0, 0, 300000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34797, 67345, 67663, 67664, 67665, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL);

-- Dreadscale
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34799, 53421, 0, 0, 0, 40000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34799, 66794, 67644, 67645, 67646, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34799, 66796, 67632, 67633, 67634, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34799, 66821, 66821, 66821, 66821, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34799, 66879, 67624, 67625, 67626, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34799, 66902, 67627, 67628, 67629, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL),
(34799, 66883, 67641, 67642, 67643, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(34799, 35176, 0, 0, 0, 30000, 30000, 45000, 60000, 30000, 30000, 45000, 60000, 0, 0, 0, 0, 1, 5, 0, 0, 0, 0, 11, 0, 0, 0, NULL),
(34799, 68335, 68335, 68335, 68335, 20000, 20000, 20000, 20000, 20000, 20000, 20000, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL);

-- Snobold vassal
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34800, 66313, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, NULL),
(34800, 66317, 0, 0, 0, 10000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, NULL),
(34800, 66318, 0, 0, 0, 10000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, NULL),
(34800, 66406, 0, 0, 0, 10000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 0, NULL),
(34800, 66407, 0, 0, 0, 10000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL),
(34800, 66408, 0, 0, 0, 10000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL),
(34800, 66636, 0, 0, 0, 15000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 0, NULL);

-- Infernal volcano
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34813, 66255, 0, 0, 0, 30000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(34813, 66258, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, NULL);

-- Fel infernal
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34815, 66494, 66494, 66494, 66494, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34815, 67047, 67047, 67047, 67047, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL);

-- Mistress of pain
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(34826, 66316, 66316, 66316, 66316, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(34826, 67098, 67098, 67098, 67098, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL);

-- Acidmaw
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(35144, 53421, 0, 0, 0, 40000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, NULL),
(35144, 66794, 67644, 67645, 67646, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(35144, 66819, 66819, 66819, 66819, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(35144, 66824, 67612, 67613, 67614, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, NULL),
(35144, 66880, 67606, 67607, 67608, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(35144, 66901, 67615, 67616, 66617, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, NULL),
(35144, 66883, 67641, 67642, 67643, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, NULL),
(35144, 35176, 0, 0, 0, 30000, 30000, 45000, 60000, 30000, 30000, 45000, 60000, 0, 0, 0, 0, 1, 5, 0, 0, 0, 0, 11, 0, 0, 0, NULL),
(35144, 68335, 68335, 68335, 68335, 20000, 20000, 20000, 20000, 20000, 20000, 20000, 20000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL);

-- Slime pool
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `varData`, `StageMask_N`, `StageMask_H`, `CastType`, `isVisualEffect`, `isBugged`, `textEntry`, `comment`) VALUES
(35176, 66881, 67638, 67639, 67640, 20000, 20000, 20000, 20000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, NULL),
(35176, 66883, 67641, 67642, 67643, 15000, 15000, 15000, 15000, 30000, 30000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL),
(35176, 66882, 0, 0, 0, 500, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL);

-- Retro Paladins
DELETE FROM `boss_spell_table` WHERE `entry` IN (34471,34456);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34471, 66011, 0, 180000, 180000, 3),
(34471, 66003, 0, 6000, 18000, 3),
(34471, 66010, 0, 0, 3600001, 1),
(34471, 66006, 0, 10000, 10000, 3),
(34471, 66007, 0, 40000, 40000, 3),
(34471, 66009, 0, 300000, 300000, 1),
(34471, 66005, 68018, 8000, 15000, 3),
(34471, 66008, 0, 60000, 60000, 4),
(34471, 66004, 68021, 10000, 15000, 1);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34456, 66011, 0, 180000, 180000, 3),
(34456, 66003, 0, 6000, 18000, 3),
(34456, 66010, 0, 0, 3600001, 1),
(34456, 66006, 0, 10000, 10000, 3),
(34456, 66007, 0, 40000, 40000, 3),
(34456, 66009, 0, 300000, 300000, 1),
(34456, 66005, 68018, 8000, 15000, 3),
(34456, 66008, 0, 60000, 60000, 4),
(34456, 66004, 68021, 10000, 15000, 1);

-- Pet's
DELETE FROM `boss_spell_table` WHERE `entry` IN (35465,35610);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(35465, 67518, 0, 15000, 30000, 3),
(35465, 67519, 0, 15000, 30000, 3);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(35610, 67793, 67980, 67981, 67982, 5000, 10000, 3);

-- Druids
DELETE FROM `boss_spell_table` WHERE `entry` IN (34460,34451);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34460, 66093, 67957, 5000, 15000, 14),
(34460, 66066, 67965, 10000, 20000, 14),
(34460, 66067, 67968, 10000, 20000, 14),
(34460, 66065, 67971, 10000, 20000, 14),
(34460, 66086, 67974, 40000, 90000, 1),
(34460, 65860, 0, 45000, 90000, 1),
(34460, 66068, 0, 15000, 30000, 6),
(34460, 66071, 0, 40000, 80000, 1);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34451, 66093, 67957, 10000, 20000, 14),
(34451, 66066, 67965, 10000, 20000, 14),
(34451, 66067, 67968, 10000, 20000, 14),
(34451, 66065, 67971, 10000, 20000, 14),
(34451, 66086, 67974, 40000, 90000, 1),
(34451, 65860, 0, 45000, 90000, 1),
(34451, 66068, 0, 15000, 30000, 6),
(34451, 66071, 0, 40000, 80000, 1);

-- Warriors
DELETE FROM `boss_spell_table` WHERE `entry` IN (34475,34453);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34475, 65947, 0, 20000, 30000, 1),
(34475, 65930, 0, 10000, 60000, 3),
(34475, 65926, 0, 6000, 25000, 3),
(34475, 68764, 0, 3000, 25000, 3),
(34475, 65935, 0, 20000, 80000, 3),
(34475, 65924, 0, 30000, 90000, 1),
(34475, 65936, 0, 5000, 25000, 3),
(34475, 65940, 0, 10000, 25000, 3),
(34475, 65932, 0, 30000, 60000, 1);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34453, 65947, 0, 20000, 30000, 1),
(34453, 65930, 0, 10000, 60000, 3),
(34453, 65926, 0, 6000, 25000, 3),
(34453, 68764, 0, 3000, 25000, 3),
(34453, 65935, 0, 20000, 80000, 3),
(34453, 65924, 0, 30000, 90000, 1),
(34453, 65936, 0, 5000, 25000, 3),
(34453, 65940, 0, 10000, 25000, 3),
(34453, 65932, 0, 30000, 60000, 1);

-- Mage
DELETE FROM `boss_spell_table` WHERE `entry` IN (34468,34449);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34468, 65799, 67995, 3000, 10000, 3),
(34468, 65791, 67998, 5000, 15000, 3),
(34468, 65800, 68001, 5000, 15000, 3),
(34468, 65793, 0, 7000, 25000, 1),
(34468, 65807, 68004, 5000, 15000, 4),
(34468, 65790, 0, 5000, 15000, 6),
(34468, 65792, 0, 7000, 15000, 1),
(34468, 65802, 0, 0, 3600001, 1),
(34468, 65801, 0, 15000, 40000, 4);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34449, 65799, 67995, 3000, 10000, 3),
(34449, 65791, 67998, 5000, 15000, 3),
(34449, 65800, 68001, 5000, 15000, 3),
(34449, 65793, 0, 7000, 25000, 1),
(34449, 65807, 68004, 5000, 15000, 4),
(34449, 65790, 0, 5000, 15000, 6),
(34449, 65792, 0, 7000, 15000, 1),
(34449, 65802, 0, 0, 3600001, 1),
(34449, 65801, 0, 15000, 40000, 4);

-- Shaman
DELETE FROM `boss_spell_table` WHERE `entry` IN (34463,34455);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34463, 66055, 68115, 5000, 15000, 14),
(34463, 66053, 68118, 5000, 15000, 14),
(34463, 66056, 0, 5000, 15000, 14),
(34463, 65983, 0, 30000, 60000, 1),
(34463, 65980, 0, 5000, 15000, 6),
(34463, 66054, 0, 10000, 40000, 4),
(34463, 66063, 0, 5000, 15000, 14),
(34463, 65973, 68100, 5000, 15000, 4);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34455, 66055, 68115, 5000, 15000, 14),
(34455, 66053, 68118, 5000, 15000, 14),
(34455, 66056, 0, 5000, 15000, 14),
(34455, 65983, 0, 30000, 60000, 1),
(34455, 65980, 0, 5000, 15000, 6),
(34455, 66054, 0, 10000, 40000, 4),
(34455, 66063, 0, 5000, 15000, 14),
(34455, 65973, 68100, 5000, 15000, 4);

-- Enh shaman
DELETE FROM `boss_spell_table` WHERE `entry` IN (34470,34444);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34470, 65973, 0, 5000, 8000, 4),
(34470, 65974, 0, 5000, 8000, 3),
(34470, 65983, 0, 25000, 600000, 1),
(34470, 65970, 0, 5000, 90000, 3);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34444, 65973, 0, 5000, 8000, 4),
(34444, 65974, 0, 5000, 8000, 3),
(34444, 65983, 0, 25000, 600000, 1),
(34444, 65970, 0, 5000, 90000, 3);

-- Hunter
DELETE FROM `boss_spell_table` WHERE `entry` IN (34467,34448);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34467, 65583, 67978, 3000, 8000, 3),
(34467, 65871, 0, 20000, 120000, 1),
(34467, 65869, 0, 12000, 20000, 1),
(34467, 65866, 67984, 3000, 8000, 3),
(34467, 65880, 0, 12000, 30000, 1),
(34467, 65868, 67989, 4000, 8000, 3),
(34467, 65867, 0, 4000, 8000, 3),
(34467, 66207, 0, 4000, 8000, 3),
(34467, 65877, 0, 7000, 60000, 4);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34448, 65583, 67978, 3000, 8000, 3),
(34448, 65871, 0, 20000, 120000, 1),
(34448, 65869, 0, 12000, 20000, 1),
(34448, 65866, 67984, 3000, 8000, 3),
(34448, 65880, 0, 12000, 30000, 1),
(34448, 65868, 67989, 4000, 8000, 3),
(34448, 65867, 0, 4000, 8000, 3),
(34448, 66207, 0, 4000, 8000, 3),
(34448, 65877, 0, 7000, 60000, 4);

-- Rogue
DELETE FROM `boss_spell_table` WHERE `entry` IN (34472,34454);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34472, 65955, 0, 8000, 10000, 3),
(34472, 65956, 0, 12000, 120000, 1),
(34472, 65960, 0, 7000, 8000, 6),
(34472, 65961, 0, 20000, 120000, 1),
(34472, 66178, 0, 10000, 8000, 3),
(34472, 65954, 0, 5000, 8000, 3),
(34472, 65957, 68095, 15000, 20000, 3);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34454, 65955, 0, 8000, 10000, 3),
(34454, 65956, 0, 12000, 120000, 1),
(34454, 65960, 0, 7000, 8000, 6),
(34454, 65961, 0, 20000, 120000, 1),
(34454, 66178, 0, 10000, 8000, 3),
(34454, 65954, 0, 5000, 8000, 3),
(34454, 65957, 68095, 15000, 20000, 3);

-- Priest
DELETE FROM `boss_spell_table` WHERE `entry` IN (34466,34456);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34466, 66177, 68035, 3000, 8000, 14),
(34466, 66099, 68032, 3000, 8000, 14),
(34466, 66104, 68023, 3000, 8000, 14),
(34466, 66100, 68026, 3000, 8000, 4),
(34466, 65546, 0, 3000, 8000, 6),
(34466, 65543, 0, 5000, 25000, 1);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34456, 66177, 68035, 3000, 8000, 14),
(34456, 66099, 68032, 3000, 8000, 14),
(34456, 66104, 68023, 3000, 8000, 14),
(34456, 66100, 68026, 3000, 8000, 4),
(34456, 65546, 0, 3000, 8000, 6),
(34456, 65543, 0, 5000, 25000, 1);

-- Shadow priest
DELETE FROM `boss_spell_table` WHERE `entry` IN (34473,34441);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34473, 65542, 0, 8000, 15000, 6),
(34473, 65490, 68091, 3000, 8000, 4),
(34473, 65541, 68088, 3000, 8000, 4),
(34473, 65488, 68042, 3000, 8000, 3),
(34473, 65492, 68038, 3000, 8000, 3),
(34473, 65545, 0, 3000, 8000, 3),
(34473, 65544, 0, 1000, 180000, 1),
(34473, 65546, 0, 3000, 8000, 4),
(34473, 65543, 0, 8000, 24000, 1),
(34473, 16592, 0, 3000, 8000, 1);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34441, 65542, 0, 8000, 15000, 6),
(34441, 65490, 68091, 3000, 8000, 4),
(34441, 65541, 68088, 3000, 8000, 4),
(34441, 65488, 68042, 3000, 8000, 3),
(34441, 65492, 68038, 3000, 8000, 3),
(34441, 65545, 0, 3000, 8000, 3),
(34441, 65544, 0, 1000, 180000, 1),
(34441, 65546, 0, 3000, 8000, 4),
(34441, 65543, 0, 8000, 24000, 1),
(34441, 16592, 0, 3000, 8000, 1);

-- Death knight
DELETE FROM `boss_spell_table` WHERE `entry` IN (34461,34458);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34461, 66020, 0, 5000, 15000, 3),
(34461, 66019, 67930, 5000, 15000, 3),
(34461, 66017, 0, 5000, 15000, 3),
(34461, 66047, 67936, 5000, 15000, 3),
(34461, 66023, 0, 5000, 90000, 1),
(34461, 66021, 67939, 8000, 12000, 3),
(34461, 66018, 0, 10000, 90000, 6);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34458, 66020, 0, 5000, 15000, 3),
(34458, 66019, 67930, 5000, 15000, 3),
(34458, 66017, 0, 5000, 15000, 3),
(34458, 66047, 67936, 5000, 15000, 3),
(34458, 66023, 0, 5000, 90000, 1),
(34458, 66021, 67939, 8000, 12000, 3),
(34458, 66018, 0, 10000, 90000, 6);

-- Paladin
DELETE FROM `boss_spell_table` WHERE `entry` IN (34465,34445);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34465, 68757, 0, 25000, 40000, 6),
(34465, 66010, 0, 0, 3600001, 14),
(34465, 66116, 0, 5000, 15000, 14),
(34465, 66113, 68008, 5000, 10000, 14),
(34465, 66112, 68011, 5000, 15000, 14),
(34465, 66009, 0, 0, 3600001, 6),
(34465, 66114, 68015, 6000, 15000, 14),
(34465, 66613, 0, 5000, 15000, 4);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34445, 68757, 0, 25000, 40000, 6),
(34445, 66010, 0, 0, 3600001, 14),
(34445, 66116, 0, 5000, 15000, 14),
(34445, 66113, 68008, 5000, 10000, 14),
(34445, 66112, 68011, 5000, 15000, 14),
(34445, 66009, 0, 0, 3600001, 6),
(34445, 66114, 68015, 6000, 15000, 14),
(34445, 66613, 0, 5000, 15000, 4);

-- Boomkin (druid in moonkin form)
DELETE FROM `boss_spell_table` WHERE `entry` IN (34469,34459);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34469, 65859, 0, 5000, 40000, 4),
(34469, 65857, 0, 5000, 40000, 3),
(34469, 65863, 0, 10000, 40000, 4),
(34469, 65861, 0, 25000, 40000, 3),
(34469, 65855, 67942, 25000, 40000, 4),
(34469, 65856, 67945, 5000, 40000, 3),
(34469, 65854, 67948, 25000, 40000, 3),
(34469, 65860, 0, 5000, 120000, 1),
(34469, 65862, 67952, 25000, 40000, 3);

-- Warlock
DELETE FROM `boss_spell_table` WHERE `entry` IN (34474,34450);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34474, 65816, 68146, 15000, 30000, 1),
(34474, 65810, 68134, 15000, 30000, 4),
(34474, 65814, 68137, 15000, 30000, 4),
(34474, 65815, 0, 15000, 30000, 4),
(34474, 65809, 0, 4000, 15000, 4),
(34474, 65819, 68149, 15000, 30000, 3),
(34474, 65821, 68152, 3000, 10000, 3),
(34474, 65812, 68155, 2000, 10000, 4);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_H10`, `timerMin_N10`, `timerMax_N10`, `CastType`) VALUES
(34450, 65816, 68146, 15000, 30000, 1),
(34450, 65810, 68134, 15000, 30000, 4),
(34450, 65814, 68137, 15000, 30000, 4),
(34450, 65815, 0, 15000, 30000, 4),
(34450, 65809, 0, 4000, 15000, 4),
(34450, 65819, 68149, 15000, 30000, 3),
(34450, 65821, 68152, 3000, 10000, 3),
(34450, 65812, 68155, 2000, 10000, 4);

DELETE FROM `script_texts` WHERE `entry` BETWEEN -1713799 AND -1713499;
INSERT INTO `script_texts`
(`comment`,`sound`, `entry`,`content_loc8`,`type`,`language`,`emote`,`content_default`) VALUES
('34996','16036','-1713500','Добро пожаловать, герои! Вы услышали призыв Серебряного Авангарда и без колебаний откликнулись на него! В этом колизее вам предстоит сразиться с опаснейшими противниками. Те из вас, кто смогут пережить испытания, войдут в ряды Серебряного Авангарда, который направится в Цитадель Ледяной Короны.','6','0','0','Welcome champions, you have heard the call of the argent crusade and you have boldly answered. It is here in the crusaders coliseum that you will face your greatest challenges. Those of you who survive the rigors of the coliseum will join the Argent Crusade on its marsh to ice crown citadel.'),
('34996','16038','-1713501','Из самых глубоких и темных пещер Грозовой Гряды был призван Гормок Пронзающий Бивень! В бой, герои!','6','0','0','Hailing from the deepest, darkest carverns of the storm peaks, Gormok the Impaler! Battle on, heroes!'),
('34990','16069','-1713502','Твои чудовища не чета героям Альянса, Тирион!','6','0','0','Your beast will be no match for my champions Tirion!'),
('34995','16026','-1713702','Я видел и более достойных соперников в багровом круге. Ты напрасно тратишь наше время, паладин.','6','0','0','Your beast will be no match for my champions Tirion!'),
('34796','0','-1713601','Мои рабы! Уничтожьте врага!','3','0','0','My slaves! Destroy the enemy!'),
('34996','16039','-1713503','Приготовьтесь к схватке с близнецами-чудовищами, Кислотной Утробой и Жуткой Чешуей!','6','0','0','Steel yourselves, heroes, for the twin terrors Acidmaw and Dreadscale. Enter the arena!'),
('34799','0','-1713504','После гибели товарища %s приходит в ярость!','3','0','0','After the death of sister %s goes berserk!'),
('34996','16040','-1713505','В воздухе повеяло ледяным дыханием следующего бойца: на арену выходит Ледяной Рев! Сражайтесь или погибните, чемпионы!','6','0','0','The air freezes with the introduction of our next combatant, Icehowl! Kill or be killed, champions!'),
('34797','0','-1713506','%s глядит на |3-3($n) и испускает гортанный вой!','3','0','0','%S looks at |3-3($n) and emits a guttural howl!'),
('34797','0','-1713507','%s врезается в стену Колизея и теряет ориентацию!','3','0','0','%S crashes into a wall of the Colosseum and lose focus!'),
('34797','0','-1713508','|3-3(%s) охватывает кипящая ярость, и он растаптывает всех на своем пути!','3','0','0','|3-3(%s) covers boiling rage, and he tramples all in its path!'),
('34996','16041','-1713509','Все чудовища повержены!','6','0','0','All the monsters defeated!'),
('34996','16042','-1713709','Прискорбно. Как яростно они не бились, чудовища Нордскола оказались сильнее. Почтим память павших героев минутой молчания.','6','0','0',''),
('34996','16043','-1713510','Сейчас великий чернокнижник Вилфред Непопамс призовет вашего нового противника. Готовьтесь к бою!','6','0','0','Grand Warlock Wilfred Fizzlebang will summon forth your next challenge. Stand by for his entry!'),
('35458','16268','-1713511','Благодарю, Верховный лорд. А теперь, смельчаки, я приступаю к ритуалу призыва. Когда я закончу, появится грозный демон!','6','0','0','Thank you, Highlord! Now challengers, I will begin the ritual of summoning! When I am done, a fearsome Doomguard will appear!'),
('35458','16269','-1713512','Готовьтесь к забвению!','6','0','0','Prepare for oblivion!'),
('35458','16270','-1713513','АГА! Получилось! Трепещи перед всевластным Вилфредом Непопамсом, мастером призыва! Ты покорен мне, демон!','6','0','0','Ah ha! Behold the absolute power of Wilfred Fizzlebang, master summoner! You are bound to ME, demon!'),
('34780','16143','-1713514','Ничтожный гном! Тебя погубит твоя самоуверенность!','6','0','0','Trifling gnome, your arrogance will be your undoing!'),
('35458','16271','-1713515','Тут я главный!','6','0','0','But I am in charge here-'),
('35458','0','-1713715','Ну вот, опять я облажался...','6','0','0','Agonized Scream!!!'),
('34996','16044','-1713516','Быстрей, герои, расправьтесь с повелителем демонов, прежде чем он откроет портал в свое темное царство!','6','0','0','Quickly, heroes! Destroy the demon lord before it can open a portal to its twisted demonic realm!'),
('34780','16144','-1713517','Перед вами Джараксус, эредарский повелитель Пылающего Легиона!','6','0','0','You face Jaraxxus, eredar lord of the Burning Legion!'),
('34780','0','-1713518','На вас направлено |cFFFF0000Пламя Легиона!|r','3','0','0','You have been sent |cFFFF0000Plamya Legion!|R'),
('34780','0','-1713519','%s создает врата Пустоты!','3','0','0','%S creates the gates of the Void!'),
('34780','0','-1713520','%s создает |cFF00FF00Вулкан инферналов!|r','3','0','0','%S creates |cFF00FF00Vulkan Infernals!|R'),
('34780','16150','-1713521','Явись, сестра! Господин зовет!','6','0','0','Come forth, sister! Your master calls!'),
('34780','0','-1713522','$n $gподвергся:подверглась; |cFF00FFFFИспепелению плоти!|r Исцелите $gего:ее;!','3','0','0','$N $gpodvergsya:been; |cFF00FFFFIspepeleniyu flesh!|R Heal $gego:it;!'),
('34780','16149','-1713523','ПЛОТЬ ОТ КОСТИ!','6','0','0','FLESH FROM BONE!'),
('34780','16151','-1713524','ИНФЕРНАЛ!','6','0','0','INFERNO!'),
('34780','16147','-1713525','Мое место займут другие. Ваш мир обречен...','6','0','0','Another will take my place. Your world is doomed.'),
('34996','16045','-1713526','Гибель Вилфреда Непопамса весьма трагична и должна послужить уроком тем, кто смеет беспечно играть с темной магией. К счастью, вы победили демона, и теперь вас ждет новый противник.','6','0','0','The loss of Wilfred Fizzlebang, while unfortunate, should be a lesson to those that dare dabble in dark magic. Alas, you are victorious and must now face the next challenge.'),
('34995','16021','-1713527','Подлые собаки Альянса! Вы выпустили повелителя демонов на воинов Орды? Ваша смерть будет быстрой!','6','0','0','Treacherous Alliance dogs! You summon a demon lord against warriors of the Horde!? Your deaths will be swift!'),
('34990','16064','-1713528','Альянсу не нужна помощь повелителя демонов, чтобы справиться с ордынским отродьем, пес!','6','0','0','The Alliance doesnt need the help of a demon lord to deal with Horde filth. Come, pig!'),
('34996','16046','-1713529','Тише! Успокойтесь! Никакого заговора здесь нет. Чернокнижник заигрался и поплатился за это. Турнир продолжается!','6','0','0','Everyone, calm down! Compose yourselves! There is no conspiracy at play here. The warlock acted on his own volition - outside of influences from the Alliance. The tournament must go on!'),
('34996','16047','-1713530','В следующем бою вы встретитесь с могучими рыцарями Серебряного Авангарда! Лишь победив их, вы заслужите достойную награду.','6','0','0','The next battle will be against the Argent Crusades most powerful knights! Only by defeating them will you be deemed worthy...'),
('34995','16023','-1713531','Орда требует справедливости! Мы вызываем Альянс на бой! Позволь нам встать на место твоих рыцарей, паладин. Мы покажем этим псам, как оскорблять Орду!','6','0','0','The Horde demands justice! We challenge the Alliance. Allow us to battle in place of your knights, paladin. We will show these dogs what it means to insult the Horde!'),
('34995','16066','-1713731','Они хотели запятнать честь Альянса, они пытались нас оклеветать! Я требую справедливости! Тириорн, позволь моим чемпионам сражаться вместо твоих рыцарей. Мы бросаем вызов Орде!','6','0','0','Our honor has been besmirched! They make wild claims and false accusations against us. I demand justice! Allow my champions to fight in place of your knights, Tirion. We challenge the Horde!'),
('34996','16048','-1713532','Хорошо. Да будет так. Сражайтесь с честью!','','0','0','Very well, I will allow it. Fight with honor!'),
('34995','16022','-1713533','Не щадите никого, герои Орды! ЛОК-ТАР ОГАР!','6','0','0','Show them no mercy, Horde champions! LOK-TAR OGAR!'),
('34995','16065','-1713733','Сражайтесь во славу Альянса, герои! Во имя вашего короля!','6','0','0','Fight for the glory of the Alliance, heroes! Honor your king and your people!'),
('34990','16067','-1713534','СЛАВА АЛЬЯНСУ!','6','0','0','GLORY OF THE ALLIANCE!'),
('34990','16024','-1713734','Это было лишь пробой того, что ждёт нас в будущем. За Орду!','6','0','0','LOK-TAR OGAR!'),
('34996','16049','-1713535','Пустая и горькая победа. После сегодняшних потерь мы стали слабее как целое. Кто еще, кроме Короля-лича, выиграет от подобной глупости? Пали великие воины. И ради чего? Истинная опасность еще впереди - нас ждет битва с Королем-личом.','6','0','0','A shallow and tragic victory. We are weaker as a whole from the losses suffered today. Who but the Lich King could benefit from such foolishness? Great warriors have lost their lives. And for what? The true threat looms ahead - the Lich King awaits us all in death.'),
('34996','16050','-1713536','Лишь сплотившись, вы сможете пройти последнее испытание. Из глубин Ледяной Короны навстречу вам подымаются две могучие воительницы Плети: жуткие валькирии, крылатые вестницы Короля-лича!','6','0','0','Only by working together will you overcome the final challenge. From the depths of Icecrown come two of the Scourges most powerful lieutenants: fearsome valkyr, winged harbingers of the Lich King!'),
('34996','16037','-1713537','Пусть состязания начнутся!','6','0','0','Let the games begin!'),
('34497','0','-1713538','%s начинает читать заклинание|cFFFFFFFFСветлая воронка!|r Переключение к |cFFFFFFFFСветлой|r сущности!','3','0','0','%S begins to read a spell |cFFFFFFFFSvetlaya funnel!|R switch to |cFFFFFFFFSvetloy|r essence!'),
('34497','0','-1713539','%s начинает читать заклинание Договор близнецов!','3','0','0','%S begins to read the spell Treaty twins!'),
('34496','0','-1713540','%s начинает читать заклинание |cFF9932CDТемная воронка!|r Переключение к |cFF9932CDТемной|r сущности!','3','0','0','%S begins to read a spell |cFF9932CDTemnaya funnel!|R switch to |cFF9932CDTemnoy|r essence!'),
('34497','16272','-1713541','Во имя темного повелителя. Во имя Короля-лича. Вы. Умрете.','6','0','0','In the name of our dark master. For the Lich King. You. Will. Die.'),
('34496','16272','-1713741','Во имя светлого повелителя. Во имя Короля-лича. Вы. Умрете.','6','0','0','In the name of our dark master. For the Lich King. You. Will. Die.'),
('34496','16279','-1713542','Да поглотит вас Свет!','6','0','0','Let the light consume you!'),
('34496','16277','-1713543','Пустое место!','6','0','0','Empty place!'),
('34497','16276','-1713544','Тебя оценили и признали ничтожеством.','6','0','0','You appreciated and acknowledged nothing.'),
('34497','16274','-1713545','ХАОС!','3','0','0','CHAOS!'),
('34496','16278','-1713546','Да поглотит вас Тьма!','6','0','0','Let the dark consume you!'),
('34496','16275','-1713547','Плеть не остановить...','6','0','0','The Scourge cannot be stopped...'),
('34990','16068','-1713548','Против Альянса не выстоять даже самым сильным прислужникам Короля-лича! Все славьте наших героев!','6','0','0','Against the Alliance can not stand even the most powerful henchmen of the Lich King! All glorify our heroes!'),
('34995','16025','-1713748','Ты все еще сомневаешься в могуществе Орды, паладин? Мы примем любой вызов!','6','0','0','Against the Horde does not withstand even the most powerful henchmen of the Lich King! All glorify our heroes!'),
('34996','16051','-1713549','Король-лич понес тяжелую потерю! Вы проявили себя как бесстрашные герои Серебряного Авангарда! Мы вместе нанесем удар по Цитадели Ледяной Короны и разнесем в клочья остатки Плети! Нет такого испытания, которое мы бы не могли пройти сообща!','6','0','0','A mighty blow has been dealt to the Lich King! You have proven yourselves able bodied champions of the Argent Crusade. Together we will strike at Icecrown Citadel and destroy what remains of the Scourge! There is no challenge that we cannot face united!'),
('16980','16321','-1713550','Будет тебе такое испытание, Фордринг.','6','0','0','You will have your challenge, Fordring.'),
('34996','16052','-1713551','Артас! Нас гораздо больше! Сложи Ледяную Скорбь, и я подарю тебе заслуженную смерть.','6','0','0','Arthas! You are hopelessly outnumbered! Lay down Frostmourne and I will grant you a just death.'),
('35877','16322','-1713552','Нерубианцы воздвигли целую империю под льдами Нордскола. Империю, на которой вы так бездумно построили свои дома. МОЮ ИМПЕРИЮ.','6','0','0','The Nerubians built an empire beneath the frozen wastes of Northrend. An empire that you so foolishly built your structures upon. MY EMPIRE.'),
('16980','16323','-1713553','Души твоих павших чемпионов будут принадлежать мне, Фордринг.','6','0','0','The souls of your fallen champions will be mine, Fordring.'),
('34564','16235','-1713554','А вот и гости заявились, как и обещал господин.','6','0','0','Ahhh... Our guests arrived, just as the master promised.'),
('34564','16234','-1713555','Это место станет вашей могилой!','3','0','0','This place will serve as your tomb!'),
('34564','16240','-1713556','Ауум на-л ак-к-к-к, ишшш. Вставайте, слуги мои. Время пожирать...','6','0','0','Auum na-l ak-k-k-k, isshhh. Rise, minions. Devour...'),
('34564','0','-1713557','%s зарывается в землю!','3','0','0','%S buries itself in the earth!'),
('34660','0','-1713558','Шипы %s преследуют $n!','3','0','0','%s spikes  pursuing $n!'),
('34564','0','-1713559','%s вылезает на поверхность!','3','0','0','%S getting out of the ground!'),
('34564','16241','-1713560','Стая поглотит вас!','6','0','0','The swarm shall overtake you!'),
('34564','0','-1713561','%s выпускает рой жуков-трупоедов, чтобы восстановить здоровье!','3','0','0','%S produces a swarm of beetles Peon to restore your health!'),
('34564','16236','-1713562','Ф-лаккх шир!','6','0','0','F-lakkh shir!'),
('34564','16237','-1713563','Еще одна душа накормит хозяина.','6','0','0','Another soul to sate the host.'),
('34564','16238','-1713564','Я подвел тебя, господин...','6','0','0','I have failed you, master...'),
('36095','0','-1713565','Чемпионы, вы прошли испытание великого крестоносца! Знайте, что только самые сильные искатели приключений могли рассчитывать завершить это испытание.','6','0','0','Champions, you are alive! Not only have you defeated every challenge of the Trial of the Crusader, but thwarted Arthas directly! Your skill and cunning will prove to be a powerful weapon against the Scourge. Well done! Allow one of my mages to transport you back to the surface!'),
('36095','0','-1713566','Позвольте вручить вам эти сундуки в качестве заслуженной награды, и пусть его содержимое послужит вам верой и правдой в походе против Артаса в самом центре Цитадели Ледяной Короны!','6','0','0','Let me hand you the chests as a reward, and let its contents will serve you faithfully in the campaign against Arthas in the heart of the Icecrown Citadel!');
