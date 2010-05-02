-- Accounts
ALTER TABLE `account`
ADD COLUMN `premium` int(255) NOT NULL default 0 AFTER `locale`,
ADD COLUMN `premium_time` int(255) NOT NULL default 0 AFTER `premium`;