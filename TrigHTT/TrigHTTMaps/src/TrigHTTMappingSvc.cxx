// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "TrigHTTMappingSvc.h"
#include "TrigHTTConfig/IHTTEventSelectionSvc.h"

// for callback
//#include "AthenaPoolUtilities/CondAttrListCollection.h"
//#include "AthenaPoolUtilities/AthenaAttributeList.h"

// COOL includes
//#include "CoralBase/Blob.h"


TrigHTTMappingSvc::TrigHTTMappingSvc(const std::string& name, ISvcLocator*svc) :
    AthService(name, svc),
    m_EvtSel("HTTEventSelectionSvc", name)
{
    declareProperty("rmap",                 m_rmap_path,            "path of the region-map file");
    declareProperty("subrmap",              m_subrmap_path,         "path of the region-map file for subregions");
    declareProperty("pmap",                 m_pmap_path,            "path of the PMAP file");
    declareProperty("ssmap",                m_ssmap_path,           "path of the SSMAP DC file ");
    declareProperty("modulemap",            m_modulelut_path,       "path of the ModuleLUT file");
    declareProperty("connection",           m_connmap_path,         "path of the connection file");
    declareProperty("NNmap",                m_NNmap_path,           "path of the NN weighting file");
    declareProperty("mappingType",          m_mappingType,          "can be FILE or COOL?");
    declareProperty("layerOverride",        m_layerOverrides,       "Overrides the selection of the 1st stage logical layers in the plane map. Each entry declares a detector layer to use as a logical layer. Specify a detector layer with { SilictonTech * 1000 + DetectorZone * 100 + PhysicalLayer }");

}


StatusCode TrigHTTMappingSvc::queryInterface(const InterfaceID& riid, void** ppvIf)
{
    if (interfaceID() == riid)
        *ppvIf = dynamic_cast< TrigHTTMappingSvc* > (this);
    else if (ITrigHTTMappingSvc::interfaceID() == riid)
        *ppvIf = dynamic_cast<ITrigHTTMappingSvc*> (this);
    else
        return AthService::queryInterface(riid, ppvIf);

    addRef();
    return StatusCode::SUCCESS;
}


StatusCode TrigHTTMappingSvc::checkInputs()
{
    if (m_pmap_path.value().empty())
        ATH_MSG_FATAL("Main plane map definition missing");
    else if (m_rmap_path.value().empty())
        ATH_MSG_FATAL("Missing region map path");
    else if (m_modulelut_path.value().empty())
        ATH_MSG_FATAL("Module LUT file is missing");
    else if (m_ssmap_path.value().empty())
        ATH_MSG_FATAL("Missing superstrip map path");
    else if (m_connmap_path.value().empty())
      ATH_MSG_FATAL("Missing connection file path");
    else if (m_NNmap_path.value().empty())
      ATH_MSG_FATAL("Missing NN weighting file path");
    else
        return StatusCode::SUCCESS;

    return StatusCode::FAILURE;
}


StatusCode TrigHTTMappingSvc::checkAllocs()
{
    if (!m_pmap_1st)
        ATH_MSG_FATAL("Error using 1st stage plane map: " << m_pmap_path);
    if (!m_pmap_2nd)
        ATH_MSG_FATAL("Error using 2nd stage plane map: " << m_pmap_path);
    if (!m_rmap_1st)
        ATH_MSG_FATAL("Error creating region map for 1st stage from: " << m_rmap_path);
    if (!m_rmap_2nd)
        ATH_MSG_FATAL("Error creating region map for 2nd stage from: " << m_rmap_path);
    if (!m_subrmap)
        ATH_MSG_FATAL("Error creating sub-region map from: " << m_subrmap_path);
    if (!m_ssmap)
        ATH_MSG_FATAL("Error creating superstrip map from: " << m_ssmap_path);
    if (!m_connmap)
      ATH_MSG_FATAL("Error creating connection map from : " << m_connmap_path);
    if (!m_NNmap)
      ATH_MSG_FATAL("Error creating NN map from : " << m_NNmap_path);

    return StatusCode::SUCCESS;
}


StatusCode TrigHTTMappingSvc::initialize()
{
    ATH_CHECK(m_EvtSel.retrieve());
    ATH_CHECK(checkInputs());

    if (m_mappingType.value() == "FILE")
    {
        ATH_MSG_DEBUG("Creating the 1st stage plane map");
        MsgStream p1msg(msgSvc(), "HTTPlaneMap_1st");
        p1msg.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function
        m_pmap_1st = new HTTPlaneMap(p1msg, m_pmap_path.value(), m_EvtSel->getRegionID(), 1, m_layerOverrides);

        ATH_MSG_DEBUG("Creating the 2nd stage plane map");
        MsgStream p2msg(msgSvc(), "HTTPlaneMap_2nd");
        p2msg.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function
        m_pmap_2nd = new HTTPlaneMap(p2msg, m_pmap_path.value(), m_EvtSel->getRegionID(), 2);

        ATH_MSG_DEBUG("Creating the 1st stage region map");
        MsgStream r1msg(msgSvc(), "HTTRegionMap_1st");
        r1msg.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function
        m_rmap_1st = new HTTRegionMap(r1msg, m_pmap_1st, m_rmap_path.value());

        ATH_MSG_DEBUG("Creating the 2nd stage region map");
        MsgStream r2msg(msgSvc(), "HTTRegionMap_2nd");
        r2msg.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function
        m_rmap_2nd = new HTTRegionMap(r2msg, m_pmap_2nd, m_rmap_path.value());

        ATH_MSG_DEBUG("Creating the sub-region map");
        MsgStream smsg(msgSvc(), "HTTSubRegionMap");
        smsg.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function
        m_subrmap = new HTTRegionMap(smsg, m_pmap_1st, m_subrmap_path.value());

        ATH_MSG_DEBUG("Setting the Module LUT for Region Maps");
        m_rmap_1st->loadModuleIDLUT(m_modulelut_path.value().c_str());
        m_rmap_2nd->loadModuleIDLUT(m_modulelut_path.value().c_str());

        ATH_MSG_DEBUG("Creating the SS Map");
        MsgStream ssmsg(msgSvc(), "HTTSSMap");
        ssmsg.setLevel(msg().level()); // cause AthMessaging is stupid and doesn't have this function
        m_ssmap = new HTTSSMap(ssmsg, m_rmap_1st, m_ssmap_path.value());

	ATH_MSG_DEBUG("Creating first to second stage connection map");
	MsgStream connmsg(msgSvc(), "HTTConnectionMap");
	connmsg.setLevel(msg().level());
	m_connmap = new HTTConnectionMap(connmsg, m_connmap_path.value());

	ATH_MSG_DEBUG("Creating NN weighting map");
	MsgStream NNmsg(msgSvc(), "HTTNNMap");
	NNmsg.setLevel(msg().level());
	m_NNmap = new HTTNNMap(NNmsg, m_NNmap_path.value());
    }

    ATH_CHECK(checkAllocs());

    /*
       if (m_mappingType!="COOL") {
       if (!m_cablingTool->fillMapFromFile(m_final_mapping_file,m_cabling.get())) {
       ATH_MSG_ERROR("Filling pixel cabling from file \"" << m_final_mapping_file << "\" failed");
       return StatusCode::FAILURE;
       }
       }
       */


    // Register readout speed callback
    // const DataHandle<AthenaAttributeList> attrlist;
    // if (m_detStore->contains<AthenaAttributeList>(m_key)) {
    //   CHECK(m_detStore->regFcn(&ITrigHTTMappingSvc::IOVCallBack,dynamic_cast<ITrigHTTMappingSvc*>(this),attrlist,m_key));
    // }
    // else {
    //   ATH_MSG_WARNING("Folder " << m_key << " not found, using default readoutspeed" << " values (all modules at SINGLE_40)");
    // }



    return StatusCode::SUCCESS;
}


StatusCode TrigHTTMappingSvc::finalize()
{
    if (m_pmap_1st) delete m_pmap_1st;
    if (m_pmap_2nd) delete m_pmap_2nd;
    if (m_rmap_1st) delete m_rmap_1st;
    if (m_rmap_2nd) delete m_rmap_2nd;
    if (m_ssmap)    delete m_ssmap;
    if (m_subrmap)  delete m_subrmap;
    if (m_connmap)  delete m_connmap;
    if (m_NNmap)    delete m_NNmap;

    return StatusCode::SUCCESS;
}


// StatusCode TrigHTTMappingSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys)) {

//   for (std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key) {
//     ATH_MSG_INFO("IOVCALLBACK for key " << *key << " number " << I);
//   }

//   // Clear all the existing map content
//   //m_cabling->clear_all_maps();

//   // const AthenaAttributeList* attrlist = 0;

//   // CHECK(m_detStore->retrieve(attrlist,m_key));

//   // if (msgLvl(MSG::DEBUG)) {
//   //   ATH_MSG_DEBUG("AthenaAttributeList for " << m_key << ":");
//   //   attrlist->print(std::cout);
//   //   ATH_MSG_DEBUG("");
//   // }


//   return StatusCode::SUCCESS;
// }

