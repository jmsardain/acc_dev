/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTSECTORMAP_H
#define HTTSECTORMAP_H

#include <vector>
#include <map>

/* 
   A simple map that links sectors in 8L, 4L, and 11L cases:
   HTTSectorMap::GetSector(sector_8L,sector_4L) returns sector_11L
*/
class HTTSectorMap {
  typedef std::map<int,int> mapint;
  typedef std::map<int,mapint> mapint2;
private:
  mapint2 m_data;
public:
  HTTSectorMap() {};
  HTTSectorMap(const char *fname) { LoadFromFile(fname); }
  ~HTTSectorMap() {};
  void SetSector(int, int, int);
  int GetSector(int, int);
  int LoadFromFile(const char *);
  int CreateFile(const char *,const char *, const char *);
  void Dump();
  static std::map<int,int> makeLookup(const char* fname);
};

#endif
