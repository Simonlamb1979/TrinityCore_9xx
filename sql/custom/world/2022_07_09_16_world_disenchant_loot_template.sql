DELETE FROM `disenchant_loot_template` WHERE `Reference` = 0 AND `item` IN (10978, 10998, 11082, 11083, 11084, 11134, 11135, 11137, 11138, 11139, 11174, 11175, 11176, 11177, 11178, 20725);
UPDATE `disenchant_loot_template` SET `Chance` = 70 WHERE `Entry` = 8 AND `Item` = 10940;
UPDATE `disenchant_loot_template` SET `Chance` = 72 WHERE `Entry` = 31 AND `Item` = 16202;
UPDATE `disenchant_loot_template` SET `Chance` = 70 WHERE `Entry` = 97 AND `Item` = 52555;
UPDATE `disenchant_loot_template` SET `Chance` = 25 WHERE `Entry` = 98 AND `Item` = 52719;
UPDATE `disenchant_loot_template` SET `Chance` = 68 WHERE `Entry` = 102 AND `Item` = 52719;
UPDATE `disenchant_loot_template` SET `Chance` = 38 WHERE `Entry` = 103 AND `Item` = 52719;
DELETE FROM `disenchant_loot_template` WHERE `Entry` IN (32, 52, 56, 57);
