-- (12311) Gag Order (Rank 1)
DELETE FROM `spell_proc_event` WHERE `entry` IN (12311);
INSERT INTO `spell_proc_event` VALUES (12311, 0x01, 0, 0x00000800, 0x00000001, 0x00000000, 0x00020220, 0x00000207, 0, 50, 0);
-- (12958) Gag Order (Rank 2)
DELETE FROM `spell_proc_event` WHERE `entry` IN (12958);
INSERT INTO `spell_proc_event` VALUES (12958, 0x01, 0, 0x00000800, 0x00000001, 0x00000000, 0x00020220, 0x00000207, 0, 100, 0);
