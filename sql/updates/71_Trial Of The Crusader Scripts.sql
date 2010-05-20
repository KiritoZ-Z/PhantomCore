UPDATE `instance_template` SET `script`='instance_trial_of_the_crussader' WHERE (`map`='649');

UPDATE `creature_template` SET `ScriptName`='npc_tcrus_announcer' WHERE (`entry`='34816');
UPDATE `creature_template` SET `ScriptName`='npc_tcrus_tirion' WHERE (`entry`='34996');
UPDATE `creature_template` SET `ScriptName`='npc_tcrus_fizzlebang' WHERE (`entry`='35458');
UPDATE `creature_template` SET `ScriptName`='npc_Garrosh' WHERE (`entry`='34995');
UPDATE `creature_template` SET `ScriptName`='npc_KingVyrn' WHERE (`entry`='34990');
UPDATE `creature_template` SET `ScriptName`='npc_tcrus_tirion' WHERE (`entry`='34996');
UPDATE `creature_template` SET `ScriptName`='npc_LichKing' WHERE (`entry`='35877');

-- Faction Champions Horde
UPDATE `creature_template` SET `ScriptName`='boss_gorgrim_shadowcleave' WHERE (`entry`='34458');
UPDATE `creature_template` SET `ScriptName`='boss_birana_stormhoof' WHERE (`entry`='34451');
UPDATE `creature_template` SET `ScriptName`='boss_erin_misthoof' WHERE (`entry`='34459');
UPDATE `creature_template` SET `ScriptName`='boss_rujkah' WHERE (`entry`='34448');
UPDATE `creature_template` SET `ScriptName`='boss_ginselle_blightslinger' WHERE (`entry`='34449');
UPDATE `creature_template` SET `ScriptName`='boss_liandra_suncaller' WHERE (`entry`='34445');
UPDATE `creature_template` SET `ScriptName`='boss_malithas_brightblader' WHERE (`entry`='34456');
UPDATE `creature_template` SET `ScriptName`='boss_caiphus_the_stern' WHERE (`entry`='34447');
UPDATE `creature_template` SET `ScriptName`='boss_vivienne_blackwhisper' WHERE (`entry`='34441');
UPDATE `creature_template` SET `ScriptName`='boss_mazdinah' WHERE (`entry`='34454');
UPDATE `creature_template` SET `ScriptName`='boss_broln_stouthorn' WHERE (`entry`='34455');
UPDATE `creature_template` SET `ScriptName`='boss_thrakgar' WHERE (`entry`='34444');
UPDATE `creature_template` SET `ScriptName`='boss_harkzog' WHERE (`entry`='34450');
UPDATE `creature_template` SET `ScriptName`='boss_narrhok_steelbreaker' WHERE (`entry`='34453');

-- Faction Champions Alliance
UPDATE `creature_template` SET `ScriptName`='boss_tyrius_duskblade' WHERE (`entry`='34461');
UPDATE `creature_template` SET `ScriptName`='boss_kavina_grovesong' WHERE (`entry`='34460');
UPDATE `creature_template` SET `ScriptName`='boss_melador_valestrider' WHERE (`entry`='34469');
UPDATE `creature_template` SET `ScriptName`='boss_alyssia_moonstalker' WHERE (`entry`='34467');
UPDATE `creature_template` SET `ScriptName`='boss_noozle_whizzlestick' WHERE (`entry`='34468');
UPDATE `creature_template` SET `ScriptName`='boss_velanaa' WHERE (`entry`='34465');
UPDATE `creature_template` SET `ScriptName`='boss_baelnor_lightbearer' WHERE (`entry`='34471');
UPDATE `creature_template` SET `ScriptName`='boss_anthar_forgemender' WHERE (`entry`='34466');
UPDATE `creature_template` SET `ScriptName`='boss_brienna_nightfell' WHERE (`entry`='34473');
UPDATE `creature_template` SET `ScriptName`='boss_irieth_shadowstepe' WHERE (`entry`='34472');
UPDATE `creature_template` SET `ScriptName`='boss_shaabad' WHERE (`entry`='34463');
UPDATE `creature_template` SET `ScriptName`='boss_saamul' WHERE (`entry`='34470');
UPDATE `creature_template` SET `ScriptName`='boss_serissa_grimdabbler' WHERE (`entry`='34474');
UPDATE `creature_template` SET `ScriptName`='boss_shocuul' WHERE (`entry`='34475');

-- Spell DBC
INSERT INTO `spelldifficulty_dbc` VALUES ('6000', '66331', '67478', '67477', '67479'); -- Gormok Impale
INSERT INTO `spelldifficulty_dbc` VALUES ('6001', '66330', '67647', '67648', '67649'); -- Gormok Staggering Stomp
INSERT INTO `spelldifficulty_dbc` VALUES ('6002', '66689', '67650', '67651', '67652'); -- Icehowl Artic Breath
INSERT INTO `spelldifficulty_dbc` VALUES ('6003', '66683', '67660', '67660', '67662'); -- Icehowl Massic Crash
INSERT INTO `spelldifficulty_dbc` VALUES ('6004', '67345', '67663', '67663', '67665'); -- Icehowl Whirl
INSERT INTO `spelldifficulty_dbc` VALUES ('6005', '66770', '67655', '67654', '67656'); -- Icehowl Ferocious Butt
INSERT INTO `spelldifficulty_dbc` VALUES ('6006', '66879', '67624', '67625', '67626'); -- Dreadscale Burning Bite
INSERT INTO `spelldifficulty_dbc` VALUES ('6007', '66820', '67636', '67635', '67637'); -- Dreadscale Molten Spew
INSERT INTO `spelldifficulty_dbc` VALUES ('6020', '66237', '67049', '67050', '67051'); -- Jaraxxus Incinerate Flesh
INSERT INTO `spelldifficulty_dbc` VALUES ('6021', '66528', '67029', '67030', '67031'); -- Jaraxxus Fel Lighting
INSERT INTO `spelldifficulty_dbc` VALUES ('6022', '68124', '68126', '68127', '68128'); -- Jaraxxus Legion Flame
INSERT INTO `spelldifficulty_dbc` VALUES ('6023', '66532', '66963', '66964', '66965'); -- Jaraxxus Fel Fireball
INSERT INTO `spelldifficulty_dbc` VALUES ('6024', '66258', '66258', '67903', '67903'); -- Jaraxxus Summon Volcan
INSERT INTO `spelldifficulty_dbc` VALUES ('6025', '67103', '67103', '67104', '67105'); -- Portal Summon Guardian

UPDATE `creature_template` SET `ScriptName`='Boss_Jaraxxus' WHERE (`entry`='34780');
UPDATE `creature_template` SET `ScriptName`='Mob_Vulcan' WHERE `entry`=34813;
UPDATE `creature_template` SET `ScriptName`='Mob_FireTrigger' WHERE `entry`=34784;
UPDATE `creature_template` SET `ScriptName`='Mob_Netherportal' WHERE `entry`=34825;
UPDATE `creature_template` SET `scale`='2' WHERE `entry`=34825;
UPDATE `creature_template` SET `ScriptName`='Mob_MistressOfPain' WHERE `entry`=34826;
INSERT INTO `creature_template` VALUES ('41000', '0', '0', '0', '0', '0', '169', '11686', '169', '0', 'Wilfred Portal Trigger', '', '', '0', '1', '1', '0', '14', '14', '0', '1', '1', '0', '1', '2', '0', '0', '1', '2000', '2000', '1', '0', '0', '0', '0', '0', '0', '0', '1', '2', '0', '10', '1049600', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '0', '3', '0.0238095', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '2', '', '11159');

-- Gormok
UPDATE `creature_template` SET `ScriptName`='boss_gormok_impaler' WHERE (`entry`='34796');
UPDATE `creature_template` SET `scale`='1.5', `faction_A`='16', `faction_H`='16', `ScriptName`='mob_firebomb_trigger' WHERE (`entry`='34854');

-- Acidmaw & Dreadscale
UPDATE `creature_template` SET `ScriptName`='boss_acidmaw' WHERE (`entry`='35144');
UPDATE `creature_template` SET `ScriptName`='boss_dreadscale' WHERE (`entry`='34799');

-- IceHowl
UPDATE `creature_template` SET `ScriptName`='boss_icehowl' WHERE (`entry`='34797');

UPDATE `creature_template` SET `ScriptName`='boss_fjola' WHERE (`entry`='34497');
UPDATE `creature_template` SET `ScriptName`='boss_eydis' WHERE (`entry`='34496');
UPDATE `creature_template` SET `AIName`='', `ScriptName`='mob_valkyr_orb' WHERE `entry` IN (34628, 34630);
UPDATE `creature_template` SET unit_flags= 33554434, faction_A =14, faction_H=14 WHERE `entry` IN (34628, 34630);
INSERT INTO npc_spellclick_spells VALUES (34567, 65684, 0, 0, 0, 1), (34568, 65686, 0, 0, 0, 1); -- dark/light "portals"
INSERT INTO spell_script_target values (65875, 1, 34497), (65876, 1, 34496);

UPDATE `creature_template` SET `ScriptName`='mob_power_of_twin' WHERE (`entry`='34630');
UPDATE `creature_template` SET `ScriptName`='mob_power_of_twin' WHERE (`entry`='34628');

UPDATE `creature_template` SET `ScriptName`='mob_essence_of_twin' WHERE (`entry`='34630');
UPDATE `creature_template` SET `ScriptName`='mob_essence_of_twin' WHERE (`entry`='34628');

UPDATE `creature_template` SET `ScriptName`='Boss_Raid_Anubarak' WHERE (`entry`='34564');
UPDATE `creature_template` SET `ScriptName`='Mob_NerubianTrigger' WHERE (`entry`='34862');

INSERT INTO `script_texts` VALUES ('0', '-1600205', 'Welcome champions, you have heard the call of the Argent Crusade, and you have boldly answered. It is here, in the Crusader\'s Coliseum, that you will face your greatest challenges. Those of you, who survive the rigors of the coliseum will join the Argent Crusade on it\'s march to Icecrown Citadel.', null, null, 'Willkommen, Champions! Ihr habt den Ruf des Argentumkreuzzugs vernommen und seid ihm mutig gefolgt! Hier, im Kolosseum der Kreuzfahrer, werdet Ihr auf Eure größten Herausforderungen treffen.Die jenigen unter Euch, welche die Gefahren des Kolosseums überleben, werden den Kreuzzug bei seinem Marsch auf die Eiskronenzitadelle begleiten.', null, null, null, null, null, '16036', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600206', 'Hailing from the deepest, darkest carverns of the storm peaks, Gormok the Impaler! Battle on, heroes!', null, null, 'Er kommt aus den tiefsten, dunkelsten Höhlen der Sturmgifpel - Gormok der Pfähler! Voran, Helden!', null, null, null, null, null, '16038', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600207', 'Lasst die Spiele beginnen.', '', null, 'Lasst die Spiele beginnen.', null, null, null, null, null, '16037', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600208', 'Im Ring des Blutes habe ich würdigere Kämpfer gesehen. Ihr verschwendet unsere Zeit, Paladin!', null, null, 'Im Ring des Blutes habe ich würdigere Kämpfer gesehen. Ihr verschwendet unsere Zeit, Paladin!', null, null, null, null, null, '16026', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600209', 'Steel yourselves, heroes, for the twin terrors Acidmaw and Dreadscale. Enter the arena!', null, null, 'Stellt euch Helden, den die Zwillingsschrecken Ätschlund und Schreckensmaul erscheinen in der Arena.', null, null, null, null, null, '16039', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600210', 'The air freezes with the introduction of our next combatant, Icehowl! Kill or be killed, champions!', null, null, 'Mit der Ankündigung unseres nächsten Kämpers gefriert die Luft selbst: Eisheuler! Tötet oder werdet getötet, Champions!', null, null, null, null, null, '16040', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600211', 'Grand Warlock Wilfred Fizzlebang will summon forth your next challenge. Stand by for his entry!', null, null, 'Großhexenmeister Wilfred Zischknall wird Eure nächste Herausforderung beschwören. Harrt seiner Ankunft.', null, null, null, null, null, '16043', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600212', 'Thank you, Highlord! Now challengers, I will begin the ritual of summoning! When I am done, a fearsome Doomguard will appear!', null, null, 'Danke. Hochlord. Nun, Champions, werde ich mit dem Beschwörungsritual beginnen. Sobald es beendet ist, wird eine fürchterliche Verdammniswache erscheinen!', null, null, null, null, null, '16268', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600213', 'Prepare for oblivion!', null, null, 'Bereitet euch auf den Untergang vor!', null, null, null, null, null, '16269', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600214', 'Ah ha! Behold the absolute power of Wilfred Fizzlebang, master summoner! You are bound to ME, demon!', null, null, 'A-HA! Ich hab es vollbracht! Bestaunt die gerenzenlose Macht von Wilfred Zischknall, Meisterbeschwörer! Du bist an mich gebunden, Dämon!', null, null, null, null, null, '16270', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600215', 'Trifling gnome, your arrogance will be your undoing!', null, null, 'Unbedeutener Gnom! Deine Arroganz wird dir zum Verhängnis!', null, null, null, null, null, '16143', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600216', 'But I\'m in charge her-', null, null, 'aber ich hab hier die Kontrolle ahh......', null, null, null, null, null, '16271', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600217', 'Quickly, heroes! Destroy the demon lord before it can open a portal to its twisted demonic realm!', null, null, 'Schnell, Helden, Zerstört den Dämonenlord, bevor er ein Portal zu seiner verzerrten Welt öffnen kann!', null, null, null, null, null, '16044', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600218', 'You face Jaraxxus, eredar lord of the Burning Legion!', null, null, null, null, null, null, null, null, '16144', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600219', 'FLESH FROM BONE!', null, null, null, null, null, null, null, null, '16149', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600220', 'Come forth, sister! Your master calls!', null, null, null, null, null, null, null, null, '16150', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600221', 'INFERNO!', null, null, null, null, null, null, null, null, '16151', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600222', 'Insignificant gnat!', null, null, null, null, null, null, null, null, '16145', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600223', 'Banished to the Nether!', null, null, null, null, null, null, null, null, '16146', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600224', 'Another will take my place. Your world is doomed.', null, null, null, null, null, null, null, null, '16147', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600229', 'The next battle will be against the Argent Crusade\'s most powerful knights! Only by defeating them will you be deemed worthy...', null, null, null, null, null, null, null, null, '0', '1', '0', '0', null);
-- 1600229 - 1600233 Todo Texte
INSERT INTO `script_texts` VALUES ('0', '-1600234', 'The next battle will be against the Argent Crusade\'s most powerful knights! Only by defeating them will you be deemed worthy...', null, null, null, null, null, null, null, null, '16047', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600235', 'Our honor has been besmirched! They make wild claims and false accusations against us. I demand justice! Allow my champions to fight in place of your knights, Tirion. We challenge the Horde!', null, null, null, null, null, null, null, null, '16066', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600236', 'Very well, I will allow it. Fight with honor!', null, null, null, null, null, null, null, null, '16048', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600237', 'Fight for the glory of the Alliance, heroes! Honor your king and your people!', null, null, null, null, null, null, null, null, '16065', '1', '0', '0', null);
-- 1600238 Horde Spawn
INSERT INTO `script_texts` VALUES ('0', '-1600239', 'Only by working together will you overcome the final challenge. From the depths of Icecrown come to of the Scourge\'s most powerful lieutenants: fearsome val\'kyr, winged harbingers of the Lich King!', null, null, null, null, null, null, null, null, '0', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600240', 'A mighty blow has been dealt to the Lich King! You have proven yourselves able bodied champions of the Argent Crusade. Together we will strike at Icecrown Citadel and destroy what remains of the Scourge! There is no challenge that we cannot face united!', null, null, 'Dem Lichtkönig wurde ein schwerer Schlag versetzt! Ihr habt Euch als würdige Champions erwiesen. Gemeinsam werden wir den Angriff auf die Eiskronenzitadelle durchführen und den Rest der Geißel zerstören! Gemeinsam meistern wir alles!', null, null, null, null, null, '16051', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600241', 'You will have your challenge, Fordring.', null, null, 'Ihr werdet Eure Herausforderung bekommen, Fordring.', null, null, null, null, null, '16321', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600242', 'Arthas! You are hopelessly outnumbered! Lay down Frostmourne and I will grant you a just death.', null, null, 'Arthas! Wir sind Euch zahlenmäßig überlegen! Legt Frostgram nieder und ich werde Euch einen gerechten Tod gewähren.', null, null, null, null, null, '16052', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600243', 'The Nerubians built an empire beneath the frozen wastes of Northrend. An empire that you so foolishly built your structures upon. MY EMPIRE.', null, null, 'Die Neruber erbauten ein Königreich unter den gefrorenen Einöden Nordens. Ein Königreich, auf dem Ihr so töricht Euren Spielplatz aufgebaut habt, AUF MEINEM KÖNIGREICH.', null, null, null, null, null, '16322', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600244', 'The souls of your fallen champions will be mine, Fordring.', null, null, 'Die Seelen Eurer gefallenen Champions werden mir gehören, Fordering.', null, null, null, null, null, '16323', '1', '0', '0', null);
-- 1600245 Anubarak Intro
INSERT INTO `script_texts` VALUES ('0', '-1600246', 'The loss of Wilfred Fizzlebang, while unfortunate, should be a lesson to those that dare dabble in dark magic. Alas, you are victorious and must now face the next challenge. ', null, null, 'Der Verlust von Wilfred Fizzlebang sollte jeden unter euch eine Lehre sein, die es wagen mit den dunklen künsten herumzuspielen. Jedoch ihr seid Siegreich und ihr müsst nun die nächste Herausforderung antreten.', null, null, null, null, null, '16045', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600247', 'Treacherous Alliance dogs! You summon a demon lord against warriors of the Horde!? Your deaths will be swift! ', null, null, 'Hinterhältige Allianz Hunde! Ihr beschwört einen Dämonenlord gegen die Krieger der Horde! Ihr werdet einen schnellen Tot finden.', null, null, null, null, null, '16021', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600248', 'The Alliance doesn\'t need the help of a demon lord to deal with Horde filth. Come, pig! ', null, null, 'Die Allianz braucht keinen Dämon zur unterstützung gegen Hordendreck! Komm her Schwein!', null, null, null, null, null, '16064', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600249', 'Everyone, calm down! Compose yourselves! There is no conspiracy at play here. The warlock acted on his own volition - outside of influences from the Alliance. The tournament must go on! ', null, null, 'Ihr alle beruhigt euch. Hier ist keine Verschwörung ingange der Hexenmeister handelte aus eigenen willen heraus unabhängig von der Allianz! Das Tunier muss weitergehen.', null, null, null, null, null, '16046', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600250', 'A shallow and tragic victory. We are weaker as a whole from the losses suffered today. Who but the Lich King could benefit from such foolishness? Great warriors have lost their lives. And for what? The true threat looms ahead - the Lich King awaits us all in death.', null, null, null, null, null, null, null, null, '0', '1', '0', '0', null);
-- -1600251 Northrend Beast Wipe
INSERT INTO `script_texts` VALUES ('0', '-1600252', 'In the name of our dark master. For the Lich King. You. Will. Die.', null, null, null, null, null, null, null, null, '16272', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600253', 'CHAOS!', null, null, null, null, null, null, null, null, '16274', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600254', 'The Scourge cannot be stopped...', null, null, null, null, null, null, null, null, '16275', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600255', 'UNWORTHY!', null, null, null, null, null, null, null, null, '16276', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600256', 'You have been measured, and found wanting!', null, null, null, null, null, null, null, null, '16277', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600257', 'Let the dark consume you!', null, null, null, null, null, null, null, null, '16278', '1', '0', '0', null);
INSERT INTO `script_texts` VALUES ('0', '-1600258', 'Let the light consume you!', null, null, null, null, null, null, null, null, '16279', '1', '0', '0', null);
