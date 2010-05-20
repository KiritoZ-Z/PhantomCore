-- Fix cooldown metasocket Invigorating Earthsiege Diamond
UPDATE `spell_proc_event` SET Cooldown = 45 WHERE entry = 61356;
-- Holy fire Dot damage
UPDATE `spell_bonus_data` SET dot_bonus = 0.052 WHERE entry = 14914;
-- Fix faction elemental water (Mage summon)
UPDATE `creature_template` SET unit_flags = 0 WHERE entry = 510;
