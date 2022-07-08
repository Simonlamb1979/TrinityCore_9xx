INSERT INTO `quest_template_addon` (`ID`)
SELECT `ID` FROM `quest_template` WHERE `id` NOT IN(SELECT `id` FROM `quest_template_addon`);


update `quest_template_addon`,`quest_template`
set `quest_template_addon`.`NextQuestID`=`quest_template`.`RewardNextQuest`
where `quest_template_addon`.`ID`=`quest_template`.`ID` and `quest_template`.`RewardNextQuest`!='0';

CREATE TABLE `quest_template_addon_copy` LIKE `quest_template_addon`; 
INSERT INTO `quest_template_addon_copy` SELECT * FROM `quest_template_addon`;

update `quest_template_addon`,`quest_template_addon_copy`
set `quest_template_addon`.`PrevQuestID`=`quest_template_addon_copy`.`ID`
where `quest_template_addon`.`ID`=`quest_template_addon_copy`.`nextquestid`;

DROP TABLE IF EXISTS `quest_template_addon_copy`;

update `quest_template_addon`,`quest_template`
set `quest_template_addon`.`ProvidedItemCount`='1'
where `quest_template_addon`.`id`=`quest_template`.`id` and `quest_template`.`StartItem`!=0 AND `quest_template_addon`.`ProvidedItemCount`='0';

update `quest_template_addon`,`quest_template`
set `quest_template_addon`.`ProvidedItemCount`=0
where `quest_template_addon`.`ID`=`quest_template`.`ID` and `quest_template`.`StartItem`='0';

-- 采矿
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='186'
where `quest_template`.`QuestSortID`='-408'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 剥皮
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='393'
where `quest_template`.`QuestSortID`='-411'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;
-- 附魔
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='333'
where `quest_template`.`QuestSortID`='-410'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 钓鱼
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='356'
where `quest_template`.`QuestSortID`='-101'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 炼金
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='171'
where `quest_template`.`QuestSortID`='-181'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 考古
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='794'
where `quest_template`.`QuestSortID`='-377'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 煅造
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='164'
where `quest_template`.`QuestSortID`='-121'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 烹饪
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='185'
where `quest_template`.`QuestSortID`='-304'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 工程
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='202'
where `quest_template`.`QuestSortID`='-201'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 急救
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='129'
where `quest_template`.`QuestSortID`='-324'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 草药
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='182'
where `quest_template`.`QuestSortID`='-24'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 铭文
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='773'
where `quest_template`.`QuestSortID`='-371'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 珠宝
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='755'
where `quest_template`.`QuestSortID`='-373'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 制皮
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='165'
where `quest_template`.`QuestSortID`='-182'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

-- 裁缝
update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillID`='197'
where `quest_template`.`QuestSortID`='-264'
and `quest_template_addon`.`ID`=`quest_template`.`ID`;

update `quest_template_addon`,`quest_template`
SET `quest_template_addon`.`RequiredSkillPoints`='1'
where `quest_template`.`QuestSortID`<'0'
and `quest_template_addon`.`ID`=`quest_template`.`ID`
and `quest_template_addon`.`RequiredSkillPoints`='0';
