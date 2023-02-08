// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef TRIGHTTMAPPINGSVC_H
#define TRIGHTTMAPPINGSVC_H

#include "AthenaBaseComps/AthService.h"

#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTMaps/HTTSSMap.h"
#include "TrigHTTMaps/HTTConnectionMap.h"
#include "TrigHTTMaps/HTTNNMap.h"

class IHTTEventSelectionSvc;

class TrigHTTMappingSvc : public AthService, virtual public ITrigHTTMappingSvc
{
    public:

        TrigHTTMappingSvc(const std::string& name, ISvcLocator* svc);
        virtual ~TrigHTTMappingSvc() = default;

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        virtual const HTTPlaneMap* PlaneMap_1st()       const override { return m_pmap_1st; }
        virtual const HTTPlaneMap* PlaneMap_2nd()       const override { return m_pmap_2nd; }
        virtual const HTTRegionMap* RegionMap_1st()     const override { return m_rmap_1st; }
        virtual const HTTRegionMap* RegionMap_2nd()     const override { return m_rmap_2nd; }
        virtual const HTTRegionMap* SubRegionMap()      const override { return m_subrmap;  }
        virtual const HTTSSMap* SSMap()                 const override { return m_ssmap;    }
        virtual const HTTConnectionMap* ConnectionMap() const override { return m_connmap;  }
        virtual const HTTNNMap* NNMap()                 const override { return m_NNmap;    }

        static const InterfaceID& interfaceID();
        virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

    private:

        // Handles
        ServiceHandle<IHTTEventSelectionSvc>  m_EvtSel;

        // Configuration
        std::vector<int> m_layerOverrides; // See same variable in HTTPlaneMap

        // Filepaths
        StringProperty  m_mappingType = std::string("FILE");
        StringProperty  m_pmap_path;//  {this, "PMapPath", ".",  "path of the PMAP file"};
        StringProperty  m_rmap_path;//  {this, "RMapPath", ".",  "path of the region-map file"};
        StringProperty  m_ssmap_path;//  {this, "SSMapDCPath", ".",  "path of the SSMap DC file"};
        StringProperty  m_modulelut_path;//  {this, "ModuleLUTPath", ".",  "path of the ModuleLUT File");
        StringProperty  m_subrmap_path;
        StringProperty  m_connmap_path;
        StringProperty  m_NNmap_path; // used for storing NN weights (not really a map)

        // Map pointers
        HTTPlaneMap  *m_pmap_1st = nullptr; //  pointer to the pmap object for 1st stage
        HTTPlaneMap  *m_pmap_2nd = nullptr; //  pointer to the pmap object for 2nd stage
        HTTRegionMap *m_rmap_1st = nullptr; //  pointer to the RMAP object using 1st stage plane map
        HTTRegionMap *m_rmap_2nd = nullptr; //  pointer to the RMAP object using 2nd stage plane map
        HTTSSMap     *m_ssmap = nullptr;
        HTTRegionMap *m_subrmap = nullptr;
        HTTConnectionMap *m_connmap = nullptr;
        HTTNNMap *m_NNmap = nullptr;

        // Helpers
        StatusCode checkInputs();
        StatusCode checkAllocs();
};

inline const InterfaceID& TrigHTTMappingSvc::interfaceID()
{
    static const InterfaceID IID_TrigHTTMappingSvc("TrigHTTMappingSvc", 1, 0);
    return IID_TrigHTTMappingSvc;
}

#endif   // TRIGHTTMAPPINGSVC_H
