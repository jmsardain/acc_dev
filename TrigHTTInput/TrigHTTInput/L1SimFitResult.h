
#ifndef L1SimFitResult_H_
#define L1SimFitResult_H_

#include <ostream>
#include <vector>
#include "TObject.h"
#include "TMath.h"


enum fit_status    {good, no_sector, bad_sector, error};
enum pattern_type  {nopat=0, nomiss=1, miss1=2, nomiss_1WC=3, miss1_1WC=4, nomiss_2WC=5};

class FitResult: public TObject {

 public:

  FitResult(){};
  FitResult(int id){_nmuons=id;_barcodem=4;};//some dummy constructor for testing
  
 FitResult(fit_status fitstat) :
  _fitstat(fitstat),
    _pattern_type(pattern_type::nopat),
    _curv(-999.), 
    _eta(-999.), 
    _phi(-999.), 
    _z0(-999.), 
    _d0(-999.), 
    _chi2(-1.), 
    _deltaR(-999.),
    _barcodem(-1),
    _nmuons(-1), 
    _nhitlayers(-1), 
    _pattern(-1), 
    _eventId(-1),
    _passHW(false),
    _setID(0)
      {_layMiss.clear();};
  

 FitResult(fit_status fitstat, int nmuons, int setID, int pattern, int eventId): 
  _fitstat(fitstat),
    _pattern_type(pattern_type::nopat),
    _curv(-999.), 
    _eta(-999.), 
    _phi(-999.), 
    _z0(-999.), 
    _d0(-999.), 
    _chi2(-1.), 
    _deltaR(-999.), 
    _barcodem(-1), 
    _nmuons(nmuons), 
    _nhitlayers(-1), 
    _pattern(pattern), 
    _eventId(eventId),
    _passHW(false),
    _setID(setID)
    {_layMiss.clear();};
  
 FitResult(fit_status fitstat, float curv, float eta, float phi, float z0, float d0, float chi2, 
	   float deltaR, int barcodem, int nmuons, int nhitlayers, int pattern, std::vector <double> hitComb, int setID, int eventId ) :
  _fitstat(fitstat),
    _pattern_type(pattern_type::nopat),
    _curv(curv), 
    _eta(eta),
    _phi(phi), 
    _z0(z0), 
    _d0(d0), 
    _chi2(chi2),  
    _deltaR(deltaR), 
    _barcodem(barcodem), 
    _nmuons(nmuons), 
    _nhitlayers(nhitlayers), 
    _pattern(pattern), 
    _hitComb(hitComb), 
    _eventId(eventId),
    _passHW(false),
    _setID(setID)

    {_layMiss.clear();};


  virtual  ~FitResult(){};


  //get functions

  float curv()       const  { return _curv;};
  float eta()        const  { return _eta;};
  float phi()        const  { return _phi;};
  float z0()         const  { return _z0;};
  float d0()         const  { return _d0;};
  float chi2()       const  { return _chi2;};
  float chi2ndf()    const  { return (_nhitlayers-5) !=0 ? _chi2/float(_nhitlayers-5): -1.;};
  float chi2Prob()   const  { return TMath::Prob(_chi2,(_nhitlayers-5));};//p-value = 1-P(a,x) = prob of having >chi2 by chance
  float deltaR()     const  { return _deltaR;};
  float closerDeltaR() const  { return _closerDeltaR;};
  int   barcodem()   const  { return _barcodem;};// pixel layer is counted once (so need to exclude one pxil coordinate to caclualte the fraction)
  int   nmuons()     const  { return _nmuons;};
  int   nhitlayers() const  { return _nhitlayers;};//real hits used in the fit (excluding WC, but counting all pixel coordinates)
  int   pattern()    const  { return _pattern;};  
  int   eventId()    const  { return _eventId;};
  std::vector <double> hitComb() const {return _hitComb;};
  std::vector<int>  layMiss()    const {return _layMiss;};
  int hwlayers()     const  { return _hwlayers;};//HitWarrior layers
  bool passHW()      const  { return _passHW;};//passed HitWarrior
  int setID()        const  { return _setID;};
  std::vector<unsigned int>  barcodes()    const {return _hitBarcodes;};
  fit_status fitstat() const {return _fitstat;};
  bool isGood()        const {return _fitstat==fit_status::good;};
  pattern_type patternType()  const   { return _pattern_type;};


  //set functions
  void setLayMiss(std::vector<int> lay_miss) { _layMiss=lay_miss;};// missing layers
  void setHwlayers(int hwlayers)             { _hwlayers=hwlayers;};
  void setPassHW(bool passHW)                { _passHW=passHW;};
  void setPatternType(pattern_type type)     { _pattern_type=type;};
  void setPattern(int pattern)               { _pattern=pattern;};
  void setDeltaR(float deltaR)               { _deltaR=deltaR;};
  void setCloserDeltaR(float deltaR) {_closerDeltaR=deltaR;};
  void setBarcodes(std::vector<unsigned int> HitBarcodes) { _hitBarcodes=HitBarcodes;};
  void setBarcodem(unsigned int barcodem)    {  _barcodem=barcodem;};

  // version 8 adding:
  void setHashIds(std::vector<unsigned int> hashIds) {_hashIds=hashIds;};
  std::vector<unsigned int> getHashIds() {return _hashIds;};
 private:
  std::vector<unsigned int> _hashIds;
  //end of adding for version 8

 private:
  fit_status _fitstat;
  pattern_type _pattern_type;
  float      _curv;
  float      _eta;
  float      _phi;
  float      _z0;
  float      _d0;
  float      _chi2;
  float      _deltaR;
  int        _barcodem;
  int        _nmuons;
  int        _nhitlayers;
  int        _pattern; // index of matched pattern
  std::vector <double> _hitComb; //vector containing hit combination used to make the fit
  int        _eventId;
  std::vector<int> _layMiss;
  int       _hwlayers=0;//HitWarrior layers
  bool      _passHW=true;
  int _setID;
  std::vector <unsigned int> _hitBarcodes;
  float _closerDeltaR;

  ClassDef(FitResult,8)  //Event structure
};

std::ostream& operator<<(std::ostream& , const FitResult& );


#endif
