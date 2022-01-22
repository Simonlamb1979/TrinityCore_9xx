UPDATE `gameobject_template` SET `displayId` = 3411 WHERE `entry` IN (176567,176568,176569,176570,176571); 
UPDATE `gameobject_template` SET `displayId` = 65816 WHERE `entry` = 175885;
UPDATE `gameobject_template` SET `displayId` = 12840 WHERE `entry` = 185324;
UPDATE `gameobject` SET `PhaseId` = 0 WHERE `guid` = 32360;
DELETE FROM `gameobject` WHERE `guid` = 500063;
UPDATE `gameobject` SET `PhaseId` = 0 WHERE `guid` = 21065;