#include <GaudiKernel/DeclareFactoryEntries.h>
#include <TrigHTTFastSim/HTTFastSim.h>
#include "../TrigHTTSmearingSvc_TDR.h"

DECLARE_ALGORITHM_FACTORY( HTTFastSim )

DECLARE_SERVICE_FACTORY( TrigHTTSmearingSvc_TDR )


DECLARE_FACTORY_ENTRIES( HTTFastSim )
{
  DECLARE_ALGORITHM( HTTFastSim );

  DECLARE_SERVICE( TrigHTTSmearingSvc_TDR );
  
}

