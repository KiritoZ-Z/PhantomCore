CREATE TABLE IF NOT EXISTS `mail_external` (
  `id` int(20) unsigned NOT NULL AUTO_INCREMENT,
  `receiver` bigint(20) unsigned NOT NULL,
  `subject` varchar(200) DEFAULT 'Support Message',
  `message` varchar(500) DEFAULT 'Support Message',
  `money` int(20) unsigned NOT NULL DEFAULT '0',
  `item` int(20) unsigned NOT NULL DEFAULT '0',
  `item_count` int(20) unsigned NOT NULL DEFAULT '1',
  `sent` tinyint(4) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=967 ;
