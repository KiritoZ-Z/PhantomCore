-- Fix damage Mirror Image (Frostbolt,Fireblast)
DELETE FROM spell_bonus_data WHERE entry IN (59638, 59637);
INSERT INTO spell_bonus_data VALUES
(59638, 0.3, -1, -1, -1, 'Mage - Mirror Image Frostbolt'),
(59637, 0.15, -1, -1, -1, 'Mage - Mirror Image Fire blast');
-- Fix damage Shield of Righteousness
DELETE FROM spell_bonus_data WHERE entry = 53600;
INSERT INTO spell_bonus_data VALUES
(53600, 0, 0, 0, 0, 'Paladin - Shield Of Righteousness');