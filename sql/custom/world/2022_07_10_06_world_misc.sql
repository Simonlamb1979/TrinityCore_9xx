DELETE FROM `player_factionchange_achievement` WHERE `alliance_id` = 1466 AND `horde_id` = 926;
DELETE FROM `player_factionchange_achievement` WHERE `alliance_id` = 58 AND `horde_id` = 593;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 233251 WHERE `garrPlotInstanceId` = 18;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 232652 WHERE `garrPlotInstanceId` = 19;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 236261 WHERE `garrPlotInstanceId` = 20;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 233250 WHERE `garrPlotInstanceId` = 22;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 232651 WHERE `garrPlotInstanceId` = 23;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 236262 WHERE `garrPlotInstanceId` = 24;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 236263 WHERE `garrPlotInstanceId` = 25;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 236191 WHERE `garrPlotInstanceId` = 59;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 236190 WHERE `garrPlotInstanceId` = 63;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 236192 WHERE `garrPlotInstanceId` = 67;
UPDATE `garrison_plot_finalize_info` SET `allianceGameObjectId` = 236193 WHERE `garrPlotInstanceId` = 81;
DELETE FROM `spell_scripts` WHERE `id` IN (66477, 71874, 53341);
DELETE FROM `event_scripts` WHERE `id` = 8420 AND `delay` = 750 AND `command` = 9 AND `datalong` = 99784;
DELETE FROM `event_scripts` WHERE `id` = 525 AND `command` = 9 AND `datalong` = 825;
DELETE FROM `event_scripts` WHERE `id` = 525 AND `command` = 9 AND `datalong` = 8194;
DELETE FROM `event_scripts` WHERE `id` = 525 AND `command` = 9 AND `datalong` =8195;
DELETE FROM `event_scripts` WHERE `id` = 525 AND `command` = 9 AND `datalong` =8196;
DELETE FROM `event_scripts` WHERE `id` = 525 AND `command` = 9 AND `datalong` =8197;
DELETE FROM `event_scripts` WHERE `id` = 9542 AND `command` = 9 AND `datalong` =8987;
DELETE FROM `event_scripts` WHERE `id` = 9542 AND `command` = 9 AND `datalong` =8988;
DELETE FROM `event_scripts` WHERE `id` = 9542 AND `command` = 9 AND `datalong` =8989;
DELETE FROM `event_scripts` WHERE `id` = 9542 AND `command` = 9 AND `datalong` =8990;
DELETE FROM `event_scripts` WHERE `id` = 9542 AND `command` = 9 AND `datalong` =8991;
DELETE FROM `event_scripts` WHERE `id` = 9542 AND `command` = 9 AND `datalong` =9002;
DELETE FROM `event_scripts` WHERE `id` = 18481 AND `delay` = 1 AND `command` = 15 AND `datalong` = 39983;
DELETE FROM `event_scripts` WHERE `id` IN (11286, 11289, 11290, 11291) AND `command` = 10 AND `datalong` = 17841 AND `datalong2` = 100;
DELETE FROM `event_scripts` WHERE `id` = 13852 AND `command` = 10 AND `datalong` = 21633 AND `datalong2` = 120000;
DELETE FROM `waypoint_scripts` WHERE `command` IN (2, 4, 5);

UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=21599;
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=30911;
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=39680;
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=39699;
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=39701;
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=39706;
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=48943;
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=48944;

DELETE FROM `spell_pet_auras` WHERE `spell` = 20895 AND `effectId` = 0 AND `pet` = 0;
-- UPDATE `lfg_dungeon_rewards` SET `maxLevel` = 45 WHERE `maxLevel` > 45;

DELETE FROM `spell_target_position` WHERE `ID` IN (18960, 65486, 68630, 71241, 85263, 85264, 85265, 85266, 103120, 203802) AND `EffectIndex` = 0;
DELETE FROM `spell_target_position` WHERE `ID` = 68639 AND `EffectIndex` = 2;
UPDATE `spell_target_position` SET `EffectIndex` = 1 WHERE `ID` = 53822 AND `EffectIndex` = 0;
UPDATE `spell_target_position` SET `MapID` = 870 WHERE `ID` = 118953 AND `EffectIndex` = 0;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = -93342 AND `spell_effect` = 71037 AND `type` = 0;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = -70923 AND `spell_effect` = -73015 AND `type` = 0;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = -59907 AND `spell_effect` = 7 AND `type` = 0;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = -47960 AND `spell_effect` = -63311 AND `type` = 0;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 17767 AND `spell_effect` = 54501 AND `type` = 2;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 41635 AND `spell_effect` = 41637 AND `type` = 0;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 42965 AND `spell_effect` = 42919 AND `type` = 0;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 42966 AND `spell_effect` = -42965 AND `type` = 0;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 47585 AND `spell_effect` = 63230 AND `type` = 2;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 51010 AND `spell_effect` = -5268 AND `type` = 0;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 51010 AND `spell_effect` = 5268 AND `type` = 2;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 77767 AND `spell_effect` = 91954 AND `type` = 0;

DELETE FROM `playerchoice_response` WHERE `ChoiceId` = 588 AND `ResponseId` = 0;
DELETE FROM `playerchoice_response` WHERE `ChoiceId` = 588 AND `ResponseId` = 1869836137;
DELETE FROM `playerchoice_response_reward` WHERE `ChoiceId` = 667;
DELETE FROM `playerchoice_response_reward` WHERE `ChoiceId` = 342;
DELETE FROM `playerchoice_response_reward` WHERE `ChoiceId` = 276 AND `ResponseId` = 608;
DELETE FROM `playerchoice_response_reward` WHERE `ChoiceId` = 291 AND `ResponseId` = 655;
DELETE FROM `item_loot_template` WHERE `Reference` = 0 AND `item` IN (5000, 43237);
DELETE FROM `item_loot_template` WHERE `Entry` IN (54218);
UPDATE `item_loot_template` SET `Chance` = 30 WHERE `Entry` = 11024 AND `Item` = 11040;
UPDATE `access_requirement` SET `difficulty` = 33 WHERE `mapId` = 564 AND `difficulty` = 4;
UPDATE `access_requirement` SET `difficulty` = 14 WHERE `mapId` = 603 AND `difficulty` = 3;
UPDATE `access_requirement` SET `difficulty` = 33 WHERE `mapId` = 603 AND `difficulty` = 4;
UPDATE `access_requirement` SET `difficulty` = 14 WHERE `mapId` = 720 AND `difficulty` = 3;
UPDATE `access_requirement` SET `difficulty` = 15 WHERE `mapId` = 720 AND `difficulty` = 4;
UPDATE `access_requirement` SET `difficulty` = 33 WHERE `mapId` = 720 AND `difficulty` = 5;
DELETE FROM `access_requirement` WHERE `mapId` = 720 AND `difficulty` = 6;




