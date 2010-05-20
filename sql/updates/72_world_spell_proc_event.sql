DELETE FROM `spell_proc_event` WHERE `entry` IN (70799,70808,70817,70723,70805,70803,70854);
INSERT INTO `spell_proc_event`
(`entry`,`SchoolMask`,`SpellFamilyName`,`SpellFamilyMask0`,`SpellFamilyMask1`,`SpellFamilyMask2`,`procFlags`,`procEx`,`ppmRate`,`CustomChance`,`Cooldown`) VALUES
(70799,0,6,0x00000800,0,0,0,0,0,0,0), -- Priest T10 Healer 4P Bonus
(70808,0,11,0x00000100,0,0,0,2,0,0,0), -- Shaman T10 Restoration 4P Bonus
(70817,0,11,0,0x00001000,0,0x00010000,0,0,0,0), -- Shaman T10 Elemental 4P Bonus
(70723,0,7,0x00000005,0,0,0,2,0,0,0), -- Druid T10 Balance 4P Bonus
(70805,0,8,0,0x00020000,0,0x00004000,0,0,0,0), -- Rogue T10 2P Bonus
(70803,0,8,4063232,0,0,0,0,0,0,0), -- Rogue T10 4P Bonus
(70854,0,5,0,0x00000010,0,0,0,0,0,0), -- Warrior T10 Melee 2P Bonus
(70841,0,5,0x00000004,0x00000100,0,0,0,0,0,0); -- Warlock T10 4P Bonus