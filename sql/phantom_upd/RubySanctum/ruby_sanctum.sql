-- Game Objects
SET @GUID = 750000;
INSERT INTO `gameobject` VALUES
(@GUID+1, 202347, 724, 15, 1, 3286.8, 533.392, 98.5718, 0, 0, 0, 0, 0, 100, 100, 1),
(@GUID+2, 202348, 724, 15, 1, 3286.8, 533.392, 98.5718, 0, 0, 0, 0, 0, 100, 100, 1),
(@GUID+3, 202349, 724, 15, 1, 3286.8, 533.392, 98.5718, 0, 0, 0, 0, 0, 100, 100, 1),
(@GUID+4, 202350, 724, 15, 1, 3286.8, 533.392, 98.5718, 0, 0, 0, 0, 0, 100, 100, 1),
(@GUID+5, 202347, 571, 15, 1, 3608.19, 186.17, -100.0, 2.1, 0, 0, 0, 0, 100, 100, 1),
(@GUID+6, 202348, 571, 15, 1, 3608.19, 186.17, -100.0, 2.1, 0, 0, 0, 0, 100, 100, 1),
(@GUID+7, 202349, 571, 15, 1, 3608.19, 186.17, -100.0, 2.1, 0, 0, 0, 0, 100, 100, 1),
(@GUID+8, 202350, 571, 15, 1, 3608.19, 186.17, -100.0, 2.1, 0, 0, 0, 0, 100, 100, 1);

DELETE FROM `gameobject_template` WHERE `entry` IN (202794, 203003,203004,203005,203006,203007,203034,203035,203036,203037,203079,203624,203080,203959,203960,203961,203962,204051,204052,204053,204054);
INSERT INTO `gameobject_template`
   (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`)
VALUES
   (202794, 10, 1327, 'Twilight Portal', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75074, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0),
   (203003, 10, 7398, 'Red Dragon Egg', '', '', '', 0, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.5, 0, 0, 0, 0, 0, 0),
   (203004, 10, 9483, 'Broken Red Dragon Egg', '', '', '',0, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.5, 0, 0, 0, 0, 0, 0),
   (203005, 0, 9484, 'Fire Field', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0),
   (203006, 0, 9485, 'Flame Walls', '', '', '', 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0),
   (203007, 0, 9482, 'Ruby Sanctum Halion Flame Ring', '', '', '', 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.6, 0, 0, 0, 0, 0, 0),
   (203034, 1, 9490, 'Burning Tree', '', '', '',  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.2, 0, 0, 0, 0, 0, 0),
   (203035, 1, 9490, 'Burning Tree', '', '', '',  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.2, 0, 0, 0, 0, 0, 0),
   (203036, 1, 9490, 'Burning Tree', '', '', '',  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.2, 0, 0, 0, 0, 0, 0),
   (203037, 1, 9490, 'Burning Tree', '', '', '',  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.2, 0, 0, 0, 0, 0, 0),
   (203079, 10, 7398, 'Red Dragon Egg (Large)', '', '', '',  0, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.75, 0, 0, 0, 0, 0, 0),
   (203080, 10, 9483, 'Broken Red Dragon Egg (Large)', '', '', '',  0, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.75, 0, 0, 0, 0, 0, 0),
   (203624, 0, 9624, 'Halion Twilight Ring', '', '', '',  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.6, 0, 0, 0, 0, 0, 0),
   (203959, 31, 9041, 'Doodad_InstancePortal_Green_10Man01', '', '', '', 724, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.73945, 0, 0, 0, 0, 0, 0),
   (203960, 31, 9040, 'Doodad_InstancePortal_Green_10Man_Heroic01', '', '', '',  724, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.73945, 0, 0, 0, 0, 0, 0),
   (203961, 31, 9042, 'Doodad_InstancePortal_Green_25Man01', '', '', '',  724, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.73945, 0, 0, 0, 0, 0, 0),
   (203962, 31, 9043, 'Doodad_InstancePortal_Green_25Man_Heroic01', '', '', '',  724, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.73945, 0, 0, 0, 0, 0, 0),
   (204051, 31, 9041, 'Doodad_InstancePortal_Green_10Man01', '', '', '',  724, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.59985, 0, 0, 0, 0, 0, 0),
   (204052, 31, 9040, 'Doodad_InstancePortal_Green_10Man_Heroic01', '', '', '',  724, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.59985, 0, 0, 0, 0, 0, 0),
   (204053, 31, 9042, 'Doodad_InstancePortal_Green_25Man01', '', '', '', 724, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.59985, 0, 0, 0, 0, 0, 0),
   (204054, 31, 9043, 'Doodad_InstancePortal_Green_25Man_Heroic01', '', '','', 724, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.59985, 0, 0, 0, 0, 0, 0);

-- Instance Entry 
DELETE FROM `instance_template` WHERE `map` = 724; 
INSERT INTO `instance_template` VALUES (724, 0, 5690, 3273, 534, 89, NULL, 'instance_ruby_sanctum', 1);
INSERT INTO `access_requirement` VALUES
(5690, 80, 80, 0, 0, 0, 0, 0, 0, '', 0, '', 'Ruby Sactum', 15);
DELETE FROM areatrigger_teleport WHERE id IN (5869,5872);
INSERT INTO areatrigger_teleport VALUES
(5869, 'Kammer der Aspekte, Rubin Sanktum(Eingang)', 5690, 724, 3267.97, 533.6, 87.65, 3.047),
(5872, 'Kammer der Aspekte, Rubin Sanktum(Ausgang)', 0, 571, 3597.65, 201.19, -115.05, 0);

-- Quests
DELETE FROM `quest_template` WHERE `entry` IN (26012,26013,26034);
INSERT INTO quest_template
   (`entry`, `Method`, `QuestLevel`, `MinLevel`, `ZoneOrSort`, `Type`, `SuggestedPlayers`, `RepObjectiveFaction`, `RepObjectiveValue`, `RepObjectiveFaction2`, `RepObjectiveValue2`, `NextQuestInChain`, `RewXPId`, `RewOrReqMoney`, `RewMoneyMaxLevel`, `RewSpell`, `RewSpellCast`, `RewHonorAddition`, `RewHonorMultiplier`, `SrcItemId`, `QuestFlags`, `CharTitleId`, `PlayersSlain`, `BonusTalents`, `RewardArenaPoints`, `unk0`, `RewItemId1`, `RewItemCount1`, `RewItemId2`, `RewItemCount2`, `RewItemId3`, `RewItemCount3`, `RewItemId4`, `RewItemCount4`, `RewChoiceItemId1`, `RewChoiceItemCount1`, `RewChoiceItemId2`, `RewChoiceItemCount2`, `RewChoiceItemId3`, `RewChoiceItemCount3`, `RewChoiceItemId4`, `RewChoiceItemCount4`, `RewChoiceItemId5`, `RewChoiceItemCount5`, `RewChoiceItemId6`, `RewChoiceItemCount6`, `RewRepFaction1`, `RewRepFaction2`, `RewRepFaction3`, `RewRepFaction4`, `RewRepFaction5`, `RewRepValueId1`, `RewRepValueId2`, `RewRepValueId3`, `RewRepValueId4`, `RewRepValueId5`, `RewRepValue1`, `RewRepValue2`, `RewRepValue3`, `RewRepValue4`, `RewRepValue5`, `PointMapId`, `PointX`, `PointY`, `PointOpt`, `Title`, `Objectives`, `Details`, `EndText`, `CompletedText`, `ReqCreatureOrGOId1`, `ReqCreatureOrGOCount1`, `ReqSourceId1`, `ReqSourceCount1`, `ReqCreatureOrGOId2`, `ReqCreatureOrGOCount2`, `ReqSourceId2`, `ReqSourceCount2`, `ReqCreatureOrGOId3`, `ReqCreatureOrGOCount3`, `ReqSourceId3`, `ReqSourceCount3`, `ReqCreatureOrGOId4`, `ReqCreatureOrGOCount4`, `ReqSourceId4`, `ReqSourceCount4`, `ReqItemId1`, `ReqItemCount1`, `ReqItemId2`, `ReqItemCount2`, `ReqItemId3`, `ReqItemCount3`, `ReqItemId4`, `ReqItemCount4`, `ReqItemId5`, `ReqItemCount5`, `ReqItemId6`, `ReqItemCount6`, `ObjectiveText1`, `ObjectiveText2`, `ObjectiveText3`, `ObjectiveText4`)
VALUES
   (26012, 2, 82, 80, 4987, 62, 0, 0, 0, 0, 0, 26013, 1, 8000, 13500, 0, 0, 0, 0, 0, 136, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Trouble at Wyrmrest', 'Speak with Krasus at Wyrmrest Temple in Dragonblight.', 'My old friend Krasus sends word of dire happenings at Wyrmrest, in Dragonblight. His missive was brief, but it seems that there has been some sort of attack on the Ruby Sanctum beneath the Temple.$B$BPressing business here in Dalaran keeps me from attending to the matter personally, but I have heard word of your exploits in the Lich King\'s Citadel and you are doubtless up to the task -- perhaps you could assist Krasus in my stead?', '', 'Speak with Krasus at Wyrmrest Temple in Dragonblight.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', '', ''),

   (26013, 2, 82, 80, 4987, 62, 0, 0, 0, 0, 0, 26034, 5, 80000, 135000, 0, 0, 0, 0, 0, 136, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Assault on the Sanctum', 'Investigate the Ruby Sanctum beneath Wyrmrest Temple.', '$R, something terrible has transpired within the Ruby Sanctum beneath the Temple.$B$BThe entry to the Sanctum is in ruins, and its guardians violently slain. We sent scouts to investigate, but they have not returned. I cannot help but fear the worst.$B$BEnter the Ruby Sanctum and discover what has befallen the home of my flight. Once we have more information, we can plan our next steps accordingly.', '', 'Investigate the Ruby Sanctum beneath Wyrmrest Temple.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', '', ''),
   (26034, 2, 82, 80, 4987, 62, 0, 0, 0, 0, 0, 0, 7, 240000, 202500, 0, 0, 0, 0, 0, 200, 0, 0, 0, 0, 0, 49426, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'The Twilight Destroyer', 'Defeat Halion and repel the invasion of the Ruby Sanctum.', 'This was no reckless attack, but rather a carefully orchestrated strike against the heart of the Red Dragonflight.$B$BZarithrian, on the bluff over there, is overseeing the assault, but the true leader of this force is a brash and powerful Twilight dragon named Halion. Not since Dargonax has a full-grown spawn of Sinestra\'s twisted experiments been seen.$B$BSlay him, $N, and then when the invasion has been fully repelled, report to Krasus regarding all you have seen here.', '', 'Report back to Krasus atop Wyrmrest Temple.', 39863, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', '', '');

-- Scripts
REPLACE INTO script_texts (`entry`, `content_default`, `npc_entry`, `content_loc3`, `sound`, `type`, `language`) VALUES
('-1752008', '', '0', 'Help I\'m caught in that tree! Help me!', '17490', '1', '7'),	 	
('-1752009', '', '0', 'Thank you', '17491', '1', '7'),	 	
('-1752010', '', '0', 'We believed the Sanctum is well appreciated but this type of attack we were not prepared ...', '17492', '0', '7'),	 	
('-1752011', '', '0', 'The Black Dragonkin appeared from nowhere and attacked us to respond before we could ...', '17493', '0', '7'), 	
('-1752012', '', '0', 'We had no chance ... While my brothers died, I could retire here and the entrance block.', '17494', '0', '7'), 	
('-1752013', '', '0', 'They slaughtered us with cold efficiency ... But their wares goal seemed to be the eggs which are located in the Sanctum.', '17495', '0', '7'),	
('-1752014', '', '0', 'The commander of ground troops is a cruel beast called Zarithrian. But I fear greater powers are at work ...', '17496', '0', '7'),	
('-1752015', '', '0', 'I saw in their attack shortly their true leader ... a terrible twilight adult dragon.', '17497', '0', '7'),	
('-1752016', '', '0', 'I know their exact plans but I know one thing: They may not be successful!', '17498', '0', '7'),

( -1752001, "Ah, the entertainment has arrived.", 0, "Ahh...Die Unterhaltung ist eingetroffen!", 17520, 1, 7),
( -1752002, "Baltharus leaves nobody alive!", 0, "Baltharus lasst niemanden am Leben!", 17521, 1, 7),
( -1752003, "This world has enough heroes.", 0, "Diese Welt hat genug Helden!", 17522, 1, 7),
( -1752004, "I..don't saw...that coming...", 0, "Das...habe ich...nicht..kommen sehen...", 17523, 1, 7),
( -1752005, "Twice the pain and half the fun.", 0, "Doppelter Schmerz, Halb so viel Spa?!", 17524, 1, 7),
( -1752006, "", 0, "Eure Macht schwindet, Uralte! Balt folgt ihr euren Freunden!", 17525, 1, 7),

( -1752017, "Alexstrasza has chosen capable allies... A pity that I must END YOU!", 0, "Alextrasza hat fahige Verbundete... Zu Schade, dass es hier mit euch ENDET!", 17512, 1, 7),
( -1752018, "You thought you stood a chance?", 0, "Ihr dachtet ihr konntet bestehen?", 17513, 1, 7),
( -1752019, "It's for the best.", 0, "Es ist zu eurem Besten.", 17514, 1, 7),
( -1752020, "HALION! I...", 0, "HALION! Ich...", 17515, 1, 7),
( -1752021, "Turn them to ashes, minions!", 0, "Aschert sie ein, Lakeien!", 17516, 1, 7),

( -1752022, "You will sssuffer for this intrusion!", 0, "Fur dieses Eindringen werdet ihr leiden!", 17528, 1, 7),
( -1752023, "As it should be...", 0, "Wie es sich gehort...", 17529, 1, 7),
( -1752024, "Halion will be pleased", 0, "Halion wird erfreut sein...", 17530, 1, 7),
( -1752025, "HHrr...Grr..", 0, "HHrr...Grr..", 17531, 1, 7),
( -1752026, "Burn in the master's flame!", 0, "Brennt in der Flamme des Meisters!", 17532, 1, 7),

('-1752027', '', '0', 'annoying insects! You\'re too late! The Ruby Sanctum is lost ...', '17499', '1', '7'),	
('-1752028', '', '0', 'Your world is in the balance. You all will be witnesses of the beginning of a new age of destruction', '17500', '1', '7'),	 	
('-1752029', '', '0', 'Another hero is gone!', '17501', '1', '7'),	 	
('-1752030', '', '0', 'Hrrhahaha!', '17502', '1', '7'),	 	
('-1752031', '', '0', 'Enjoy your victory, mortals for it was your Last! On the return of the master will burn this world ...', '17499', '1', '7'),
('-1752032', '', '0', 'Not good enough.', '17504', '1', '7'),
('-1752033', '', '0', 'Burn The Sky!', '17505', '1', '7'),
('-1752033', '', '0', 'Beware the shadows!', '17506', '1', '7'),
('-1752033', '', '0', 'You will find only in the realm of twilight suffering. Come in if you dare!', '17507', '1', '7'),
('-1752033', '', '0', 'I am the light and darkness. Tremble mortals before the herald Deathwing!', '17508', '1', '7');

-- Scriptnames
UPDATE `creature_template` SET `ScriptName` = 'boss_baltharus' WHERE `entry` = '39751';
UPDATE `creature_template` SET `ScriptName` = 'boss_baltharus_clone' WHERE `entry` = '39899';
UPDATE `creature_template` SET `ScriptName` = 'npc_xerestrasza' WHERE `entry` = '40429';
UPDATE `creature_template` SET `ScriptName` = 'boss_zarithrian' WHERE `entry` = '39746';
UPDATE `creature_template` SET `ScriptName` = 'boss_ragefire' WHERE `entry` = '39747';
UPDATE `creature_template` SET `ScriptName` = 'boss_halion' WHERE `entry`= '39863';
UPDATE `creature_template` SET `ScriptName` = 'boss_twilight_halion' WHERE `entry` = '40142';
UPDATE `creature_template` SET `ScriptName` = 'npc_onyx_flamecaller' WHERE `entry` = '39814';
UPDATE `gameobject_template` SET `ScriptName` = 'go_firefield' WHERE `entry` = '203005';