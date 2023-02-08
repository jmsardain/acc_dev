// ------------------- //
// --- DESCRIPTION --- //
// ------------------- //

// Code to test the output xAOD containing the HTT tracks
// Produces plots comparing the track parameters (d0,z0,phi,eta,pt) distributions for offline and HTT tracks

// To avoid writing a full EventLoop or AthAnalysis code we use MakeTransientTree to quickly analyse CollectionTree as if it were a flat ntuple

// -----------------------------//
// --- RUNNING INSTRUCTIONS --- //
// ---------------------------- //
// - Set up and AnalysisBase release
// - Start root with: root -l $ROOTCOREDIR/scripts/load_packages.C
// - Load and run macro as usual
// - Takes as input the output root xAOD file create by HTTFastSim. It assumes it is inside the run directory

void testOutputXAOD(){

  gROOT->LoadMacro("/afs/cern.ch/work/a/amoreira/AtlasStyle.h");
  gROOT->LoadMacro("/afs/cern.ch/work/a/amoreira/AtlasStyle.C");
  gROOT->ProcessLine("SetAtlasStyle()");
  
  //THtml::LoadAllLibs();
  xAOD::Init();
  
  // Output file with collection of htt tracks (smearing applied)
  TFile *fout = TFile::Open("../run/myXAOD.pool.root");
  TTree *tout = xAOD::MakeTransientTree(fout);

  // Create histograms
  // Before smearing
  TH1F *h_d0 = new TH1F("h_d0","h_d0",100,-2,2);
  TH1F *h_z0 = new TH1F("h_z0","h_z0",100,-300,300);
  TH1F *h_phi = new TH1F("h_phi","h_phi",100,-4,4);
  TH1F *h_eta = new TH1F("h_eta","h_eta",100,-5,5);
  TH1F *h_pt = new TH1F("h_pt","h_pt",100,0,100);
  // After smearing
  TH1F *h_d0_smear = new TH1F("h_d0_smear","h_d0_smear",100,-2,2);
  TH1F *h_z0_smear = new TH1F("h_z0_smear","h_z0_smear",100,-300,300);
  TH1F *h_phi_smear = new TH1F("h_phi_smear","h_phi_smear",100,-4,4);
  TH1F *h_eta_smear = new TH1F("h_eta_smear","h_eta_smear",100,-5,5);
  TH1F *h_pt_smear = new TH1F("h_pt_smear","h_pt_smear",100,0,100);

  // Get histograms from trees
  // Before smearing
  tout->Draw("OffTracks.d0()>>h_d0","OffTracks.pt()*0.001>=4","goff");
  tout->Draw("OffTracks.z0()>>h_z0","OffTracks.pt()*0.001>=4","goff");
  tout->Draw("OffTracks.phi()>>h_phi","OffTracks.pt()*0.001>=4","goff");
  tout->Draw("OffTracks.eta()>>h_eta","OffTracks.pt()*0.001>=4","goff");
  tout->Draw("OffTracks.pt()*0.001>>h_pt","OffTracks.pt()*0.001>=4","goff");

  h_d0->SetLineWidth(2);
  h_z0->SetLineWidth(2);
  h_phi->SetLineWidth(2);
  h_eta->SetLineWidth(2);
  h_pt->SetLineWidth(2);
  
  // After smearing
  tout->Draw("httTracks.d0()>>h_d0_smear","","goff");
  tout->Draw("httTracks.z0()>>h_z0_smear","","goff");
  tout->Draw("httTracks.phi()>>h_phi_smear","","goff");
  tout->Draw("httTracks.eta()>>h_eta_smear","","goff");
  tout->Draw("httTracks.pt()*0.001>>h_pt_smear","","goff");
  
  h_d0_smear->SetLineColor(kRed);
  h_d0_smear->SetLineWidth(2);

  h_z0_smear->SetLineColor(kRed);
  h_z0_smear->SetLineWidth(2);

  h_phi_smear->SetLineColor(kRed);
  h_phi_smear->SetLineWidth(2);

  h_eta_smear->SetLineColor(kRed);
  h_eta_smear->SetLineWidth(2);

  h_pt_smear->SetLineColor(kRed);
  h_pt_smear->SetLineWidth(2);

  // Scale histos to same area, because we have less HTT tracks, since we are only smearing tracks with pt>4 GeV
  h_d0->Scale(1/(h_d0->Integral()));
  h_z0->Scale(1/(h_z0->Integral()));
  h_phi->Scale(1/(h_phi->Integral()));
  h_eta->Scale(1/(h_eta->Integral()));
  h_pt->Scale(1/(h_pt->Integral()));

  h_d0_smear->Scale(1/(h_d0_smear->Integral()));
  h_z0_smear->Scale(1/(h_z0_smear->Integral()));
  h_phi_smear->Scale(1/(h_phi_smear->Integral()));
  h_eta_smear->Scale(1/(h_eta_smear->Integral()));
  h_pt_smear->Scale(1/(h_pt_smear->Integral()));

  // Rebin
  int Nrebin = 2;

  h_d0->Rebin(Nrebin);
  h_z0->Rebin(Nrebin);
  h_phi->Rebin(Nrebin);
  h_eta->Rebin(Nrebin);
  h_pt->Rebin(Nrebin);

  h_d0_smear->Rebin(Nrebin);
  h_z0_smear->Rebin(Nrebin);
  h_phi_smear->Rebin(Nrebin);
  h_eta_smear->Rebin(Nrebin);
  h_pt_smear->Rebin(Nrebin);
  
  //  cout<<h_d0->GetEntries()<<";"<<h_d0_smear->GetEntries()<<endl;

  TCanvas *c1 = new TCanvas();
  h_d0->Draw("hist");
  h_d0_smear->Draw("hist same");
  h_d0->GetXaxis()->SetTitle("d_{0} [mm]");
  c1->SetLogy();
  c1->SaveAs("d0.png");

  TCanvas *c2 = new TCanvas();
  h_z0->Draw("hist");
  h_z0_smear->Draw("hist same");
  h_z0->GetXaxis()->SetTitle("z_{0} [mm]");
  c2->SetLogy();
  c2->SaveAs("z0.png");

  TCanvas *c3 = new TCanvas();
  h_phi->Draw("hist");
  h_phi_smear->Draw("hist same");
  h_phi->GetXaxis()->SetTitle("#phi");
  c3->SetLogy();
  c3->SaveAs("phi.png");

  TCanvas *c4 = new TCanvas();
  h_eta->Draw("hist");
  h_eta_smear->Draw("hist same");
  //c4->SetLogy();
  h_eta->GetXaxis()->SetTitle("#eta");
  c4->SaveAs("eta.png");

  TCanvas *c5 = new TCanvas();
  h_pt->Draw("hist");
  h_pt_smear->Draw("hist same");
  h_pt->GetXaxis()->SetTitle("p_{T} [GeV]");
  c5->SetLogy();
  c5->SaveAs("pt.png");

}
