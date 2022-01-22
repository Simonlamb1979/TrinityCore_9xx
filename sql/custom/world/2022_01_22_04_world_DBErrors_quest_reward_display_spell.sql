UPDATE `quest_reward_display_spell` SET `PlayerConditionID` = 0 WHERE `PlayerConditionID` IN (4383,89260,92667); 
DELETE FROM `quest_reward_display_spell` WHERE `QuestID` = 62828 AND `Idx` = 0;