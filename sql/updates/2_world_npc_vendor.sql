ALTER TABLE npc_vendor
  DROP PRIMARY KEY,
  PRIMARY KEY  (`entry`,`item`,`ExtendedCost`);