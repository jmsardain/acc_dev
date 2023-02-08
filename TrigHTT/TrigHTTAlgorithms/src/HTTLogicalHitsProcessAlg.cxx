#include "TrigHTTAlgorithms/HTTLogicalHitsProcessAlg.h"

#include "TrigHTTAlgorithms/HTTDataFlowTool.h"
#include "TrigHTTAlgorithms/HTTExtrapolator.h"
#include "TrigHTTAlgorithms/HTTHoughMonitorTool.h"
#include "TrigHTTAlgorithms/HTTLLPHoughMonitorTool.h"
#include "TrigHTTAlgorithms/HTTHoughRootOutputTool.h"
#include "TrigHTTAlgorithms/HTTLLPRoadFilterTool.h"
#include "TrigHTTAlgorithms/HTTNNTrackTool.h"
#include "TrigHTTAlgorithms/HTTOverlapRemovalTool.h"
#include "TrigHTTAlgorithms/HTTRoadFilterToolI.h"
#include "TrigHTTAlgorithms/HTTRoadFinderToolI.h"
#include "TrigHTTAlgorithms/HTTTrackFitterTool.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTMaps/HTTClusteringToolI.h"
#include "TrigHTTConfig/HTTRegionSlices.h"
#include "TrigHTTConfig/IHTTEventSelectionSvc.h"
#include "TrigHTTMaps/HTTHitFilteringToolI.h"
#include "TrigHTTInput/HTT_RawToLogicalHitsTool.h"
#include "TrigHTTInput/HTT_ReadRawRandomHitsTool.h"
#include "TrigHTTInput/HTTEventInputHeaderToolI.h"
#include "TrigHTTInput/HTTEventOutputHeaderToolI.h"
#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTMonitor/HTTMonitorUnionTool.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTMaps/HTTSpacePointsToolI.h"
#include "xAODTrigHTT/HTTTrackTest.h"
#include "xAODTrigHTT/HTTTrackTestAuxContainer.h"

#include "GaudiKernel/IEventProcessor.h"

// #define BENCHMARK_LOGICALHITSALG

#ifdef BENCHMARK_LOGICALHITSALG
#define TIME(name) \
    t_1 = std::chrono::steady_clock::now(); \
    (name) += std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count(); \
    t_0 = t_1;

size_t m_tread = 0, m_tprocess = 0, m_troads = 0, m_troad_filter = 0, m_tlrt = 0, m_ttracks = 0, m_tOR = 0, m_t2ndStage = 0, m_tmon = 0, m_tfin = 0;
#else
#define TIME(name)
#endif


///////////////////////////////////////////////////////////////////////////////
// Initialize

HTTLogicalHitsProcessAlg::HTTLogicalHitsProcessAlg (const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_hitInputTool("HTTSGToRawHitsTool/HTTSGToRawHitsTool"),
    m_hitInputTool2("HTT_RandRawHitsFromInvConstsTool/HTT_RandRawHitsFromInvConstsTool"),
    m_hitMapTool("HTT_RawToLogicalHitsTool/HTT_RawToLogicalHitsTool"),
    m_hitFilteringTool("HTTHitFilteringTool/HTTHitFilteringTool"),
    m_clusteringTool("HTTClusteringTool/HTTClusteringTool"),
    m_spacepointsTool("HTTSpacePointsTool/HTTSpacePointsTool"),
    m_roadFinderTool("HTTPatternMatchTool"),
    m_LRTRoadFilterTool("HTTLRTRoadFilterTool"),
    m_LRTRoadFinderTool("HTTLRTRoadFinderTool"),
    m_roadFilterTool("HTTEtaPatternFilterTool"),
    m_roadFilterTool2("HTTPhiRoadFilterTool"),
    m_NNTrackTool("HTTNNTrackTool/HTTNNTrackTool"),
    m_houghRootOutputTool("HTTHoughRootOutputTool/HTTHoughRootOutputTool"),
    m_trackFitterTool_1st("HTTTrackFitterTool/HTTTrackFitterTool_1st"),
    m_trackFitterTool_2nd("HTTTrackFitterTool/HTTTrackFitterTool_2nd"),
    m_overlapRemovalTool_1st("HTTOverlapRemovalTool/HTTOverlapRemovalTool_1st"),
    m_overlapRemovalTool_2nd("HTTOverlapRemovalTool/HTTOverlapRemovalTool_2nd"),
    m_extrapolator("HTTExtrapolator/HTTExtrapolator"),
    m_monitorUnionTool("HTTMonitorUnionTool/HTTMonitorUnionTool"),
    m_houghMonitorTool("HTTHoughMonitorTool/HTTHoughMonitorTool"),
    m_LLPhoughMonitorTool("HTTLLPHoughMonitorTool/HTTLLPHoughMonitorTool"),
    m_dataFlowTool("HTTDataFlowTool/HTTDataFlowTool"),
    m_writeOutputTool("HTTOutputHeaderTool/HTTOutputHeaderTool"),
    m_HTTMapping("TrigHTTMappingSvc", name),
    m_evtSel("HTTEventSelectionSvc", name)
{
    declareProperty("InputTool",                    m_hitInputTool);
    declareProperty("InputTool2",                   m_hitInputTool2);
    declareProperty("HTTHoughMonitorTool",          m_houghMonitorTool);
    declareProperty("HTTLLPHoughMonitorTool",          m_LLPhoughMonitorTool);
    declareProperty("RawToLogicalHitsTool",         m_hitMapTool);
    declareProperty("RoadFinder",                   m_roadFinderTool);
    declareProperty("LRTRoadFinder",                m_LRTRoadFinderTool);
    declareProperty("LRTRoadFilter",                m_LRTRoadFilterTool);
    declareProperty("doLRT",                        m_doLRT,                            "Enable Large Radious Tracking");
    declareProperty("LRTHitFiltering",              m_doLRTHitFiltering,                "flag to enable hit/cluster filtering for LRT");
    declareProperty("RoadFilter",                   m_roadFilterTool);
    declareProperty("RoadFilter2",                  m_roadFilterTool2);
    declareProperty("TrackFitter_1st",              m_trackFitterTool_1st);
    declareProperty("TrackFitter_2nd",              m_trackFitterTool_2nd);
    declareProperty("OverlapRemoval_1st",           m_overlapRemovalTool_1st);
    declareProperty("OverlapRemoval_2nd",           m_overlapRemovalTool_2nd);
    declareProperty("SpacePointTool",               m_spacepointsTool,                  "tool for spacepoint formation");
    declareProperty("MonitorUnionTool",             m_monitorUnionTool);
    declareProperty("DataFlowTool",                 m_dataFlowTool);
    declareProperty("OutputTool",                   m_writeOutputTool);
    declareProperty("FirstInputToolN",              m_firstInputToolN,                  "number of times to reuse each event in first input tool");
    declareProperty("SecondInputToolN",             m_secondInputToolN,                 "number of times to run second input tool");
    declareProperty("Monitor",                      m_monitor,                          "flag to enable the monitor");
    declareProperty("HoughMonitor",                 m_houghMonitor,                     "flag to enable the Hough monitor");
    declareProperty("LLPHoughMonitor",                 m_LLPhoughMonitor,                     "flag to enable the Hough monitor");
    declareProperty("HitFiltering",                 m_doHitFiltering,                   "flag to enable hit/cluster filtering");
    declareProperty("Clustering",                   m_clustering,                       "flag to enable the clustering");
    declareProperty("SpacePoints",                  m_doSpacepoints,                    "flag to enable the spacepoint formation");
    declareProperty("tracking",                     m_doTracking,                       "flag to enable the tracking");
    declareProperty("DoHoughRootOutput",            m_doHoughRootOutput,                "Dump output from the Hough Transform to flat ntuples");
    declareProperty("FilterRoads",                  m_filterRoads,                      "enable first road filter");
    declareProperty("FilterRoads2",                 m_filterRoads2,                     "enable second road filter");
    declareProperty("DoNNTrack",                    m_doNNTrack,                        "Run NN track filtering");
    declareProperty("DoMissingHitsChecks",          m_doMissingHitsChecks,              "flag to enable missing hits check");
    declareProperty("RunSecondStage",               m_runSecondStage,                   "flag to enable running the second stage fitting");
    declareProperty("Extrapolator",                 m_extrapolator,                     "flag to run second stage extrapolator");
    declareProperty("histoPrintDetail",             m_histoPrintDetail,                 "histogram printing detail, see HTTMonitorI");
    declareProperty("outputHitTxt",                 m_outputHitTxt,                     "write out road hits to text file");
    declareProperty("outputHitTxtName",             m_outputHitTxtName,                 "name of txt file to write road hits to");
    declareProperty("writeOutputData",              m_writeOutputData,                  "write teh output TTree");
    declareProperty("HTTTrackKey",                  m_trackKey,                         "SG key of the HTTTrackContainer");
    declareProperty("description",                  m_description,                      "tag description");
    declareProperty("eventSelector",                m_evtSel,                           "Event selector tool to use");
}


StatusCode HTTLogicalHitsProcessAlg::initialize()
{
    std::stringstream ss(m_description);
    std::string line;
    ATH_MSG_INFO("Tag config:");
    if (m_description != NULL) {
        while (std::getline(ss, line, '\n')) {
            ATH_MSG_INFO('\t' << line);
        }
    }

    ATH_CHECK(m_hitInputTool.retrieve());
    if (m_secondInputToolN > 0) ATH_CHECK(m_hitInputTool2.retrieve());
    ATH_CHECK(m_hitMapTool.retrieve());
    if (m_doHitFiltering) ATH_CHECK(m_hitFilteringTool.retrieve());
    if (m_clustering) ATH_CHECK(m_clusteringTool.retrieve());
    if (m_doSpacepoints) ATH_CHECK(m_spacepointsTool.retrieve());
    ATH_CHECK(m_roadFinderTool.retrieve());
    // if (m_monitor && m_houghMonitor)
    // {
    //     ATH_CHECK(m_houghMonitorTool.retrieve());
    //     m_houghMonitorTool->setTool(m_roadFinderTool.get());
    // }
    if (m_monitor && m_LLPhoughMonitor)
    {
        ATH_CHECK(m_LLPhoughMonitorTool.retrieve());
        m_LLPhoughMonitorTool->setTool(m_LRTRoadFinderTool.get());
    }
    if (m_doLRT) {
        ATH_CHECK(m_LRTRoadFilterTool.retrieve());
        ATH_CHECK(m_LRTRoadFinderTool.retrieve());
    }
    if (m_doHoughRootOutput) ATH_CHECK(m_houghRootOutputTool.retrieve());
    if (m_doNNTrack) ATH_CHECK(m_NNTrackTool.retrieve());
    if (m_filterRoads) ATH_CHECK(m_roadFilterTool.retrieve());
    if (m_filterRoads2) ATH_CHECK(m_roadFilterTool2.retrieve());
    if (m_doTracking) ATH_CHECK(m_trackFitterTool_1st.retrieve());
    ATH_CHECK(m_overlapRemovalTool_1st.retrieve());
    // if (m_runSecondStage) {
    //     ATH_CHECK(m_extrapolator.retrieve());
    //     if (m_doTracking) ATH_CHECK(m_trackFitterTool_2nd.retrieve());
    //     ATH_CHECK(m_overlapRemovalTool_2nd.retrieve());
    // }
    if (m_monitor) ATH_CHECK(m_monitorUnionTool.retrieve());
    ATH_CHECK(m_dataFlowTool.retrieve());
    ATH_CHECK(m_writeOutputTool.retrieve());

    ATH_CHECK(m_HTTMapping.retrieve());
    ATH_CHECK(m_evtSel.retrieve());
    ATH_CHECK(m_trackKey.initialize());

    ATH_MSG_DEBUG("initialize() Instantiating root objects");
    m_eventHeader            = new HTTEventInputHeader();
    m_logicEventHeader_1st   = m_writeOutputTool->getLogicalEventInputHeader_1st();
    m_logicEventOutputHeader = m_writeOutputTool->getLogicalEventOutputHeader();
    // if (m_runSecondStage) m_logicEventHeader_2nd = m_writeOutputTool->getLogicalEventInputHeader_2nd();

    ATH_MSG_DEBUG("initialize() Setting branch");

    if (m_monitor) {
        TTree* m_EventTree = m_writeOutputTool->getEventTree();
        ATH_CHECK(m_monitorUnionTool->regTree("/MONITOROUT/", m_EventTree));

        ATH_CHECK(m_monitorUnionTool->passMonitorEvent(m_eventHeader, m_logicEventHeader_1st, m_logicEventHeader_2nd, nullptr));
        // if (m_houghMonitor) ATH_CHECK(m_houghMonitorTool->passMonitorEvent(m_eventHeader, m_logicEventHeader_1st, m_logicEventHeader_2nd, nullptr));
        if (m_LLPhoughMonitor) ATH_CHECK(m_LLPhoughMonitorTool->passMonitorEvent(m_eventHeader, m_logicEventHeader_1st, m_logicEventHeader_2nd, nullptr));
    }

    ATH_CHECK(m_trackKey.initialize());
    if (m_outputHitTxt) {
      ATH_MSG_INFO("writing road hits to " << m_outputHitTxtName);
      m_outputHitTxtStream.open(m_outputHitTxtName);
    }

    ATH_MSG_DEBUG("initialize() Finished");
    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//                          MAIN EXECUTE ROUTINE                             //
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTLogicalHitsProcessAlg::execute()
{
#ifdef BENCHMARK_LOGICALHITSALG
    std::chrono::time_point<std::chrono::steady_clock> t_0, t_1;
    t_0 = std::chrono::steady_clock::now();
#endif

    // Read inputs
    bool done = false;
    ATH_CHECK(readInputs(done));

    if (done) {
      IEventProcessor* appMgr = nullptr;
      ATH_CHECK(service("ApplicationMgr",appMgr));
      if (!appMgr) {
          ATH_MSG_ERROR("Failed to retrieve ApplicationMgr as IEventProcessor");
          return StatusCode::FAILURE;
      }
      return appMgr->stopRun();
    }

    // Apply truth track cuts
    if (!m_evtSel->selectEvent(m_eventHeader))
    {
        ATH_MSG_DEBUG("Event skipped by HTTEventSelectionSvc");
        return StatusCode::SUCCESS;
    }

    TIME(m_tread);

    // Map, cluster, and filter hits
    ATH_CHECK(processInputs());

    // Get reference to hits
    unsigned regionID = m_evtSel->getRegionID();
    std::vector<HTTHit> const & hits_1st = m_logicEventHeader_1st->towers().at(0).hits();

    TIME(m_tprocess);

    // Get roads
    std::vector<HTTRoad*> prefilter_roads;
    std::vector<HTTRoad*>& roads_1st = prefilter_roads;
    //std::vector<HTTRoad*> roads_1st;
    std::vector<const HTTHit*> phits_1st;
    for (HTTHit const & h : hits_1st) phits_1st.push_back(&h);
    //ATH_CHECK(m_roadFinderTool->getRoads(phits_1st, roads_1st));
    ATH_CHECK(  m_LRTRoadFinderTool->getRoads( phits_1st, roads_1st ));

    TIME(m_troads);

    // Standard road Filter
    std::vector<HTTRoad*> postfilter_roads;
    if (m_filterRoads)
    {
        ATH_MSG_INFO("Filtering roads");
        ATH_CHECK(m_roadFilterTool->filterRoads(roads_1st, postfilter_roads));
        roads_1st = postfilter_roads;
    }
    ATH_CHECK(m_overlapRemovalTool_1st->runOverlapRemoval(roads_1st));

    // Road Filter2
    std::vector<HTTRoad*> postfilter2_roads;
    if (m_filterRoads2) {
        ATH_CHECK(m_roadFilterTool2->filterRoads(roads_1st, postfilter2_roads));
        roads_1st = postfilter2_roads;
    }

    TIME(m_troad_filter);

    // Get tracks
    std::vector<HTTTrack> tracks_1st;
    if (m_doTracking) {
        if (m_doNNTrack) {
            ATH_MSG_INFO("Performing NN tracking");
            const HTTNNMap* nnMap = m_HTTMapping->NNMap();
            ATH_CHECK(m_NNTrackTool->getTracks(roads_1st, tracks_1st, nnMap));
        }
        else {
            //ATH_MSG_INFO("Performing standard first stage tracking");
            ATH_CHECK(m_trackFitterTool_1st->getTracks(roads_1st, tracks_1st));
        }
    }

    TIME(m_ttracks);

    // Overlap removal
    ATH_CHECK(m_overlapRemovalTool_1st->runOverlapRemoval(tracks_1st));

    TIME(m_tOR);

    // Now, we may want to do large-radius tracking on the hits not used by the first stage tracking.
    // This follows overlap removal.
    std::vector<HTTRoad*> roadsLRT;
    std::vector<HTTTrack> tracksLRT; // currently empty
    // if (m_doLRT) {
    //     // Filter out hits that are on successful first-stage tracks
    //     std::vector<const HTTHit*> remainingHits;
    //
    //     if (m_doLRTHitFiltering) {
    //         ATH_MSG_INFO("Doing hit filtering based on prompt tracks.");
    //         ATH_CHECK(m_LRTRoadFilterTool->filterUsedHits(tracks_1st, phits_1st, remainingHits));
    //     } else {
    //         ATH_MSG_INFO("No hit filtering requested; using all hits for LRT.");
    //         remainingHits = phits_1st;
    //     }
    //
    //     // Get LRT roads with remaining hits
    //     ATH_MSG_INFO("Finding LRT roads");
    //     ATH_CHECK(m_LRTRoadFinderTool->getRoads( remainingHits, roadsLRT ));
    //
    //     // Test for now
    //     //ATH_CHECK(m_trackFitterTool_1st->getTracks(roadsLRT, tracksLRT));
    // }

    TIME(m_tlrt);

    HTTDataFlowInfo* dataFlowInfo = new HTTDataFlowInfo();

    // Second stage fitting
    std::vector<HTTRoad*> roads_2nd;
    std::vector<HTTTrack> tracks_2nd;
    // if (m_runSecondStage) {
    //     ATH_CHECK(secondStageProcessing(tracks_1st, roads_2nd, tracks_2nd, dataFlowInfo));
    //     TIME(m_t2ndStage);
    // }

    // Calculate data flow quantities
    // ATH_CHECK(m_dataFlowTool->calculateDataFlow(dataFlowInfo, m_logicEventHeader_1st, m_clusters_1st, roads_1st, tracks_1st, roads_2nd, tracks_2nd));

    // Monitoring
    ATH_MSG_DEBUG("Tower " << regionID << " has " << hits_1st.size() << " hits, " << roads_1st.size() << " roads, and " << tracks_1st.size() << " tracks");
    if (m_monitor) ATH_CHECK(endEventMonitor(roads_1st, tracks_1st, roads_2nd, tracks_2nd)); // TODO: Add a monitor tool array.
    TIME(m_tmon);

    // Write the output and reset
    //ATH_CHECK(writeOutputData(roads_1st, tracks_1st, roads_2nd, tracks_2nd, dataFlowInfo));

    // if (m_doHoughRootOutput) {
    //   ATH_CHECK(m_houghRootOutputTool->fillTree(roads_1st, m_logicEventHeader_1st->optional().getTruthTracks(), m_logicEventHeader_1st->optional().getOfflineTracks()));
    // }

    // Write xAOD output
    // TODO put this in writeOutputData
    auto outputTracks = std::make_unique<xAOD::HTTTrackTestContainer>();
    auto store = std::make_unique<xAOD::HTTTrackTestAuxContainer>();
    outputTracks->setStore(store.get());
    for (HTTTrack const & track: tracks_1st)
    {
        xAOD::HTTTrackTest *htrack = new xAOD::HTTTrackTest();
        outputTracks->push_back(htrack);
        outputTracks->back()->setPt(track.getPt());
    }
    ATH_CHECK(SG::makeHandle(m_trackKey).record(std::move(outputTracks), std::move(store)));

    // dump hit identifiers per road/track to text file
    // TODO put this in a helper function
    if(m_outputHitTxt) {

      ATH_MSG_DEBUG("Dumping hits in roads to text file " << m_outputHitTxtName);
      m_outputHitTxtStream << std::endl << "event " << m_ev << ", "
                           << roads_1st.size() << " roads"; // endl moved below to deal with trailing comma

      // m_clusters_original exists, we just need to know which ones to access
      // HTTRoad::getHits returns a vector of HTTHit, but each of them is actually a cluster
      // m_parentageMask has been co-opted to store the cluster index (set in clustering tool)
      // this lets us get hold of the actual HTTCluster via m_clusters_original[index]

      int roadCount = 0;
      for(auto road : roads_1st) {
        // to avoid empty entries if a layer has no hits in a road, save the hits to vector first
        std::vector< std::vector<HTTHit> > clusters_in_road;
        for(int layer = 0; layer < int(road->getNLayers()); layer ++) {
          for(auto cluster_as_hit : road->getHits(layer)) {
            unsigned clusterIndex = (unsigned)cluster_as_hit->getParentageMask();
            HTTCluster thisCluster = m_clusters_1st_original.at(clusterIndex);
            // check that they match (might slow things down unnecessarily - remove later perhaps)
            if( (cluster_as_hit->getR() != thisCluster.getClusterEquiv().getR()) ||
                (cluster_as_hit->getZ() != thisCluster.getClusterEquiv().getZ()) ||
                (cluster_as_hit->getGPhi() != thisCluster.getClusterEquiv().getGPhi()) ){
              ATH_MSG_ERROR("cluster retrieved from index seems not to match");
              std::cout << "road ID " << road->getRoadID() << ", layer " << layer
                        << ", cluster index " << clusterIndex << std::endl;
              std::cout << "road.getHits(layer):  " << cluster_as_hit->getR() << " "
                        << cluster_as_hit->getZ() << " " << cluster_as_hit->getGPhi() << std::endl;
              std::cout << "m_cluster[index]: " << thisCluster.getClusterEquiv().getR() << " "
                        << thisCluster.getClusterEquiv().getZ() << " "
                        << thisCluster.getClusterEquiv().getGPhi() << std::endl;
            }
            clusters_in_road.push_back(thisCluster.getHitList());
          }
        }

        // m_outputHitTxtStream << "  road " << roadCount << ", ID " << road->getRoadID() << ", "
                             // << clusters_in_road << " hits" << std::endl;
        if(roadCount != 0) m_outputHitTxtStream << ",";
        m_outputHitTxtStream << std::endl << "[ ";
        bool firstCluster = true;
        for(auto clusterHits : clusters_in_road) {
          if(firstCluster) firstCluster = false;
          else             m_outputHitTxtStream << ", ";
          bool firstHit = true;
          m_outputHitTxtStream << "[ ";
          for(auto hit : clusterHits) {
            if(firstHit) firstHit = false;
            else         m_outputHitTxtStream << ", ";

            // In the absence of hit.getIdentifier hash, need to get it back from
            // HTTIdentifierHash = (m_identifierHash << 2) + m_etaIndex; // see HTTHit.cxx
            // unsigned identifierHash = hit.getHTTIdentifierHash();
            // if(hit.isStrip())
            //   identifierHash = (hit.getHTTIdentifierHash() - hit.getEtaIndex()) >> 2;
            // But instead can just use hit.getIdentifierHash() having uncommented it from HTTHit.h
            m_outputHitTxtStream << "[" << hit.isStrip() << ", " << hit.getIdentifierHash() << ", "
                                 << hit.getEtaIndex() << ", " << hit.getPhiIndex() << "]";
          }
          m_outputHitTxtStream << " ]";
        }
        m_outputHitTxtStream << " ]";
        roadCount++;
      }
    }

    // Reset data pointers
    m_eventHeader->reset();
    m_logicEventHeader_1st->reset();
    m_logicEventOutputHeader->reset();
    // if (m_runSecondStage) m_logicEventHeader_2nd->reset();

    TIME(m_tfin);

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//                  INPUT PASSING, READING AND PROCESSING                    //
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTLogicalHitsProcessAlg::readInputs(bool & done)
{
    static HTTEventInputHeader h_first;

    if (m_ev % m_firstInputToolN == 0)
    {
        // Read primary input
        ATH_CHECK(m_hitInputTool->readData(&h_first, done));
        if (done)
        {
            ATH_MSG_INFO("Cannot read more events from file, returning");
            return StatusCode::SUCCESS; // end of loop over events
        }
    }

    *m_eventHeader = h_first;

    // Read secondary input
    for (int i = 0; i < m_secondInputToolN; i++)
    {
        ATH_CHECK(m_hitInputTool2->readData(m_eventHeader, done, false));
        if (done)
        {
            ATH_MSG_INFO("Cannot read more events from file, returning");
            return StatusCode::SUCCESS;
        }
    }

    m_ev++;

    return StatusCode::SUCCESS;
}


// Applies clustering, mapping, hit filtering, and space points
StatusCode HTTLogicalHitsProcessAlg::processInputs()
{
    m_clusters_1st.clear();
    m_spacepoints_1st.clear();
    m_hits_1st_miss.clear();

    // Map hits
    m_logicEventHeader_1st->reset();
    ATH_CHECK(m_hitMapTool->convert(1, *m_eventHeader, *m_logicEventHeader_1st));
    if (!m_runSecondStage) m_eventHeader->clearHits();

    // Get unmapped hits for debugging
    if (m_monitor)
    {
        ATH_CHECK(m_hitMapTool->getUnmapped(m_hits_1st_miss));
        ATH_CHECK(m_monitorUnionTool->passMonitorHits(m_hits_1st_miss, "Hits_1st_Miss"));
    }

    // Random removal of hits
    if (m_doHitFiltering) {
        ATH_CHECK(m_hitFilteringTool->DoRandomRemoval(*m_logicEventHeader_1st, true));
    }

    // Clustering
    if (m_clustering)
    {
        ATH_CHECK(m_clusteringTool->DoClustering(*m_logicEventHeader_1st, m_clusters_1st));
        m_clusters_1st_original = m_clusters_1st;
        // I think I also want to pass m_clusters to random removal (but won't work currently)
        if (m_doHitFiltering) ATH_CHECK(m_hitFilteringTool->DoRandomRemoval(*m_logicEventHeader_1st, false));
    }

    // Filter hits/clusters (untested for hits, ie with m_clustering = false)
    if (m_doHitFiltering)
    {
        // get the sets of layers that we want to filter hits from
        std::vector<int> filter_pixel_physLayers, filter_strip_physLayers;
        const HTTPlaneMap *planeMap_1st = m_HTTMapping->PlaneMap_1st();
        ATH_CHECK(m_hitFilteringTool->GetPairedStripPhysLayers(planeMap_1st, filter_strip_physLayers));
        m_clusters_1st.clear();
        ATH_CHECK(m_hitFilteringTool->DoHitFiltering(*m_logicEventHeader_1st, filter_pixel_physLayers, filter_strip_physLayers, m_clusters_1st));
    }

    // Space points
    if (m_doSpacepoints) ATH_CHECK(m_spacepointsTool->DoSpacePoints(*m_logicEventHeader_1st, m_spacepoints_1st));

    return StatusCode::SUCCESS;
}


StatusCode HTTLogicalHitsProcessAlg::secondStageProcessing(std::vector<HTTTrack> const & tracks_1st,
                                                           std::vector<HTTRoad*> & roads_2nd, std::vector<HTTTrack> & tracks_2nd,
                                                           HTTDataFlowInfo* dataFlowInfo)
{
    m_clusters_2nd.clear();
    m_spacepoints_2nd.clear();
    m_hits_2nd_miss.clear();

    // Map hits
    m_logicEventHeader_2nd->reset();
    ATH_CHECK(m_hitMapTool->convert(2, *m_eventHeader, *m_logicEventHeader_2nd));
    m_eventHeader->clearHits();

    // Get unmapped hits for debugging
    if (m_monitor)
    {
        ATH_CHECK(m_hitMapTool->getUnmapped(m_hits_2nd_miss));
        ATH_CHECK(m_monitorUnionTool->passMonitorHits(m_hits_2nd_miss, "Hits_2nd_Miss"));
    }

    if (m_clustering) ATH_CHECK(m_clusteringTool->DoClustering(*m_logicEventHeader_2nd, m_clusters_2nd));

    // Space points
    if (m_doSpacepoints) ATH_CHECK(m_spacepointsTool->DoSpacePoints(*m_logicEventHeader_2nd, m_spacepoints_2nd));

    std::vector<HTTHit> const & hits_2nd = m_logicEventHeader_2nd->towers().at(0).hits();

    ATH_MSG_DEBUG("Number of second stage hits = " << hits_2nd.size());

    std::vector<const HTTHit*> phits_2nd;
    for (HTTHit const & h : hits_2nd) phits_2nd.push_back(&h);

    // Get the first stage tracks after OR
    std::vector<HTTTrack> tracks_1st_OR;
    for (auto track : tracks_1st) {
        if (track.passedOR() == 1) tracks_1st_OR.push_back(track);
    }

    ATH_MSG_DEBUG("First stage tracks after OR for extrapolator " << tracks_1st_OR.size());

    ATH_CHECK(m_extrapolator->getSecondStageFittingInputs(phits_2nd, tracks_1st_OR, roads_2nd));
    dataFlowInfo->nConstants_2nd_extrapolate = m_extrapolator->getNConstantsExtrapolate();

    ATH_MSG_DEBUG("Number of roads for second stage fitting " << roads_2nd.size());

    // Get second stage tracks
    if (m_doTracking) ATH_CHECK(m_trackFitterTool_2nd->getTracks(roads_2nd, tracks_2nd));

    ATH_MSG_DEBUG("Number of tracks from the second stage fitting = " << tracks_2nd.size() << " and truth tracks = " << m_logicEventHeader_1st->optional().getTruthTracks().size());

    // Overlap removal
    ATH_CHECK(m_overlapRemovalTool_2nd->runOverlapRemoval(tracks_2nd));

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//                     OUTPUT WRITING AND MONITORING                         //
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTLogicalHitsProcessAlg::endEventMonitor(std::vector<HTTRoad*> const & roads_1st, std::vector<HTTTrack>& tracks_1st,
                                                     std::vector<HTTRoad*> const & roads_2nd, std::vector<HTTTrack>& tracks_2nd)
{
    for (HTTTrack& t : tracks_1st) t.calculateTruth();
    // if (m_runSecondStage) for (HTTTrack& t : tracks_2nd) t.calculateTruth();

    ATH_CHECK(m_monitorUnionTool->passMonitorRoads(roads_1st, "Roads_1st"));
    ATH_CHECK(m_monitorUnionTool->passMonitorTracks(tracks_1st, "Tracks_1st"));

    // if (m_runSecondStage) {
    //     ATH_CHECK(m_monitorUnionTool->passMonitorRoads(roads_2nd, "Roads_2nd"));
    //     ATH_CHECK(m_monitorUnionTool->passMonitorTracks(tracks_2nd, "Tracks_2nd"));
    // }

    if (m_clustering) {
        ATH_CHECK(m_monitorUnionTool->passMonitorClusters(m_clusters_1st, "Clusters_1st"));
        // if (m_runSecondStage) ATH_CHECK(m_monitorUnionTool->passMonitorClusters(m_clusters_2nd, "Clusters_2nd"));
    }
    if (m_doSpacepoints) {
        ATH_CHECK(m_monitorUnionTool->passMonitorClusters(m_spacepoints_1st, "Spacepoints_1st"));
        // if (m_runSecondStage) ATH_CHECK(m_monitorUnionTool->passMonitorClusters(m_spacepoints_2nd, "Spacepoints_2nd"));
    }

    if (m_doMissingHitsChecks) {
        m_tracks_1st_guessedcheck.clear(); m_tracks_1st_nomiss.clear();

        for (auto const & track : tracks_1st) {
            if (track.getNMissing() == 0) m_tracks_1st_nomiss.push_back(track);
        }
        if (m_doTracking) ATH_CHECK(m_trackFitterTool_1st->getMissingHitsCheckTracks(m_tracks_1st_guessedcheck)); // still need to pass tracks even if all empty since monitor doesn't check if pointers are null
        ATH_CHECK(m_monitorUnionTool->passMonitorTracks(m_tracks_1st_guessedcheck, "Tracks_1st_Guessed"));
        ATH_CHECK(m_monitorUnionTool->passMonitorTracks(m_tracks_1st_nomiss,       "Tracks_1st_NoMiss"));

        // if (m_runSecondStage) {
        //     m_tracks_2nd_guessedcheck.clear(); m_tracks_2nd_nomiss.clear();
        //
        //     for (auto const & track : tracks_2nd) {
        //         if (track.getNMissing() == 0) m_tracks_2nd_nomiss.push_back(track);
        //     }
        //     if (m_doTracking) ATH_CHECK(m_trackFitterTool_2nd->getMissingHitsCheckTracks(m_tracks_2nd_guessedcheck)); // still need to pass tracks even if all empty since monitor doesn't check if pointers are null
        //     ATH_CHECK(m_monitorUnionTool->passMonitorTracks(m_tracks_2nd_guessedcheck, "Tracks_2nd_Guessed"));
        //     ATH_CHECK(m_monitorUnionTool->passMonitorTracks(m_tracks_2nd_nomiss,       "Tracks_2nd_NoMiss"));
        // }
    }

    ATH_CHECK(m_monitorUnionTool->fillHistograms());

    // if (m_houghMonitor) {
    //     ATH_CHECK(m_houghMonitorTool->passMonitorRoads(roads_1st, "Roads_1st"));
    //     ATH_CHECK(m_houghMonitorTool->fillHistograms());
    // }
    if (m_LLPhoughMonitor) {
        ATH_CHECK(m_LLPhoughMonitorTool->passMonitorRoads(roads_1st, "Roads_1st"));
        ATH_CHECK(m_LLPhoughMonitorTool->fillHistograms());
    }

    return StatusCode::SUCCESS;
}


StatusCode HTTLogicalHitsProcessAlg::writeOutputData(std::vector<HTTRoad*> const & roads_1st, std::vector<HTTTrack> const & tracks_1st,
                                                     std::vector<HTTRoad*> const & roads_2nd, std::vector<HTTTrack> const & tracks_2nd,
                                                     HTTDataFlowInfo const * dataFlowInfo)
{
    m_logicEventOutputHeader->reset();

    ATH_MSG_DEBUG("NHTTRoads_1st = " << roads_1st.size() << ", NHTTTracks_1st = " << tracks_1st.size() << ", NHTTRoads_2nd = " << roads_2nd.size() << ", NHTTTracks_2nd = " << tracks_2nd.size());

    if (!m_writeOutputData) return StatusCode::SUCCESS;

    m_logicEventOutputHeader->reserveHTTRoads_1st(roads_1st.size());
    m_logicEventOutputHeader->reserveHTTTracks_1st(tracks_1st.size());

    m_logicEventOutputHeader->addHTTRoads_1st(roads_1st);
    m_logicEventOutputHeader->addHTTTracks_1st(tracks_1st);

    // if (m_runSecondStage) {
    //     m_logicEventOutputHeader->reserveHTTRoads_2nd(roads_2nd.size());
    //     m_logicEventOutputHeader->reserveHTTTracks_2nd(tracks_2nd.size());
    //
    //     m_logicEventOutputHeader->addHTTRoads_2nd(roads_2nd);
    //     m_logicEventOutputHeader->addHTTTracks_2nd(tracks_2nd);
    // }

    m_logicEventOutputHeader->setDataFlowInfo(*dataFlowInfo);
    ATH_MSG_DEBUG(m_logicEventOutputHeader->getDataFlowInfo());

    ATH_CHECK(m_writeOutputTool->writeData(m_logicEventHeader_1st, m_logicEventHeader_2nd, m_logicEventOutputHeader));

    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Finalize

StatusCode HTTLogicalHitsProcessAlg::finalize()
{
#ifdef BENCHMARK_LOGICALHITSALG
    ATH_MSG_INFO("Timings:" <<
            "\nread input:   " << std::setw(10) << m_tread <<
            "\nprocess hits: " << std::setw(10) << m_tprocess <<
            "\nroads:        " << std::setw(10) << m_troads <<
            "\nroad filter:  " << std::setw(10) << m_troad_filter <<
            "\nllp:          " << std::setw(10) << m_tlrt <<
            "\ntracks:       " << std::setw(10) << m_ttracks <<
            "\nOR:           " << std::setw(10) << m_tOR <<
            (m_runSecondStage ? : ("\n2ndStage:           " << std::setw(10) << m_t2ndStage) : "") <<
            "\nmon:          " << std::setw(10) << m_tmon <<
            "\nfin:          " << std::setw(10) << m_tfin
    );
#endif

    if (m_monitor) ATH_CHECK(m_monitorUnionTool->printHistograms(static_cast<HTTMonitorI::PrintDetail>(m_histoPrintDetail)));
    if (m_outputHitTxt) m_outputHitTxtStream.close();

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Helpers

void HTTLogicalHitsProcessAlg::printHitSubregions(std::vector<HTTHit> const & hits)
{
    ATH_MSG_WARNING("Hit regions:");
    for (auto hit : hits)
    {
        std::vector<uint32_t> regions = m_HTTMapping->SubRegionMap()->getRegions(hit);
        std::stringstream ss;
        for (auto r : regions)
            ss << r << ",";
        ATH_MSG_WARNING("\t[" << ss.str() << "]");
    }
}
