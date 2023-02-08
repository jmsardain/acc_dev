#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../HTTByteStreamDecoderAlg.h"
#include "TrigHTTByteStream/TrigHTTTPByteStreamTool.h"
#include "TrigHTTByteStream/TrigHTTHardwareConversionTool.h"

DECLARE_COMPONENT( HTTByteStreamDecoderAlg )
DECLARE_COMPONENT( TrigHTTTPByteStreamTool )
DECLARE_COMPONENT( TrigHTTHardwareConversionTool )

 DECLARE_FACTORY_ENTRIES( TrigHTTByteStream )
{
   DECLARE_ALGORITHM( HTTByteStreamDecoderAlg );
}
