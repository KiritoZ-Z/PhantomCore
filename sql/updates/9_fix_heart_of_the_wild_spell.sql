#Fix Spell Heart of the Wild
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` in ('-5487', '-9634', '-768');
INSERT INTO `spell_linked_spell` VALUES
(-5487,-24899,0, 'Heart of the Wild removed if not in Bear Form'),
(-9634,-24899,0, 'Heart of the Wild removed if not in Dire Bear Form'),
(-768,-24900,0, 'Heart of the Wild removed if not in Cat Form');

DELETE FROM `spell_dbc` WHERE `id` in ('24899', '24900');
INSERT INTO spell_dbc VALUES
(24899, 0, 0, 400, 1024, 0, 0, 2097152, 0, 0, 1, 0, 0, 101, 0, 0, 0, 0, 21, 1, 0, -1, 0, 0, 6, 0, 0, 1, 0, 0, 0, 0, 0, -1,
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 137, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 'Heart of the Wild Bear Effect'),
(24900, 0, 0, 400, 1024, 0, 0, 2097152, 0, 0, 1, 0, 0, 101, 0, 0, 0, 0, 21, 1,
0, -1, 0, 0, 6, 0, 0, 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
0, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 'Heart of the Wild Cat Effect');