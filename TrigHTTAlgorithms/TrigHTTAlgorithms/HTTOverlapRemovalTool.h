/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file TrigHTTAlgorithms/HTTOverlapRemovalTool.h
* @author Zhaoyuan.Cui@cern.ch
* @date Dec.4, 2020
* @brief Overlap removal tool for HTTTrack.
*/

#ifndef HTTOVERLAPREMOVALTOOL_H
#define HTTOVERLAPREMOVALTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"

#include <string>
#include <vector>
#include <ostream>

enum class ORAlgo {Normal, InvertGrouping};
/**
* @class HTTOverlapRemovalTool
* @brief Remove (mark) duplicate tracks
* This tool takes HTTTrack as input and mark their status of passing/failing the overlap removal criteria.
*/
class HTTOverlapRemovalTool: public AthAlgTool {
public:
  HTTOverlapRemovalTool (const std::string&, const std::string&, const IInterface*);
  ~HTTOverlapRemovalTool() {}

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode finalize();

  StatusCode runOverlapRemoval(std::vector<HTTRoad*>& roads);

  // Original Overlap Removal function
  // Compare chi2 and common number of hits
  StatusCode runOverlapRemoval(std::vector<HTTTrack>& tracks);

  void setDoSecondStage(bool v) { m_do2ndStage = v; }
  bool getDoSecondStage() const { return m_do2ndStage; }
  ORAlgo getAlgorithm() const {return m_algo;}
  //  Find the one track from the duplicate group with the minium Chi2 and maximum # of hits
  void findMinChi2MaxHit(std::vector<int> duplicates, std::vector<HTTTrack>& tracks);
  //  Find number of common hits between two tracks
  int findNCommonHits(const HTTTrack&, const HTTTrack&);
  int findNonOverlapHits(const HTTTrack&, const HTTTrack&);

  StatusCode removeOverlapping(HTTTrack & track1, HTTTrack & track2);
  static bool compareTrackQuality(const HTTTrack & track1, const HTTTrack & track2);

private:
  bool m_do2ndStage;                //  Flag to change to second stage
  int m_NumOfHitPerGrouping;        //  Number of hits used in grouping, for both Normal and Invert
  int m_totLayers;                  //  Total number of layers used for a track
  float m_minChi2;                  //  Minimum chi2 requirement for tracks being considered in OR
  StringProperty m_algorithm;       //  Overlap removal algorithm
  BooleanProperty m_roadSliceOR;    //  Do road OR across slices
  ORAlgo m_algo;                    //  Internal ORAlgo enum for faster compare
  bool m_doFastOR;                  //  Use fast OR algorithm instead of default

    // Hough
  IntegerProperty m_localMaxWindowSize = 0; // Only create roads from a local maximum
  unsigned m_imageSize_x = 0; // i.e. number of bins in phi_track
  unsigned m_imageSize_y = 0; // i.e. number of bins in q/pT

  StatusCode runOverlapRemoval_fast(std::vector<HTTTrack>& tracks);

  //  ServiceHandle
  ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;   //  Get the number of layer through map

};

#endif // HTTOverlapRemovalTool_h
