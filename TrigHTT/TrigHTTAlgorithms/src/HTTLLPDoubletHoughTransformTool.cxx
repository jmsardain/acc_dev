#include "TH2.h"
#include  <fstream>
#include "TrigHTTAlgorithms/HTTLLPDoubletHoughTransformTool.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTAlgorithms/HTTRoadUnionTool.h"
#include "TrigHTTObjects/HTTRoad.h"
#include <chrono>
#include <ctime>
#include <unistd.h>
// simple 2D vector with basic arithmetics
typedef double fp_t; //  floating point type alias, can change it to float from double

typedef std::array<fp_t, 2> pvec;
pvec operator+(const pvec& a, const pvec& b);
pvec operator-(const pvec& a, const pvec& b);
pvec operator*(const pvec& a, double scale);


pvec operator+(const pvec& a, const pvec& b) {
    return { {a[0]+b[0], a[1]+b[1]} };
}
pvec operator-(const pvec& a, const pvec& b) {
    return { {a[0]-b[0], a[1]-b[1]} };

}
pvec operator*(const pvec& a, double scale) {
    return { {a[0]*scale, a[1]*scale} };
}

double length(const pvec& v) {
    return std::hypot(v[0], v[1]);
}

pvec rotate90( const pvec& v) {
    return { -v[1], v[0]};
}

double crossProduct( const pvec& a, const pvec& b ) {
    return a[0]*b[1] - a[1]*b[0];
}


HTTLLPDoubletHoughTransformTool::HTTLLPDoubletHoughTransformTool(const std::string& algname, const std::string& name , const IInterface* ifc) :
    AthAlgTool(algname, name, ifc),
    m_tHistSvc("THistSvc", name)
{
    declareInterface<HTTRoadFinderToolI>(this);
    declareProperty("nBins_x", m_imageSize_x, "Accumulator szize in d0");
    declareProperty("nBins_y", m_imageSize_y, "Accumulator szsize in q/pt");
    declareProperty("threshold50", m_threshold50, "Threshold to be allied to bind of |d0| < 50");
    declareProperty("threshold", m_threshold, "Threshold to be allied to bins of |d0|>=50");
    declareProperty("d0_range", m_d0_range, "The d0 span: (-range, range)");
    declareProperty("qpT_range", m_qOverPt_range, "The q/pT span: (-range, range)");
    declareProperty("triplet", m_triplet, "Make doublet (false) or triplet (true)");
}


StatusCode HTTLLPDoubletHoughTransformTool::initialize() {
    if (m_imageSize_y %2 == 1) {
        ATH_MSG_ERROR("Can not have odd number  " <<  m_imageSize_y << " of bins in q/pT - will result in division by 0");
        return StatusCode::FAILURE;
    }
    m_step_x = (2*m_d0_range) / m_imageSize_x;
    m_step_y = (2*m_qOverPt_range) / m_imageSize_y;

    setHistDir("/JMS/");

    myfile.open("hits.txt");
    myfile << "event,layer,r,x,y,z" << std::endl;

    h_hit_dR_before = new TH1D("h_hit_dR_before", "dR(hit 1, hit2) before", 100, 0, 1200);
    h_hit_dz_before = new TH1D("h_hit_dz_before", "dz(hit 1, hit2) before", 100, -4000, 4000);
    h_hit_deta_before = new TH1D("h_hit_deta_before", "deta(hit 1, hit2) before", 100, -4, 4);
    h_hit_dphi_before = new TH1D("h_hit_dphi_before", "dphi(hit 1, hit2) before", 100, -4, 4);

    h_hit_phislope_12 = new TH1D("h_hit_phislope_12", "phi_slope(hit 1, hit2) before", 1000, -0.05, 0.05);
    h_hit_phislope_23 = new TH1D("h_hit_phislope_23", "phi_slope(hit 2, hit3) before", 1000, -0.05, 0.05);
    h_hit_dphislope = new TH1D("h_hit_dphislope", "dphi_slope(hit 12, hit23) before", 2000, -50, 50);

    h_hit_dR_after = new TH1D("h_hit_dR_after", "dR(hit 1, hit2) after", 100, 0, 1200);
    h_hit_dz_after = new TH1D("h_hit_dz_after", "dz(hit 1, hit2) after", 100, -4000, 4000);
    h_hit_deta_after = new TH1D("h_hit_deta_after", "deta(hit 1, hit2) after", 100, -4, 4);
    h_hit_dphi_after = new TH1D("h_hit_dphi_after", "dphi(hit 1, hit2) after", 100, -4, 4);


    h_hit_Rz_ev2   = new TH2D("h_hit_Rz_ev2", "all hit (R, z) event 2", 1000, -4000, 4000, 1000, 0, 1200);
    h_hit_Rphi_ev2 = new TH2D("h_hit_Rphi_ev2", "all hit (R, phi)  event 2", 1000, 0, 3, 1000, 0, 1200);

    h_hit_Rz_ev5   = new TH2D("h_hit_Rz_ev5", "all hit (R, z) event 5", 1000, -4000, 4000, 1000, 0, 1200);
    h_hit_Rphi_ev5 = new TH2D("h_hit_Rphi_ev5", "all hit (R, phi)  event 5", 1000, 0, 3, 1000, 0, 1200);

    h_nRoads = new TH1D("h_nRoads", "nRoads", 100, 0, 100);

    h_time = new TH1D("h_time", "time", 1000, 0, 1000);

    ATH_CHECK(regHist(getHistDir(), h_hit_dR_before));
    ATH_CHECK(regHist(getHistDir(), h_hit_dz_before));
    ATH_CHECK(regHist(getHistDir(), h_hit_deta_before));
    ATH_CHECK(regHist(getHistDir(), h_hit_dphi_before));

    ATH_CHECK(regHist(getHistDir(), h_hit_phislope_12));
    ATH_CHECK(regHist(getHistDir(), h_hit_phislope_23));
    ATH_CHECK(regHist(getHistDir(), h_hit_dphislope));

    ATH_CHECK(regHist(getHistDir(), h_hit_dR_after));
    ATH_CHECK(regHist(getHistDir(), h_hit_dz_after));
    ATH_CHECK(regHist(getHistDir(), h_hit_deta_after));
    ATH_CHECK(regHist(getHistDir(), h_hit_dphi_after));

    ATH_CHECK(regHist(getHistDir(), h_hit_Rz_ev2));
    ATH_CHECK(regHist(getHistDir(), h_hit_Rphi_ev2));

    ATH_CHECK(regHist(getHistDir(), h_hit_Rz_ev5));
    ATH_CHECK(regHist(getHistDir(), h_hit_Rphi_ev5));

    // ATH_CHECK(regHist(getHistDir(), h_hit1_Rz_before));
    // ATH_CHECK(regHist(getHistDir(), h_hit2_Rz_before));
    // ATH_CHECK(regHist(getHistDir(), h_hit1_Rz_after));
    // ATH_CHECK(regHist(getHistDir(), h_hit2_Rz_after));
    //
    // ATH_CHECK(regHist(getHistDir(), h_hit1_Rphi_before));
    // ATH_CHECK(regHist(getHistDir(), h_hit2_Rphi_before));
    // ATH_CHECK(regHist(getHistDir(), h_hit1_Rphi_after));
    // ATH_CHECK(regHist(getHistDir(), h_hit2_Rphi_after));

    ATH_CHECK(regHist(getHistDir(), h_nRoads));
    ATH_CHECK(regHist(getHistDir(), h_time));

    return StatusCode::SUCCESS;
 }
StatusCode HTTLLPDoubletHoughTransformTool::finalize() {
    ATH_MSG_INFO("Number of events procesed by the HTTLLPDoubletHoughTransformTool " << m_eventsProcessed <<  " roads " << m_roadsGenerated);
    return StatusCode::SUCCESS;
}

StatusCode HTTLLPDoubletHoughTransformTool::getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) {



    auto start = std::chrono::steady_clock::now();
    roads.clear();
    //m_roads.clear();
    m_eventsProcessed++;
    //std::cout << " here i am in doublet code " << std::endl;

    float count2loop = 0;
    float count3loop = 0;

    float countbins = 0;
    float countbins_pass = 0;
    float countbins_pass_local = 0;

    bool make3loops;
    if(m_triplet){
      make3loops = true;
    } else {
      make3loops = false;
    }
    //bool make3loops = false;

    // Understand the events
    for ( auto hit = hits.begin(); hit != hits.end(); ++hit ) {
      myfile << m_event << ","
             << (*hit)->getLayer() << ","
             << (*hit)->getR() << ","
             << (*hit)->getX() << ","
             << (*hit)->getY() << ","
             << (*hit)->getZ()
             << std::endl;
    }

    for ( auto hit = hits.begin(); hit != hits.end(); ++hit ) {
      if ( m_event == 2 ){
          h_hit_Rz_ev2->Fill((*hit)->getZ(), (*hit)->getR());
          h_hit_Rphi_ev2->Fill(Phi((*hit)->getX(),  (*hit)->getY()) , (*hit)->getR());
      }
      if ( m_event == 5 ){
          h_hit_Rz_ev5->Fill((*hit)->getZ(), (*hit)->getR());
          h_hit_Rphi_ev5->Fill(Phi((*hit)->getX(),  (*hit)->getY()) , (*hit)->getR());
      }
    }
    Image image(m_imageSize_x, m_imageSize_y); // not quite optimal to allocate this memory in evey event, but following nominal HT
    ATH_MSG_INFO(" Size of hit : " << hits.size());
    double iter = 0;
    for ( auto hit1 = hits.begin(); hit1 != hits.end(); ++hit1 ) {
      double iter1 = 0;

        for ( auto hit2 = hit1+1; hit2 != hits.end(); ++hit2 ) {
            ATH_MSG_DEBUG("Hits pair R: " << (*hit1)->getR() << " " << (*hit2)->getR());

            const double radiusDifference =  (*hit2)->getR() - (*hit1)->getR();
            const double zDifference = Dz((*hit1)->getZ(), (*hit2)->getZ());
            const double etaDifference =  Deta((*hit1)->getX(), (*hit1)->getY(), (*hit1)->getZ(), (*hit2)->getX(), (*hit2)->getY(), (*hit2)->getZ());
            const double phiDifference = Dphi((*hit1)->getX(), (*hit1)->getY(), (*hit2)->getX(), (*hit2)->getY()) ;
            const double phislope12 = Phi_slope((*hit1)->getX(), (*hit1)->getY(), (*hit2)->getX(), (*hit2)->getY());

            if ( m_event == 5 ){
              h_hit_dR_before->Fill( radiusDifference) ;
              h_hit_dz_before->Fill( zDifference );
              h_hit_deta_before->Fill( etaDifference );
              h_hit_dphi_before->Fill( phiDifference);
            }


            if ( (*hit1)->getLayer() == (*hit2)->getLayer() )
                continue;

            if (  not (m_acceptedDistanceBetweenLayersMin < radiusDifference && radiusDifference < m_acceptedDistanceBetweenLayersMax) )
                continue;
            //

            if(make3loops){
              bool pass = false;
              for ( auto hit3 = hit2+1; hit3 != hits.end(); ++hit3 ) {
                //
                const double radiusDifference23 =  (*hit3)->getR() - (*hit2)->getR();
                const double zDifference23 = Dz((*hit2)->getZ(), (*hit3)->getZ());
                const double etaDifference23 =  Deta((*hit2)->getX(), (*hit2)->getY(), (*hit2)->getZ(), (*hit3)->getX(), (*hit3)->getY(), (*hit3)->getZ());
                const double phislope23 = Phi_slope((*hit2)->getX(), (*hit2)->getY(), (*hit3)->getX(), (*hit3)->getY());

                if ( (*hit2)->getLayer() != (*hit3)->getLayer()
                    && ((( radiusDifference / zDifference ) * ( radiusDifference23 / zDifference23 ) ) > 0 )
                    && ( (phislope23 * phislope12) > 0) ) {
                      ATH_CHECK(fillImage( *hit1, *hit2, image ));
                      pass = true;
                      break;
                    }
              }
              // if(pass) continue;
              count3loop++;
            } else if (!make3loops) {
              //std::cout << " Doing standard method " << std::endl;
              ATH_CHECK(fillImage( *hit1, *hit2, image ));
              count2loop++;
            }

            if ( m_event == 5 ){
              h_hit_dR_after->Fill( radiusDifference) ;
              h_hit_dz_after->Fill( zDifference );
              h_hit_deta_after->Fill( etaDifference );
              h_hit_dphi_after->Fill( phiDifference);
              // h_hit1_Rz_after->Fill((*hit1)->getZ(), (*hit1)->getR());
              // h_hit2_Rz_after->Fill((*hit2)->getZ(), (*hit2)->getR());
              //
              // h_hit1_Rphi_after->Fill(Phi((*hit1)->getX(),  (*hit1)->getY()) , (*hit1)->getR());
              // h_hit2_Rphi_after->Fill(Phi((*hit2)->getX(),  (*hit2)->getY()) , (*hit2)->getR());
            }
            //if (iter==0){


            //}
            //iter1++;
        }
        //iter++;
    }
    if ( m_event < 20 )
        drawImage(image);

    for (int y = 0; y < m_imageSize_y; y++) {
        for (int x = 0; x < m_imageSize_x; x++) {
            if (passThreshold(image, x, y) && isLocalMaxima( image, x, y ) ) {
                ATH_MSG_DEBUG("Creating road with threshold " << image(x,y).first << " hits " << image(x,y).second.size()
                                << " at x: " << x  << " d0 " << xtod0(x) <<  " y: "<< y << " q/pt " << ytoqoverpt(y));
                m_roadsGenerated++;
                roads.push_back(createRoad(image(x, y).second, x, y));
            }
        }
    }

    // re-write the create road algorithm using
    // max of image(x,y).first as first approximation
    // (like it's done using histo)
    // double max =0, extractd0=-99999, extractqpT=-99999;
    // for (int y = 0; y < m_imageSize_y; y++) {
    //   for (int x = 0; x < m_imageSize_x; x++) {
    //     if (passThreshold(image, x, y)) {
    //       if (image(x,y).first > max) {
    //         max = image(x,y).first;
    //         extractd0 = xtod0(x);
    //         extractqpT = ytoqoverpt(y);
    //       }
    //     }
    //   }
    // }
    // double counterOfMax = 0;
    // ATH_MSG_INFO(" HERE I AM extractd0: " << extractd0
    //             << " extractqpT: " << extractqpT);
    // for (int y = 0; y < m_imageSize_y; y++) {
    //   for (int x = 0; x < m_imageSize_x; x++) {
    //     if(image(x,y).first == max){
    //       if(counterOfMax<20){
    //         roads.push_back(createRoad(image(x, y).second, x, y));
    //         counterOfMax++;
    //       }
    //     }
    //   }
    // }


    ATH_MSG_DEBUG(" count2loop = " << count2loop << " count3loop = " << count3loop);
    ATH_MSG_DEBUG(" countbins = " << countbins << " countbins_pass = " << countbins_pass << " countbins_pass_local = " << countbins_pass_local );
    h_nRoads->Fill(roads.size());

    //ATH_MSG_DEBUG(" Size of m_roads = " << m_roads.size());
    ATH_MSG_INFO(" Size of roads = " << roads.size());

    //roads.reserve(m_roads.size());
    //for (HTTRoad_Hough & r : m_roads) roads.push_back(&r);
    m_event++;
    auto end = std::chrono::steady_clock::now();
    double elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    h_time->Fill(elapsed_time);
    std::cout << "elapsed_time = " << elapsed_time << std::endl;
    return StatusCode::SUCCESS;
}
// ====================================================
// Calculate R of hits (same as hit->getR())
double HTTLLPDoubletHoughTransformTool::R(const double& x, const double& y)
{
    return sqrt(x*x+y*y);
}
// ====================================================
// Calculate eta of hits
double HTTLLPDoubletHoughTransformTool::Eta(const double& x, const double& y, const double& z)
{
    double r = R(x,y);
    double r3 = sqrt(r*r + z*z);
    double theta = acos(z/r3);

    return -log(tan(theta*0.5));
}
// ====================================================
// Calculate phi of hits
double HTTLLPDoubletHoughTransformTool::Phi(const double& x, const double& y)
{
    return atan2(y,x);
}
// ====================================================
// Calculate eta difference between hits
double HTTLLPDoubletHoughTransformTool::Deta(const double& x1, const double& y1, const double& z1,
            const double& x2, const double& y2, const double& z2)
{
    double eta1 = Eta(x1,y1,z1);
    double eta2 = Eta(x2,y2,z2);

       return eta1 - eta2;
}
// ====================================================
// Calculate phi difference between hits
double HTTLLPDoubletHoughTransformTool::Dphi(const double& x1, const double& y1, const double& x2, const double& y2)
{
    double phi1 = Phi(x1,y1);
    double phi2 = Phi(x2,y2);
    double dphi = phi2 - phi1;

    if (dphi>TMath::Pi()) dphi-=2*TMath::Pi();
    if (dphi<-TMath::Pi()) dphi+=2*TMath::Pi();

    return dphi;
}
// ====================================================
// Calculate Z difference between hits
double HTTLLPDoubletHoughTransformTool::Dz(const double& z1, const double& z2)
{
    return z2 -z1;
}
// ====================================================
double HTTLLPDoubletHoughTransformTool::Phi_slope(const double& x1, const double& y1, const double& x2, const double& y2)
{
    double dphi = Dphi(x1,y1,x2,y2);
    double r1 = R(x1,y1);
    double r2 = R(x2,y2);
    double dr   = r2 - r1;

    double phi_slope = -999;

    if( fabs(dr)>0 ){
        phi_slope = dphi / dr;
    }
    else{

        if( dphi>0 ) {
          phi_slope = std::numeric_limits<double>::max();
        }
        else if( dphi<0 ) {
          phi_slope = -std::numeric_limits<double>::max();
        }
        else {
          phi_slope = 0;
        }
    }

    return phi_slope;
}
// ====================================================

StatusCode HTTLLPDoubletHoughTransformTool::fillImage(const HTTHit *hit1, const HTTHit *hit2, Image& image) const {
    const pvec p1 {{hit1->getX(), hit1->getY()}};
    const pvec p2 {{hit2->getX(), hit2->getY()}};
    const pvec halfDiff = (p2 - p1)*0.5;
    const fp_t halfLen = length(halfDiff);

    int xbefore = -1;
    // ATH_MSG_INFO(" Here: hit1->getX() = " << hit1->getX() << " hit1->getY() = " << hit1->getY() << " R : " << sqrt(hit1->getX()*hit1->getX()+hit1->getY()*hit1->getY()));
    // ATH_MSG_INFO(" Here: hit2->getX() = " << hit2->getX() << " hit2->getY() = " << hit2->getY() << " R : " << sqrt(hit2->getX()*hit2->getX()+hit2->getY()*hit2->getY())) ;
    //
    // ATH_MSG_INFO(" Here: m_imageSize_y = " << m_imageSize_y << " m_qOverPt_range = " << m_qOverPt_range
    //                 << " m_imageSize_x = " << m_imageSize_x << " m_d0_range = " << m_d0_range);
    // //for ( int y = 0; y < m_imageSize_y; y++ ) {

    for ( int y = 1; y < m_imageSize_y+1; y++ ) {
        const fp_t qoverpt = -1.*( (y * m_step_y) + m_step_y*0.5 - m_qOverPt_range); // use middle f the bin
        const fp_t radius = 1.0/(0.6*qoverpt); // magic transform constants
        const fp_t scale = std::copysign( std::sqrt( std::pow(radius/halfLen, 2) - 1), radius );
        const pvec rprime = rotate90(halfDiff) * scale;
        const pvec center = p1 + halfDiff + rprime;
        const fp_t d0 =  (std::signbit(radius) ? -1.0 : 1.0)*(length(center) - abs(radius));
        //if (isnan(scale)) continue;
        // ATH_MSG_INFO("q/pT : " << qoverpt << " radius : " << radius << " len(center) " << length(center)
        //               << " pow : " <<  std::pow(radius/halfLen, 2) - 1
        //               << " scale : " << scale << " rprime : " << rprime
        //               << " center: " << center << " d0: " << d0
        //               << " std::pow(radius/halfLen, 2) - 1 : " << std::pow(radius/halfLen, 2) - 1);
        // // find the bin x position in the image
        int x = (d0 + m_d0_range) / m_step_x;

        if ( 1 <= x && x < m_imageSize_x) {
            if (xbefore == -1) xbefore = x;
            if ( m_continuous ) { // fill the bins along x starintg from the last one filled
                const int xmin =  (xbefore < x)? xbefore: x;
                const int xmax =  (xbefore < x)? x: xbefore;
                for ( int xinterpolated = xmin; xinterpolated <= xmax; ++xinterpolated) {
                    image(xinterpolated, y).first++;
                    image(xinterpolated, y).second.insert( hit1 );
                    image(xinterpolated, y).second.insert( hit2 );
                }
            } else {
                image(x, y).first++;
                image(x, y).second.insert( hit1 );
                image(x, y).second.insert( hit2 );
            }
            xbefore = x;
        }
    }
    return StatusCode::SUCCESS;
}

//HTTRoad_Hough* HTTLLPDoubletHoughTransformTool::createRoad(std::unordered_set<const HTTHit*> const & hits, unsigned /*x*/, unsigned /*y*/) const {
HTTRoad_Hough* HTTLLPDoubletHoughTransformTool::createRoad(std::unordered_set<const HTTHit*> const & hits, unsigned x, unsigned y) const {
    // Get the road hits
    std::vector<const HTTHit*> road_hits;
    layer_bitmask_t hitLayers = 0;
    for (const HTTHit * hit : hits)
    {
        road_hits.push_back(hit);
        hitLayers |= 1 << hit->getLayer();
    }

    auto sorted_hits = ::sortByLayer(road_hits);
    sorted_hits.resize(8); // If no hits in last layer, return from sortByLayer will be too short

    HTTRoad_Hough *r = new HTTRoad_Hough;
    r->setHitLayers(hitLayers);
    r->setHits(sorted_hits);

    // filling the x and y does not make much sense as of now, will add it once monitoring tool is able to understand it
    //    r.setX(m_bins_x[x] + m_step_x/2);
    //    r.setY(m_bins_y[y] + m_step_y/2);
    r->setX(xtod0(x));
    r->setY(ytoqoverpt(y));

    return r;
}


bool HTTLLPDoubletHoughTransformTool::passThreshold(Image const & image, int x, int y) const {
    const int count = image(x,y).first;
    const float d0 = xtod0(x);
    if ( std::abs(d0) < 50.0 && count >= m_threshold50 ) return true;
    if ( std::abs(d0) >= 50.0 && count >= m_threshold ) return true;

    return false;
}

bool HTTLLPDoubletHoughTransformTool::isLocalMaxima(Image const & image, int x, int y) const {
    const auto centerValue =  image(x,y).first;
    for ( int xaround = std::min(x-1, 0); xaround <= std::max(m_imageSize_x-1, x+1); xaround++  ) {
        for ( int yaround = std::min(y-1, 0); yaround <= std::max(m_imageSize_y-1, y+1); yaround++  ) {
    //for ( int xaround = std::max(x-1, 0); xaround <= std::min(m_imageSize_x-1, x+1); xaround++  ) {
    //  for ( int yaround = std::max(y-1, 0); yaround <= std::min(m_imageSize_y-1, y+1); yaround++  ) {
        if ( image(xaround,yaround).first > centerValue ) { return false; }
        }
    }
    return true;
}

void HTTLLPDoubletHoughTransformTool::drawImage(Image const & image) const {

    TH2D h(("event"+std::to_string( m_event )).c_str(), "LLP Doublet Hough space;d_{0}[mm];q/pT [e/GeV]",
            m_imageSize_x, -m_d0_range, m_d0_range,
            m_imageSize_y, -m_qOverPt_range, m_qOverPt_range );

    for (int x = 0; x < m_imageSize_x; x++){
        for (int y = 0; y < m_imageSize_y; y++){
            h.SetBinContent(x+1, y+1, image(x, y).first); // +1 since root bins are 1-indexed
            // if(m_event == 2){
            //   ATH_MSG_INFO(" size unordered set" << image(x, y).second.size());
            // }
          }
        }

    h.SaveAs((name()+"_event_"+std::to_string( m_event )+".root").c_str());

    // std::ofstream myfile;
    // myfile.open ("hough_event_"+std::to_string( m_event )+".txt");

    //TH1D h1(("eventresqoverpt"+std::to_string( m_event )).c_str(), ";Res(q/pT);",100, -5, 5);
    //TH1D h2(("eventresd0"+std::to_string( m_event )).c_str(), ";Res(d0);",100, -5, 5);

    //std::vector<HTTTruthTrack> truth_tracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
    double max = h.GetMaximum();
    double extractqpT = -9999;
    double extractd0  = -9999;
    for(int ibinx=1; ibinx<h.GetNbinsX()+1; ++ibinx){
      for(int ibiny=1; ibiny<h.GetNbinsY()+1; ++ibiny){
        if (h.GetBinContent(ibinx, ibiny) != max)  continue;
        extractqpT = h.GetYaxis()->GetBinCenter(ibiny);
        extractd0  = h.GetXaxis()->GetBinCenter(ibinx);
      }
    }
    ATH_MSG_INFO(" I am here and extractqpT : " << extractqpT << " extractd0: " << extractd0);
    // myfile << m_event << " " << extractqpT << " " << extractd0;
    // myfile.close();

    // std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();
    // HTTTruthTrack const & truth_t = truthtracks.front();
    //std::vector<HTTTruthTrack> const & truthtracks = m_logicEventInputHeader_1st->optional().getTruthTracks();

    //
    // for (auto const & t : truth_tracks) {
    //   h1.Fill((t.getQ()/t.getPt()) - extractqpT);
    //   h2.Fill((t.getD0()) - extractd0);
    //   ATH_MSG_INFO(" I am here and t.getQ()/t.getPt(): " << t.getQ()/t.getPt() << " extractqpT : " << extractqpT
    //                 << " t.getD0(): " << t.getD0() << " extractd0: " << extractd0);
    //
    // }
    // h1.SaveAs((name()+"_eventresqoverpt_"+std::to_string( m_event )+".root").c_str());
    // h2.SaveAs((name()+"_eventresd0_"+std::to_string( m_event )+".root").c_str());


}
