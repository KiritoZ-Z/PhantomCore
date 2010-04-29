UPDATE `creature_template` SET `ScriptName`='boss_fjola' WHERE (`entry`='34497');
UPDATE `creature_template` SET `ScriptName`='boss_eydis' WHERE (`entry`='34496');

UPDATE `creature_template` SET `ScriptName`='mob_power_of_twin' WHERE (`entry`='34630');
UPDATE `creature_template` SET `ScriptName`='mob_power_of_twin' WHERE (`entry`='34628');

UPDATE `creature_template` SET `ScriptName`='mob_essence_of_twin' WHERE (`entry`='34630');
UPDATE `creature_template` SET `ScriptName`='mob_essence_of_twin' WHERE (`entry`='34628');

-- Gormok
UPDATE `creature_template` SET `ScriptName`='boss_gormok_impaler' WHERE (`entry`='34796'); 
UPDATE `creature_template` SET `scale`='1.5', `faction_A`='16', `faction_H`='16', `ScriptName`='mob_firebomb_trigger' WHERE (`entry`='34854');

-- Acidmaw & Dreadscale
UPDATE `creature_template` SET `ScriptName`='boss_acidmaw' WHERE (`entry`='35144'); 
UPDATE `creature_template` SET `ScriptName`='boss_dreadscale' WHERE (`entry`='34799');

-- IceHowl
UPDATE `creature_template` SET `ScriptName`='boss_icehowl' WHERE (`entry`='34797'); 

UPDATE `creature_template` SET `ScriptName`='Boss_Jaraxxus' WHERE (`entry`='34780');
UPDATE `creature_template` SET `ScriptName`='Mob_Vulcan' WHERE `entry`=34813; 
UPDATE `creature_template` SET `ScriptName`='Mob_FireTrigger' WHERE `entry`=34784; 
UPDATE `creature_template` SET `ScriptName`='Mob_Netherportal' WHERE `entry`=34825;  
UPDATE `creature_template` SET `scale`='2' WHERE `entry`=34825;
UPDATE `creature_template` SET `ScriptName`='Mob_MistressOfPain' WHERE `entry`=34826;
INSERT INTO `creature_template` VALUES ('41000', '0', '0', '0', '0', '0', '169', '11686', '169', '0', 'Wilfred Portal Trigger',

UPDATE `instance_template` SET `script`='instance_trial_of_the_crussader' WHERE (`map`='649');

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


-- Faction Champions Allianz
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

UPDATE `creature_template` SET `ScriptName`='npc_tcrus_announcer' WHERE (`entry`='34816'); 
UPDATE `creature_template` SET `ScriptName`='npc_tcrus_tirion' WHERE (`entry`='34996');
UPDATE `creature_template` SET `ScriptName`='npc_tcrus_fizzlebang' WHERE (`entry`='35458');
UPDATE `creature_template` SET `ScriptName`='npc_Garrosh' WHERE (`entry`='34995');
UPDATE `creature_template` SET `ScriptName`='npc_KingVyrn' WHERE (`entry`='34990');
UPDATE `creature_template` SET `ScriptName`='npc_tcrus_tirion' WHERE (`entry`='34996');
UPDATE `creature_template` SET `ScriptName`='npc_LichKing' WHERE (`entry`='35877');





