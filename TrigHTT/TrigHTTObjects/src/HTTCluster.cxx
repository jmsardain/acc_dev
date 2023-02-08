/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTCluster.h"

void HTTCluster::Print(Option_t *) const
{
  std::cout << "Cluster formed from " << this->getHitList().size() << " hits.\n" <<std::endl;
}
