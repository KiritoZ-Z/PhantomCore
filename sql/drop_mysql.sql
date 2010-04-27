REVOKE ALL PRIVILEGES ON * . * FROM 'phantom'@'localhost';

REVOKE ALL PRIVILEGES ON `world` . * FROM 'phantom'@'localhost';

REVOKE GRANT OPTION ON `world` . * FROM 'phantom'@'localhost';

REVOKE ALL PRIVILEGES ON `characters` . * FROM 'phantom'@'localhost';

REVOKE GRANT OPTION ON `characters` . * FROM 'phantom'@'localhost';

REVOKE ALL PRIVILEGES ON `realmd` . * FROM 'phantom'@'localhost';

REVOKE GRANT OPTION ON `realmd` . * FROM 'phantom'@'localhost';

DROP USER 'phantom'@'localhost';

DROP DATABASE IF EXISTS `world`;

DROP DATABASE IF EXISTS `characters`;

DROP DATABASE IF EXISTS `realmd`;
