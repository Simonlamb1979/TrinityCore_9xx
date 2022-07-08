UPDATE `gameobject_template` SET `displayId` = 192 WHERE `entry` IN (176567, 176568, 176569, 176570, 176571, 175885, 185324, 252851);
DELETE FROM `gameobject` WHERE `guid` = 500063;
UPDATE `gameobject` SET `PhaseId` = 0 WHERE `guid` = 21065;