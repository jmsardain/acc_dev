/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTMaps/HTTSectorMap.h"
#include "TrigHTTUtils/HTTDcap.h"

#include <iostream>
#include <cstdlib>
using namespace std;

std::map<int,int> HTTSectorMap::makeLookup(const char* fname)
{
  std::map<int,int> secLookup;
  htt_dcap::istream inFile;

  const bool ok_read = htt_dcap::open_for_read(fname,inFile);
  if (!inFile || !ok_read) {
    cout << "Unable to open file";
    exit(1); // terminate with error
  }
  int evt,reg;
  while (inFile >> evt >> reg) {
    secLookup[evt]=reg;
  }

  return secLookup;
}

void HTTSectorMap::SetSector(int sec1, int sec2, int sec) {
  if (m_data.find(sec1)==m_data.end()) m_data[sec1] = mapint();
  m_data[sec1][sec2]=sec;
}

int HTTSectorMap::GetSector(int sec1, int sec2) {
  if (m_data.find(sec1)!=m_data.end())
    if (m_data[sec1].find(sec2)!=m_data[sec1].end())
      return m_data[sec1][sec2];
  return -1;
}

int HTTSectorMap::LoadFromFile(const char *fname) {
  htt_dcap::istream mapfile;
  const bool ok_read = htt_dcap::open_for_read(fname,mapfile);
  if (!mapfile || !ok_read) return -1;
  std::cout << "Reading sector map data: " << fname << std::endl;
  int sec1,sec2,sec3;
  while(mapfile >> sec1 >> sec2 >> sec3) SetSector(sec1,sec2,sec3);
  return 0;
}

int HTTSectorMap::CreateFile(const char *fname4,const char *fname8, const char *fname11) {
  std::map<int,int> map4=HTTSectorMap::makeLookup(fname4);
  std::map<int,int> map8=HTTSectorMap::makeLookup(fname8);
  htt_dcap::istream mapfile11;
  const bool ok_read = htt_dcap::open_for_read(fname11,mapfile11);
  if (!mapfile11 || !ok_read) return -1;
  int ev11, sec11;
  while(mapfile11 >> ev11 >> sec11) {
    if(map4.find(ev11)!=map4.end() && map8.find(ev11)!=map8.end())
      SetSector(map4[ev11], map8[ev11], sec11);
  }

  return 0;
}

void HTTSectorMap::Dump() {
  for (mapint2::iterator i = m_data.begin(); i != m_data.end();i++) {
    for (mapint::iterator j = (*i).second.begin(); j != (*i).second.end();j++) {
      std::cout << (*i).first << " " << (*j).first << " " << (*j).second << std::endl;
    }
  }
}
