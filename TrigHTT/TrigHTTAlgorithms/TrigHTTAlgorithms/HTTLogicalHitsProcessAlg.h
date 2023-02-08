#ifndef HTT_LOGICALHITSPROCESSALG_H
#define HTT_LOGICALHITSPROCESSALG_H

/*
 * Please put a description on what this class does
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigHTTObjects/HTTDataFlowInfo.h"
#include "xAODTrigHTT/HTTTrackTestContainer.h"

#include <fstream>

class HTTClusteringToolI;
class HTTEventInputHeaderToolI;
class HTTEventOutputHeaderToolI;
class HTTHitFilteringToolI;
class HTTRoadFinderToolI;
class HTTRoadFilterToolI;
class HTTSpacePointsToolI;

class HTT_RawToLogicalHitsTool;
class HTT_ReadRawRandomHitsTool;
class HTTDataFlowTool;
class HTTHoughMonitorTool;
class HTTLLPHoughMonitorTool;
class HTTHoughRootOutputTool;
class HTTLLPRoadFilterTool;
class HTTMonitorUnionTool;
class HTTNNTrackTool;
class HTTOverlapRemovalTool;
class HTTTrackFitterTool;

class HTTCluster;
class HTTEventInputHeader;
class HTTExtrapolator;
class HTTHit;
class HTTLogicalEventInputHeader;
class HTTLogicalEventOutputHeader;
class HTTRoad;
class HTTTrack;

class IHTTEventSelectionSvc;
class ITrigHTTBankSvc;
class ITrigHTTMappingSvc;

class HTTLogicalHitsProcessAlg : public AthAlgorithm
{
    public:
        HTTLogicalHitsProcessAlg(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~HTTLogicalHitsProcessAlg() = default;

        StatusCode initialize() override;
        StatusCode execute() override;
        StatusCode finalize() override;

    private:

        std::string m_description;
        int m_histoPrintDetail = 0; // must be int type for gaudi
        int m_ev = 0;

        // Handles
        ToolHandle<HTTEventInputHeaderToolI>    m_hitInputTool; // { this, "HitInputTool", "HTT_SGHitInput/HTT_SGHitInput", "HitInput Tool" };
        ToolHandle<HTT_ReadRawRandomHitsTool>   m_hitInputTool2; // for now this can only be HTT_ReadRawRandomHitsTool
        ToolHandle<HTT_RawToLogicalHitsTool>    m_hitMapTool;// { this, "MapTool", "HTT_SGHitInput/HTT_SGHitInput", "HitInput Tool" };
        ToolHandle<HTTHitFilteringToolI>        m_hitFilteringTool;
        ToolHandle<HTTClusteringToolI>          m_clusteringTool; // { this, "HTTClusteringFTKTool", "HTTClusteringFTKTool/HTTClusteringFTKTool", "FTKClusteringTool" };
        ToolHandle<HTTSpacePointsToolI>         m_spacepointsTool;
        ToolHandle<HTTRoadFinderToolI>          m_roadFinderTool;
        ToolHandle<HTTLLPRoadFilterTool>        m_LRTRoadFilterTool;
        ToolHandle<HTTRoadFinderToolI>          m_LRTRoadFinderTool;
        ToolHandle<HTTRoadFilterToolI>          m_roadFilterTool;
        ToolHandle<HTTRoadFilterToolI>          m_roadFilterTool2;
        ToolHandle<HTTNNTrackTool>              m_NNTrackTool;
        ToolHandle<HTTHoughRootOutputTool>      m_houghRootOutputTool;
        ToolHandle<HTTTrackFitterTool>          m_trackFitterTool_1st;
        ToolHandle<HTTTrackFitterTool>          m_trackFitterTool_2nd;
        ToolHandle<HTTOverlapRemovalTool>       m_overlapRemovalTool_1st;
        ToolHandle<HTTOverlapRemovalTool>       m_overlapRemovalTool_2nd;
        ToolHandle<HTTExtrapolator>             m_extrapolator;
        ToolHandle<HTTMonitorUnionTool>         m_monitorUnionTool;
        ToolHandle<HTTHoughMonitorTool>         m_houghMonitorTool;
        ToolHandle<HTTLLPHoughMonitorTool>         m_LLPhoughMonitorTool;
        ToolHandle<HTTDataFlowTool>             m_dataFlowTool;
        ToolHandle<HTTEventOutputHeaderToolI>   m_writeOutputTool;

        ServiceHandle<ITrigHTTMappingSvc>       m_HTTMapping;
        ServiceHandle<IHTTEventSelectionSvc>    m_evtSel;

        // Flags
        IntegerProperty m_firstInputToolN = 1; // number of times to reuse each event in first input tool
        IntegerProperty m_secondInputToolN = 0; // flag to count nmber of times to use the second input tool
        BooleanProperty m_monitor = false;
        BooleanProperty m_houghMonitor = false;
        BooleanProperty m_LLPhoughMonitor = false;
        BooleanProperty m_doHitFiltering = false;
        BooleanProperty m_clustering = false;
        BooleanProperty m_doSpacepoints = false;
        BooleanProperty m_doTracking = false;
        BooleanProperty m_doMissingHitsChecks = false;

        BooleanProperty m_filterRoads = false;
        BooleanProperty m_filterRoads2 = false;
        BooleanProperty m_runSecondStage = false;
        BooleanProperty m_doHoughRootOutput = false;
        BooleanProperty m_doNNTrack = false;
        BooleanProperty m_doLRT = false;
        BooleanProperty m_doLRTHitFiltering = false;
        BooleanProperty m_writeOutputData= true;

        // hit output text file
        BooleanProperty m_outputHitTxt = false;
        std::string m_outputHitTxtName = "outputRoadHits.txt";
        std::ofstream m_outputHitTxtStream;

        // Output handles, used to monitor or BS decoding
        SG::WriteHandleKey<xAOD::HTTTrackTestContainer> m_trackKey;//{this,"HTTTrackKey","HTTTrackKey", "SG key of the HTTTrackContainer"};
        // SG::WriteHandleKey<HTTHitContainer> m_hitKey;//{this,"HTTHitKey","HTTHitKey", "SG key of the HTTHitContainer"};

        // ROOT pointers CAN BE REMOVED
        HTTEventInputHeader*         m_eventHeader;
        HTTLogicalEventInputHeader*  m_logicEventHeader_1st;
        HTTLogicalEventInputHeader*  m_logicEventHeader_2nd;
        HTTLogicalEventOutputHeader* m_logicEventOutputHeader;


        // Event storage
        std::vector<HTTCluster> m_clusters_1st, m_clusters_1st_original, m_clusters_2nd;
        std::vector<HTTCluster> m_spacepoints_1st, m_spacepoints_2nd;
        std::vector<HTTHit>     m_hits_1st_miss, m_hits_2nd_miss;
        std::vector<HTTTrack>   m_tracks_1st_guessedcheck, m_tracks_1st_nomiss, m_tracks_2nd_guessedcheck, m_tracks_2nd_nomiss;


        StatusCode readInputs(bool & done);
        StatusCode processInputs();
        StatusCode secondStageProcessing(std::vector<HTTTrack> const & tracks_1st,
                                         std::vector<HTTRoad*> & roads_2nd, std::vector<HTTTrack> & tracks_2nd,
                                         HTTDataFlowInfo* dataFlowInfo);

        StatusCode endEventMonitor(std::vector<HTTRoad*> const & roads_1st, std::vector<HTTTrack> & tracks_1st,
                                   std::vector<HTTRoad*> const & roads_2nd, std::vector<HTTTrack> & tracks_2nd);
        StatusCode writeOutputData(std::vector<HTTRoad*> const & roads_1st, std::vector<HTTTrack> const & tracks_1st,
                                   std::vector<HTTRoad*> const & roads_2nd, std::vector<HTTTrack> const & tracks_2nd,
                                   HTTDataFlowInfo const * dataFlowInfo);

        void printHitSubregions(std::vector<HTTHit> const & hits);
};

#endif // HTTLOGICALHITSTOALGORITHMS_h
