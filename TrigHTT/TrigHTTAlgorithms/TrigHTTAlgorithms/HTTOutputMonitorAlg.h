#ifndef HTT_OUTPUTMONITORALG_H
#define HTT_OUTPUTMONITORALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class HTTDataFlowTool;
class HTTEventOutputHeaderToolI;
class HTTLogicalEventInputHeader;
class HTTLogicalEventOutputHeader;
class HTTMonitorUnionTool;

class HTTOutputMonitorAlg : public AthAlgorithm
{
    public:

        HTTOutputMonitorAlg(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~HTTOutputMonitorAlg() = default;

        virtual StatusCode initialize() override;
        virtual StatusCode execute() override;
        virtual StatusCode finalize() override;

    private:

        // Handles
        ToolHandle<HTTEventOutputHeaderToolI>   m_readOutputTool;
        ToolHandle<HTTMonitorUnionTool>         m_monitorUnionTool;
        ToolHandle<HTTDataFlowTool>             m_dataFlowTool;

        // ROOT pointers
        HTTLogicalEventInputHeader*     m_inputHeader_1st;
        HTTLogicalEventInputHeader*     m_inputHeader_2nd;
        HTTLogicalEventOutputHeader*    m_outputHeader;

        BooleanProperty m_runSecondStage = false;

        int m_histoPrintDetail = 0; // must be int type for gaudi
};

#endif // HTT_OUTPUTMONITORALG_H
