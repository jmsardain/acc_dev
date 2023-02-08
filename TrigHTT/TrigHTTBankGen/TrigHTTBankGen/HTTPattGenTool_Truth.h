#ifndef HTTPattGenTool_Truth_h
#define HTTPattGenTool_Truth_h

/**
 * @file HTTPattGenTool_Truth.h
 * @date August 8th, 2019
 * @author Riley Xu - rixu@cern.ch
 * @brief This file declares a class that generates patterns from truth tracks.
 *
 * Declarations in this file:
 *      class HTTPattGenTool_Truth : public HTTPattGenToolI
 */

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "TrigHTTUtils/HTTTypes.h"
#include "TrigHTTBankGen/HTTPattGenToolI.h"
#include "TrigHTTInput/HTTEventInputHeaderToolI.h"
#include "TrigHTTInput/HTT_RawToLogicalHitsTool.h"

class TFile;
class TTree;
class HTTHit;
class HTTLogicalEventInputHeader;


/* HTTPattGenTool_Truth
 *
 * See HTTPattGenToolI for basic structure and objective.
 *
 * This class creates patterns using an HTT wrapper file containing hits
 * from a single track, i.e. single muons. The patterns are created in the
 * straight-forward way: taking the SSIDs of the hits as the SSIDs of the pattern.
 *
 * If there are multiple hits in a layer, this class chooses the first hit as
 * stored in the wrapper file (TODO: study alternatives).
 *
 * If there are missing hits from a layer, this class fills it with a wildcard.
 *
 * Technically, one could create patterns without needing to know sectors.
 * However, the file formats and pattern matching all rely on sorting by sector.
 * So this class must also do a sector lookup for the created patterns.
 *
 * Usage: See HTTPattGenToolI.h. The "truth" property locating the truth wrapper file
 * must also be set.
 */
class HTTPattGenTool_Truth : public HTTPattGenToolI
{
    public:

        HTTPattGenTool_Truth(const std::string&, const std::string&, const IInterface*);
        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        ///////////////////////////////////////////////////////////////////////
        // Monitoring (these return per event/call to next() info)

        size_t getNLayersMissed_truth() const { return m_nLayersMissed_truth; }

    private:


        ///////////////////////////////////////////////////////////////////////
        // Parameters/Configuration

        bool m_rawInput = false; // whether truth file contains raw hits or logical hits
        std::string m_truthPath;

        ///////////////////////////////////////////////////////////////////////
        // Objects

        // Handles
        ToolHandle<HTTEventInputHeaderToolI> m_hitInputTool;
        ToolHandle<HTT_RawToLogicalHitsTool> m_hitMapTool;// { this, "MapTool", "HTT_SGHitInput/HTT_SGHitInput", "HitInput Tool" };

        // Truth file
        std::unique_ptr<TFile> m_truthFile;
        TTree *m_eventTree; // ROOT owns the object
        HTTLogicalEventInputHeader* m_header;

        ///////////////////////////////////////////////////////////////////////
        // Monitoring

        size_t m_nLayersMissed_truth = 0;

        ///////////////////////////////////////////////////////////////////////
        // Core Functions

        // Returns patterns with the minimum number of layers missed, out of
        // all patterns found. This number of layers missed is also returned
        // by reference.
        ErrorStatus nextCandidates(std::vector<HTTPattern> & pattCands, size_t & layersMissed);

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        std::vector<HTTPattern> matchSectors(pattern_t const & patt,
                std::vector<module_t> const & modules, size_t & layersMissed) const;
        bool checkAllowedWCs(pattern_t patt) const;
};

#endif
