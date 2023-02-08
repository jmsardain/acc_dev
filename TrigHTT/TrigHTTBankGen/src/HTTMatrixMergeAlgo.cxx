// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTMatrixMergeAlgo.cxx
 * @author Rewrite by Riley Xu - riley.xu@cern.ch after FTK code
 * @date May 8th, 2020
 * @brief Algorithm to merge matrix files, to be used for sector and constant generation.
 *
 * See header.
 */


#include "TrigHTTBankGen/HTTMatrixMergeAlgo.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Constructor / Initialize
///////////////////////////////////////////////////////////////////////////////

HTTMatrixMergeAlgo::HTTMatrixMergeAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_tHistSvc("THistSvc", name)
{
    declareProperty("file_path",m_fpath,"array of merging files");
    declareProperty("allregion",m_allregion);
    declareProperty("region",m_region);
    declareProperty("nbank",m_nRegions);
    declareProperty("Monitor", m_Monitor=false, "flag to enable the monitor");
}


StatusCode HTTMatrixMergeAlgo::initialize()
{
    ATH_MSG_DEBUG("initialize()");

    ATH_CHECK(m_tHistSvc.retrieve());
    if (m_Monitor) ATH_CHECK(bookHistograms());

    m_nFiles = m_fpath.size();
    ATH_MSG_INFO("Merging " << m_nFiles << " files");

    // Use the first valid file to extract some general information used to intialize
    TFile *file = nullptr;
    for (int iFile = 0; iFile < m_nFiles; iFile++)
    {
        file = TFile::Open(m_fpath[iFile].c_str());
        if (!file)
            ATH_MSG_WARNING("Could not open " << m_fpath[iFile]);
        else
        {
            if (file->GetNkeys() != 0) break;
            else file->Close();
        }
    }
    if (!file) return StatusCode::FAILURE;

    // Get the number of layers and total dimension from a sample region
    TTree *matrix_tree = (TTree*)file->Get(Form("am%d",m_region));
    matrix_tree->SetBranchAddress("nplanes", &m_nLayers);
    matrix_tree->SetBranchAddress("ndim", &m_nDim);
    matrix_tree->SetBranchAddress("ndim2", &m_nDim2);
    matrix_tree->GetEntry(0);

    // Copy the slice tree into the merged file
    ATH_CHECK(copySliceTree(file));

    file->Close();

    // Setup the boundaries for the merge
    if (m_allregion)
    {
        m_region_start = 0;
        m_region_end = m_nRegions;
    }
    else
    {
        m_region_start = m_region;
        m_region_end = m_region + 1;
    }

    m_sector_cum.resize(m_nRegions);


    readFiles();
    return StatusCode::SUCCESS;
}


StatusCode HTTMatrixMergeAlgo::bookHistograms()
{
    h_nSector = new TH1I("h_nSector", "number of sectors in merged file", 100, 0, 10000);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_nSector",h_nSector));

    h_nHit = new TH1I("h_nHit", "number of hits in sector", 100, 0, 10000);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_nHit",h_nHit));

    h_c = new TH1F("h_c", "Truth curvature in sector",100,-1e-8,1e-8);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_c",h_c));

    h_d = new TH1F("h_d", "Truth d0 in sector",100,-2.5,2.5);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_d",h_d));

    h_phi = new TH1F("h_phi", "Truth phi in sector",100,0,6.3);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_phi",h_phi));

    h_coto = new TH1F("h_coto", "Truth coto in sector",100,-4,4);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_coto",h_coto));

    h_z = new TH1F("h_z", "Truth z in sector",100,-200,200);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_z",h_z));

    return StatusCode::SUCCESS;
}


StatusCode HTTMatrixMergeAlgo::copySliceTree(TFile *file)
{
    // Is there a better way to do this? Idk how the interface to tHistSvc works
    HTTTrackPars max;
    HTTTrackPars min;
    HTTTrackParsI nBins;

    // Read old tree
    TTree *old_tree = (TTree*)file->Get("slice");
    old_tree->SetBranchAddress("c_max", &max.qOverPt);
    old_tree->SetBranchAddress("c_min", &min.qOverPt);
    old_tree->SetBranchAddress("c_slices", &nBins.qOverPt);

    old_tree->SetBranchAddress("phi_max", &max.phi);
    old_tree->SetBranchAddress("phi_min", &min.phi);
    old_tree->SetBranchAddress("phi_slices", &nBins.phi);

    old_tree->SetBranchAddress("d0_max", &max.d0);
    old_tree->SetBranchAddress("d0_min", &min.d0);
    old_tree->SetBranchAddress("d0_slices", &nBins.d0);

    old_tree->SetBranchAddress("z0_max", &max.z0);
    old_tree->SetBranchAddress("z0_min", &min.z0);
    old_tree->SetBranchAddress("z0_slices", &nBins.z0);

    old_tree->SetBranchAddress("eta_max", &max.eta);
    old_tree->SetBranchAddress("eta_min", &min.eta);
    old_tree->SetBranchAddress("eta_slices", &nBins.eta);

    old_tree->GetEntry(0);

    // Write new tree
    TTree *new_tree = new TTree("slice", "Slice boundaries");
    ATH_CHECK(m_tHistSvc->regTree("/TRIGHTTMATRIXOUT/slice", new_tree));

    new_tree->Branch("c_max", &max.qOverPt);
    new_tree->Branch("c_min", &min.qOverPt);
    new_tree->Branch("c_slices", &nBins.qOverPt);

    new_tree->Branch("phi_max", &max.phi);
    new_tree->Branch("phi_min", &min.phi);
    new_tree->Branch("phi_slices", &nBins.phi);

    new_tree->Branch("d0_max", &max.d0);
    new_tree->Branch("d0_min", &min.d0);
    new_tree->Branch("d0_slices", &nBins.d0);

    new_tree->Branch("z0_max", &max.z0);
    new_tree->Branch("z0_min", &min.z0);
    new_tree->Branch("z0_slices", &nBins.z0);

    new_tree->Branch("eta_max", &max.eta);
    new_tree->Branch("eta_min", &min.eta);
    new_tree->Branch("eta_slices", &nBins.eta);

    new_tree->Fill();

    return StatusCode::SUCCESS;
}


// Read each file and accumulate everything into m_sector_cum
void HTTMatrixMergeAlgo::readFiles()
{
  ///  TDirectory *dir = gROOT;
    for (int iFile = 0; iFile < m_nFiles; iFile++)
    {
        // Open the file
        const string &fpath = m_fpath[iFile];
        ATH_MSG_INFO("Reading " << fpath);
        TFile *file = TFile::Open(fpath.c_str());
	//        gROOT->cd();

        // Check if the file is valid
        if (!file || file->GetNkeys() == 0)
        {
            ATH_MSG_WARNING("Invalid file: " << fpath);
            continue;
        }

        // Read each tree
        for (int region = m_region_start; region < m_region_end; region++)
        {
            TTree *tree = (TTree*)file->Get(Form("am%d", region));
            ::readTree(m_sector_cum[region], tree, m_nLayers, m_nDim);
            delete tree; // Are we suppossed to be deleting trees opened by ROOT?
        }

        file->Close();
        delete file; // Are we suppossed to be deleting files opened by ROOT?
    }
    //    dir->cd();
}


///////////////////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////////////////


StatusCode HTTMatrixMergeAlgo::execute()
{
    // Do nothing; this class does not process events. The main algorithm is
    // called in initialize() and finalize().
    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTMatrixMergeAlgo::finalize()
{
    for (int region = m_region_start; region < m_region_end; region++)
    {
      std::cerr << "JAAA region = " << region << " and start = " << m_region_start << " and end = " << m_region_end << std::endl;
        // Create the tree
      std::stringstream name;
      std::stringstream title;
      name << "am" << region;
      title << "Ambank " << region << " parameters";
      TTree* tree = new TTree(name.str().c_str(), title.str().c_str());
      ATH_CHECK(m_tHistSvc->regTree(Form("/TRIGHTTMATRIXOUT/%s",tree->GetName()), tree));
      
      // JAA
      ATH_MSG_INFO("Sectors found in region " << region << ": " << m_sector_cum[region].size());
      
      
      // Fill the tree
      ::fillTree(m_sector_cum[region], tree, m_nLayers, m_nDim);
      
      // Monitoring
      if (m_Monitor)
        {
	  ATH_MSG_INFO("Sectors found in region " << region << ": " << m_sector_cum[region].size());
            h_nSector->Fill(m_sector_cum[region].size());
            for (auto & sector_info : m_sector_cum[region])
            {
                double coverage = sector_info.second.track_bins.size();
                h_nHit->Fill(coverage);
                h_c->Fill(sector_info.second.pars.qOverPt / coverage);
                h_d->Fill(sector_info.second.pars.d0 / coverage);
                h_phi->Fill(sector_info.second.pars.phi / coverage);
                h_coto->Fill(sector_info.second.pars.eta / coverage);
                h_z->Fill(sector_info.second.pars.z0 / coverage);
            }
        }
    }

    ATH_CHECK(m_tHistSvc->finalize());
    return StatusCode::SUCCESS;
}
