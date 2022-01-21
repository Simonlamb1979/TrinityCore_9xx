UPDATE `creature_template` SET `faction` = 35 WHERE `faction` = 0;
UPDATE `creature_template` SET `KillCredit1` = 0 WHERE `KillCredit1` IN (134989,125127,127372,173429);
UPDATE `creature_template` SET `KillCredit2` = 0 WHERE `KillCredit2` = 173429;
UPDATE `creature_template` SET `minlevel` = 1, `maxlevel` = 60 WHERE `minlevel` < 1;