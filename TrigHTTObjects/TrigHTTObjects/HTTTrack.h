/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTSIM_HTTTRACK_H
#define TRIGHTTSIM_HTTTRACK_H

#include "HTTHit.h"
#include "HTTMultiTruth.h"
#include "HTTTrackPars.h"
#include <vector>
#include <TObject.h>
#include <TMath.h>

class HTTTrack : public TObject {

public:
  HTTTrack();
  virtual ~HTTTrack();

  TrackCorrType getTrackCorrType() const { return m_trackCorrType; }
  TrackStage getTrackStage() const { return m_trackStage; }
  bool getDoDeltaGPhis() const { return m_doDeltaGPhis; }
  int getBankID() const { return m_bankID; }
  int getRegion() const { return m_bankID%100; }
  int getPatternID() const { return m_patternID; }
  int getFirstSectorID() const { return m_firstSectorID; }
  int getSecondSectorID() const { return m_secondSectorID; }
  int getTrackID() const { return m_trackID; }
  float getParameter(int) const;
  float getHoughX() const { return m_houghX; }
  float getHoughY() const { return m_houghY; }
  float getQOverPt() const { return m_qoverpt; }
  float getPt() const { return m_qoverpt != 0 ? abs(1/m_qoverpt) : 99999999.; }
  float getD0() const { return m_d0; }
  float getPhi() const { return m_phi; }
  float getZ0() const { return m_z0; }
  float getEta() const { return m_eta; }
  float getChi2() const { return m_chi2; }
  float getOrigChi2() const { return m_origchi2; }
  float getChi2ndof() const { return m_chi2/(getNCoords()-m_nmissing-5); }
  float getOrigChi2ndof() const { return m_origchi2/(getNCoords()-m_nmissing-5); }

  /* float getChi2ndof() const { return m_chi2/(getNCoords()-m_nmissing-0); } */
  /* float getOrigChi2ndof() const { return m_origchi2/(getNCoords()-m_nmissing-0); } */

  int   getNMissing() const { return m_nmissing; } // missing coordinates
  unsigned int getTypeMask() const { return m_typemask; }
  unsigned int getHitMap() const { return m_hitmap; } // coordinate mask!!
  //write a detmap
  int getNCoords() const;
  signed long getEventIndex() const { return m_eventindex; }
  signed long getBarcode() const { return m_barcode; }
  float getBarcodeFrac() const { return m_barcode_frac; }
  //Should be passed as const ref to avoid excessive copying.
  const std::vector <HTTHit>& getHTTHits() const { return m_hits; }
  std::vector<float> getCoords(unsigned ilayer) const;
  // helper function to calculate coordinates for the methods based in idealized detector geometry. See https://cds.cern.ch/record/2633242
  // in the delta global phis method, the coordinates are the ideal z, and the delta global phis.
  // the delta global phis are the difference between the global phi of the input hit transformed to the ideal detector layer (this is hitGPhi)
  // and the global phi of the Hough Transform "track" at the ideal layers radius (target_r, R') (this is expectedGPhi)
  // expectedGPhi = phi_0 (from HT) - R' * A*q/pT - ((R' * A*q/pT)^3) / 6
  // hitGPhi = GPhi (from hit) + (R - R') * A*q/pT + ((R * A*q/pT)^3) / 6
  std::vector<float> computeIdealCoords(unsigned ilayer) const;
  //Has some size protections
  float getEtaCoord(int ilayer) const;
  float getPhiCoord(int ilayer) const;

  void setTrackCorrType(TrackCorrType v) { m_trackCorrType = v;}
  void setTrackStage(TrackStage v) { m_trackStage = v; }
  void setDoDeltaGPhis(bool v) { m_doDeltaGPhis = v; }
  void setBankID(int v) { m_bankID = v; }
  void setPatternID(int v) { m_patternID = v; }
  void setFirstSectorID(int v) { m_firstSectorID = v; }
  void setSecondSectorID(int v) { m_secondSectorID = v; }
  void setTrackID(int v) { m_trackID = v; }
  void setParameter(int,float);
  void setHoughX(float v) { m_houghX = v; }
  void setHoughY(float v) { m_houghY = v; }
  void setQOverPt(float v) { m_qoverpt = v; }
  void setD0(float v) { m_d0 = v; }
  void setPhi(float v,bool ForceRange=true);
  void setZ0(float v) {m_z0 = v;}
  void setEta(float v) { m_eta = v; }
  void setChi2(float v) { m_chi2 = v; }
  void setOrigChi2(float v) { m_origchi2 = v; }
  void setNMissing(int v) { m_nmissing = v; }
  void setTypeMask(unsigned int v) { m_typemask = v; }
  void setHitMap(unsigned int v) { m_hitmap = v; }
  void setEventIndex( const signed long& v ) { m_eventindex = v; }
  void setBarcode( const signed long& v ) { m_barcode = v; }
  void setBarcodeFrac(const float& v ) { m_barcode_frac = v; }

  void calculateTruth(); // this will calculate the above quantities based on the hits
  void setNLayers(int); //Reset/resize the track hits vector
  void setHTTHit(unsigned i,const HTTHit &hit);
  void setPars(HTTTrackPars const & pars)
  {
      setQOverPt(pars.qOverPt);
      setPhi(pars.phi, false);
      setEta(pars.eta);
      setD0(pars.d0);
      setZ0(pars.z0);
  }

  // Functions for overlap removal
  unsigned int passedOR() const {return m_ORcode;}
  void setPassedOR(unsigned int);

  virtual void Print(Option_t * opts="") const;

  friend std::ostream& operator<<(std::ostream&,const HTTTrack&);

private:

  TrackCorrType m_trackCorrType; // type of correction to make for track coordinates
  TrackStage m_trackStage; // Is this a 1st stage or second stage track?
  bool m_doDeltaGPhis; // if it uses the delta phis method for fitting

  int m_bankID; // Bank ID of the road related to this track
  int m_patternID; // TODO add documentation
  int m_firstSectorID; // Sector identifier in the first stage tracking
  int m_secondSectorID; // Sector identifier in thesecond stage tracking
  int m_trackID; // Unique track ID in this bank

  int m_IdealGeoCorr; // 

  float m_houghX; // phi0 from HTTRoad_Hough
  float m_houghY; // QOverPt from HTTRoad_Hough
  float m_qoverpt; // charge over pT
  float m_d0; // impact paramter in the ATLAS reference system
  float m_phi; // phi of the track
  float m_z0; // z0 in standard ATLAS reference system
  float m_eta; // eta of the track
  float m_chi2; // chi2 of the track
  float m_origchi2; // In the case of majority recovery, this is the chi2 of

  //TODO: Switch to matchedhits mask
  unsigned int m_nmissing; // number of missing coordinates
  unsigned int m_typemask; // set on in bits related to the step recovery were used, ex.: 0 no recovery, 01, rec 1st step, 11, recovery in the 1st and the 2nd stage
  unsigned int m_hitmap;

  std::vector<HTTHit> m_hits; //[m_nlayers] hits associated to the track

  signed long m_eventindex; // matched particle event index
  signed long m_barcode; // matched geant particle barcode
  float m_barcode_frac; // largest "matching fraction" with any "good"
                        // geant particle, corresponding to the
                        // particle with m_barcode

  // Overlap removal member
  // There is currently only one algorithm
  unsigned int m_ORcode; // Each digit should represent pass/fail(1/0) result from a specific OR algorithm

  ClassDef(HTTTrack,2)
};



#endif // TRIGHTTSIM_HTTTRACK_H
