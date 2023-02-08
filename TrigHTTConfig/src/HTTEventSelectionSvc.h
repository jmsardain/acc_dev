/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTEVENTSELECTIONSVC_H
#define HTTEVENTSELECTIONSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigHTTConfig/IHTTEventSelectionSvc.h"
#include "TrigHTTObjects/HTTTrackPars.h"

class HTTHit;
class HTTTrack;
class HTTOfflineTrack;
class HTTTruthTrack;
class HTTRegionSlices;
class HTTEventInputHeader;
class HTTLogicalEventInputHeader;
enum class SampleType;

class HTTEventSelectionSvc : public AthService, virtual public IHTTEventSelectionSvc
{
 public:

  HTTEventSelectionSvc(const std::string& name, ISvcLocator* svc);
  virtual ~HTTEventSelectionSvc() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  static const InterfaceID& interfaceID();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  virtual unsigned getRegionID() const override { return m_regionID; }
  virtual SampleType getSampleType() const override { return m_st; }
  virtual bool checkPU() const override { return m_withPU.value(); }
  virtual const HTTRegionSlices* getRegions() override;
  virtual int getLRTpdgID() const override { return m_LRT_pdgID; }

  virtual HTTTrackPars getMin() const override { return m_min; }
  virtual HTTTrackPars getMax() const override { return m_max; }

  virtual bool passCuts(const HTTHit&) const override;
  virtual bool passCuts(const HTTTrack&) const override;
  virtual bool passCuts(const HTTOfflineTrack&) const override;
  virtual bool passCuts(const HTTTruthTrack&) const override;

  virtual bool passQOverPt(const HTTTrack&) const override;
  virtual bool passEta(const HTTTrack&) const override;
  virtual bool passPhi(const HTTTrack&) const override;
  virtual bool passD0(const HTTTrack&) const override;
  virtual bool passZ0(const HTTTrack&) const override;
  virtual bool passQOverPt(const HTTOfflineTrack&) const override;
  virtual bool passEta(const HTTOfflineTrack&) const override;
  virtual bool passPhi(const HTTOfflineTrack&) const override;
  virtual bool passD0(const HTTOfflineTrack&) const override;
  virtual bool passZ0(const HTTOfflineTrack&) const override;
  
  virtual bool passMatching(HTTTrack const &) const override;
  virtual bool passMatching(HTTTruthTrack const &) const override;

  virtual bool selectEvent(HTTEventInputHeader*) const override;
  virtual bool selectEvent(HTTLogicalEventInputHeader*) const override;

 private:

  unsigned m_regionID = 0;               // Current region of interest
  StringProperty m_regions_path;         // path to slices file
  StringProperty m_sampleType;           // type of sample ("skipTruth", "singleElectrons", "singleMuons", "singlePions")
  BooleanProperty m_withPU;              // flag to say if there is pile-up or not
  BooleanProperty m_LRT = false;         // flag to require cancelling of selections on d0 and z0 in the case of large-radius tracking
  FloatProperty m_minLRTpT = 5.;         // minimum pT, in GeV, to use in LRT selection
  IntegerProperty m_LRT_pdgID = 0;        // If we are running an LLP sample but want only some PDGID of output in the truth selection, set this
  SampleType m_st;                       // internal value for faster comparisons in selectEvent()
  BooleanProperty m_allowHighBarcode = false; // whether or not to allow barcodes over 200000 in truth matching

  HTTTrackPars m_min;                    // min limits of current region
  HTTTrackPars m_max;                    // max limits of current region
  HTTTrackPars m_trackmin;               // min limits of tracks to be accepted in region
  HTTTrackPars m_trackmax;               // max limits of tracks to be accepted in region

  HTTRegionSlices* m_regions = nullptr;  // pointer to RegionSlices class
  void createRegions();                  // helper function to create RegionSlices object

  bool checkTruthTracks(const std::vector<HTTTruthTrack>&) const; // helper function to check the truth tracks for selectEvent()
  bool checkTruthTracksLRT(const std::vector<HTTTruthTrack>&) const; // check the truth tracks for selectEvent() with LRT requirements
};

inline const InterfaceID& HTTEventSelectionSvc::interfaceID()
{
  static const InterfaceID IID_HTTEventSelectionSvc("HTTEventSelectionSvc", 1, 0);
  return IID_HTTEventSelectionSvc;
}

#endif  // TRIGHTTEVENTSELECTIONSVC_H
