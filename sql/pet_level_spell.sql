DROP TABLE IF EXISTS `pet_level_spell`;
CREATE TABLE `pet_level_spell` (
  `pet_family` int(11) unsigned NOT NULL default '0' COMMENT 'Pet Family',
  `level` int(11) unsigned NOT NULL default '0' COMMENT 'Required Spell Level',
  `spellid` int(11) unsigned NOT NULL default '0' COMMENT 'Spell Identifier',
  PRIMARY KEY  (`pet_family`,`spellid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';


DELETE FROM `pet_level_spell`;
-- Imp
INSERT INTO `pet_level_spell` VALUES ('23','1','3110');    -- Firebolt Rank 1
INSERT INTO `pet_level_spell` VALUES ('23','4','6307');    -- Blood Pact Rank 1
INSERT INTO `pet_level_spell` VALUES ('23','8','7799');    -- Firebolt Rank 2
INSERT INTO `pet_level_spell` VALUES ('23','12','4511');   -- Phase Shift
INSERT INTO `pet_level_spell` VALUES ('23','14','7804');   -- Blood Pact Rank 2
INSERT INTO `pet_level_spell` VALUES ('23','14','2947');   -- Fire Shield Rank 1
INSERT INTO `pet_level_spell` VALUES ('23','18','7800');   -- Firebolt Rank 3
INSERT INTO `pet_level_spell` VALUES ('23','24','8316');   -- Fire Shield Rank 2
INSERT INTO `pet_level_spell` VALUES ('23','26','7805');   -- Blood Pact Rank 3
INSERT INTO `pet_level_spell` VALUES ('23','28','7801');   -- Firebolt Rank 4
INSERT INTO `pet_level_spell` VALUES ('23','34','8317');   -- Fire Shield Rank 5
INSERT INTO `pet_level_spell` VALUES ('23','38','11766');  -- Blood Pact Rank 4
INSERT INTO `pet_level_spell` VALUES ('23','38','7802');   -- Firebolt Rank 5
INSERT INTO `pet_level_spell` VALUES ('23','44','11770');  
INSERT INTO `pet_level_spell` VALUES ('23','48','11762');
INSERT INTO `pet_level_spell` VALUES ('23','50','11767');
INSERT INTO `pet_level_spell` VALUES ('23','54','11771');
INSERT INTO `pet_level_spell` VALUES ('23','58','11763');
INSERT INTO `pet_level_spell` VALUES ('23','62','27268');
INSERT INTO `pet_level_spell` VALUES ('23','64','27269');
INSERT INTO `pet_level_spell` VALUES ('23','68','27267');
INSERT INTO `pet_level_spell` VALUES ('23','74','47982');
INSERT INTO `pet_level_spell` VALUES ('23','76','47983');
INSERT INTO `pet_level_spell` VALUES ('23','78','47964');
-- Voidwalker
INSERT INTO `pet_level_spell` VALUES ('16','10','3716');
INSERT INTO `pet_level_spell` VALUES ('16','16','7812');
INSERT INTO `pet_level_spell` VALUES ('16','18','17767');
INSERT INTO `pet_level_spell` VALUES ('16','20','7809');
INSERT INTO `pet_level_spell` VALUES ('16','24','19438');
INSERT INTO `pet_level_spell` VALUES ('16','24','17735');
INSERT INTO `pet_level_spell` VALUES ('16','26','17850');
INSERT INTO `pet_level_spell` VALUES ('16','30','7810');
INSERT INTO `pet_level_spell` VALUES ('16','32','19440');
INSERT INTO `pet_level_spell` VALUES ('16','34','17851');
INSERT INTO `pet_level_spell` VALUES ('16','36','17750');
INSERT INTO `pet_level_spell` VALUES ('16','40','19441');
INSERT INTO `pet_level_spell` VALUES ('16','40','7811');
INSERT INTO `pet_level_spell` VALUES ('16','42','17852');
INSERT INTO `pet_level_spell` VALUES ('16','48','19442');
INSERT INTO `pet_level_spell` VALUES ('16','48','17751');
INSERT INTO `pet_level_spell` VALUES ('16','50','17853');
INSERT INTO `pet_level_spell` VALUES ('16','50','11774');
INSERT INTO `pet_level_spell` VALUES ('16','56','19443');
INSERT INTO `pet_level_spell` VALUES ('16','58','17854');
INSERT INTO `pet_level_spell` VALUES ('16','60','17752');
INSERT INTO `pet_level_spell` VALUES ('16','60','11775');
INSERT INTO `pet_level_spell` VALUES ('16','63','27271');
INSERT INTO `pet_level_spell` VALUES ('16','64','27273');
INSERT INTO `pet_level_spell` VALUES ('16','66','27272');
INSERT INTO `pet_level_spell` VALUES ('16','69','33701');
INSERT INTO `pet_level_spell` VALUES ('16','70','27270');
INSERT INTO `pet_level_spell` VALUES ('16','72','47985');
INSERT INTO `pet_level_spell` VALUES ('16','73','47987');
INSERT INTO `pet_level_spell` VALUES ('16','75','47989');
INSERT INTO `pet_level_spell` VALUES ('16','78','47988');
INSERT INTO `pet_level_spell` VALUES ('16','79','47986');
INSERT INTO `pet_level_spell` VALUES ('16','80','47990');
INSERT INTO `pet_level_spell` VALUES ('16','80','47984');
-- Succubus
INSERT INTO `pet_level_spell` VALUES ('17','20','7814');
INSERT INTO `pet_level_spell` VALUES ('17','22','6360');
INSERT INTO `pet_level_spell` VALUES ('17','26','6358');
INSERT INTO `pet_level_spell` VALUES ('17','28','7815');
INSERT INTO `pet_level_spell` VALUES ('17','32','7870');
INSERT INTO `pet_level_spell` VALUES ('17','34','7813');
INSERT INTO `pet_level_spell` VALUES ('17','36','7816');
INSERT INTO `pet_level_spell` VALUES ('17','44','11778');
INSERT INTO `pet_level_spell` VALUES ('17','46','11784');
INSERT INTO `pet_level_spell` VALUES ('17','52','11779');
INSERT INTO `pet_level_spell` VALUES ('17','58','11785');
INSERT INTO `pet_level_spell` VALUES ('17','60','11780');
INSERT INTO `pet_level_spell` VALUES ('17','68','27274');
INSERT INTO `pet_level_spell` VALUES ('17','70','27275');
INSERT INTO `pet_level_spell` VALUES ('17','74','47991');
INSERT INTO `pet_level_spell` VALUES ('17','80','47992');
-- Felhunter
INSERT INTO `pet_level_spell` VALUES ('15','30','19505');
INSERT INTO `pet_level_spell` VALUES ('15','32','54424');
INSERT INTO `pet_level_spell` VALUES ('15','36','19244');
INSERT INTO `pet_level_spell` VALUES ('15','38','19731');
INSERT INTO `pet_level_spell` VALUES ('15','42','57564');
INSERT INTO `pet_level_spell` VALUES ('15','42','54049');
INSERT INTO `pet_level_spell` VALUES ('15','46','19734');
INSERT INTO `pet_level_spell` VALUES ('15','50','54050');
INSERT INTO `pet_level_spell` VALUES ('15','52','57565');
INSERT INTO `pet_level_spell` VALUES ('15','52','19647');
INSERT INTO `pet_level_spell` VALUES ('15','54','19736');
INSERT INTO `pet_level_spell` VALUES ('15','58','54051');
INSERT INTO `pet_level_spell` VALUES ('15','62','27276');
INSERT INTO `pet_level_spell` VALUES ('15','62','57566');
INSERT INTO `pet_level_spell` VALUES ('15','66','54052');
INSERT INTO `pet_level_spell` VALUES ('15','70','27277');
INSERT INTO `pet_level_spell` VALUES ('15','72','57567');
INSERT INTO `pet_level_spell` VALUES ('15','74','54053');
INSERT INTO `pet_level_spell` VALUES ('15','77','48011');
-- Felguard
INSERT INTO `pet_level_spell` VALUES ('29','50','33698');
INSERT INTO `pet_level_spell` VALUES ('29','50','30213');
INSERT INTO `pet_level_spell` VALUES ('29','52','30151');
INSERT INTO `pet_level_spell` VALUES ('29','60','33699');
INSERT INTO `pet_level_spell` VALUES ('29','60','30219');
INSERT INTO `pet_level_spell` VALUES ('29','61','30194');
INSERT INTO `pet_level_spell` VALUES ('29','68','30223');
INSERT INTO `pet_level_spell` VALUES ('29','69','33700');
INSERT INTO `pet_level_spell` VALUES ('29','69','30198');
INSERT INTO `pet_level_spell` VALUES ('29','76','47994');
INSERT INTO `pet_level_spell` VALUES ('29','78','47993');
INSERT INTO `pet_level_spell` VALUES ('29','79','47996');