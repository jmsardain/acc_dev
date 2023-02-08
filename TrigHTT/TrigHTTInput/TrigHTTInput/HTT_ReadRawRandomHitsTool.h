/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTT_READRAWRANDOMHITSTOOL_H
#define HTT_READRAWRANDOMHITSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTInput/HTTEventInputHeaderToolI.h"

#include "TFile.h"
#include "TTree.h"



class HTT_ReadRawRandomHitsTool : public extends<AthAlgTool, HTTEventInputHeaderToolI>
{
    public:

        HTT_ReadRawRandomHitsTool(const std::string&, const std::string&, const IInterface*);
        virtual ~HTT_ReadRawRandomHitsTool() = default;

        virtual StatusCode initialize() override;
        virtual StatusCode finalize()   override;
        virtual StatusCode readData(HTTEventInputHeader* header, bool &last) override;
        StatusCode readData(HTTEventInputHeader* header, bool &last, bool doReset);
        virtual StatusCode writeData(HTTEventInputHeader* header) override; //not implmeneted yet

    private:
        // JO configuration
        StringProperty m_inpath; // {this, "OutFileName", "httsim_smartwrapper.root", "output path"};

        // Internal pointers
        TFile *m_infile;
        TTree *m_EventTree;
        unsigned int m_nEntries;
        unsigned m_entry = 0;
};

#endif // HTT_READRAWRANDOMHINPUTTOOL_H
