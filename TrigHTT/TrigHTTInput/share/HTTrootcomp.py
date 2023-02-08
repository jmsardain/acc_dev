#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# @file:    root.comp.py
# @purpose: Script to compare two root files, stolen from TrigVal rootcomp.py

"""
Compares two HTT root files.

Return value: script returns 0 if
                   - athena Job succeeds
                   - comparison with reference succeeds 
  
"""              

__author__  = "F.Pastore"
__doc__     = "Script to compare two HTT root files"

import sys

def lsroot(thedir):
   """Return list of all keys in 'dir' (recursively)"""

   import ROOT
   
   def dols(direc, keys):
      """Do the recursive traversal"""
      dirList = direc.GetListOfKeys()
#      print dirList.Print()
      for k in dirList:
         kname = k.GetName()
         if k.GetClassName()=="TDirectoryFile" or k.GetClassName()=="TDirectory":
            direc.cd(kname)
            dols(ROOT.gDirectory, keys)
         elif k.GetClassName()=="TObjString":
             print "Description = ",direc.Get(kname).GetName()
         else:
             path = direc.GetPath() if direc.GetPath().endswith('/') else direc.GetPath()+"/"
             keys += [path+kname]

      
      direc.cd("..")
      return

   keys = []
   basedir = thedir.GetPath().rstrip("/")+ "/"
   dols(thedir,keys)
   
   # Return sorted list with base directory removed
   return sorted([k.replace(basedir,"") for k in keys])


def diffFiles(ref,newfile):
    """Compare the histograms TH1F/TH2F and the TTree in both files
       returns number of errors found during comparison
    """
    
    import ROOT   
    tref = ROOT.TFile.Open(ref, "read")
    tfile = ROOT.TFile.Open(newfile, "read")
    if tref.IsZombie() or tfile.IsZombie():
        return 1

    print "Reference:", ref
    lsref = lsroot(tref)
    print "New:", newfile
    lsfile = lsroot(tfile)
    
#    print "Ref: ", lsref
#    print "New: ", lsfile
    diff = 0

    if  len(lsfile) != len(lsref):
        print "ERROR: Not found same number of TObjects in file: New ",len(lsfile)," Ref ", len(lsref)
        diff+=1
    
    uncommon = [item for item in lsref if item not in set(lsfile) ]
    if  len(uncommon):
        print "ERROR: Found these un-common items: "
        print("\n".join(uncommon))
        diff+=1


    common = [item for item in lsref if item in set(lsfile) ]
    for item in common:
        refOb = tref.Get(item)
        newOb = tfile.Get(item)
        className=refOb.ClassName()
        if className == "TH1F" or className == "TH2F":
            if refOb.GetEntries() != newOb.GetEntries():
                print "ERROR: Different entries in ", refOb.GetName()," :  ref=", refOb.GetEntries(),", new= ",newOb.GetEntries()
                diff+=1        
        elif className == "TH1F":
            if abs(refOb.GetMean() - newOb.GetMean()) > 0.01 * abs(refOb.GetMean()):
                 print "ERROR: TH1F ", refOb.GetName(), " Mean diff: ", refOb.GetMean(),newOb.GetMean()                 
                 diff+=1
            if abs(refOb.GetRMS() - newOb.GetRMS()) > 0.01 * abs(refOb.GetRMS()) :
                 print "ERROR: TH1F ", refOb.GetName(), " RMS diff: ", refOb.GetRMS(),newOb.GetRMS()
                 diff+=1
        elif className == "TH2F":
            if abs(refOb.ProjectionX().GetMean() - newOb.ProjectionX().GetMean()) > 0.01 * abs(refOb.ProjectionX().GetMean()):
                 print "ERROR: TH2F ", refOb.GetName(), " Mean diff: ", refOb.ProjectionX().GetMean(),newOb.ProjectionX().GetMean()
                 diff+=1
            if abs(refOb.ProjectionX().GetRMS() - newOb.ProjectionX().GetRMS()) > 0.01 * abs(refOb.ProjectionX().GetRMS()):
                 print "ERROR: TH2F ", refOb.GetName()," RMS diff: ",  refOb.ProjectionX().GetRMS(),newOb.ProjectionX().GetRMS()
                 diff+=1
        elif className  ==  "TTree":
            filetype=""
            isInput=0
            isOutput=1
            for branch in refOb.GetListOfBranches():
                if( branch.GetName() == "HTTEventInputHeader" ):
                    filetype=isInput
                elif( branch.GetName() == "HTTLogicalEventInputHeader" ):
                    filetype=isOutput
            
            if filetype == isInput:            
                if newOb.GetBranch("HTTEventInputHeader") == 0x0:
                    print "ERROR: Branch HTTEventInputHeader not found in new file"
                    return 1

            if filetype == isOutput:            
                if newOb.GetBranch("HTTLogicalEventInputHeader") == 0x0:
                    print "ERROR: Branch HTTLogicalEventInputHeader not found in new file"
                    return 1
           
            
            refTree =refOb
            newTree =newOb
            NEvref = refTree.GetEntries()
            NEvnew = newTree.GetEntries()
            if NEvref != NEvnew:
                print "ERROR: found different events in Tree: ref=", NEvref, " new=", NEvnew
                diff+=1
            print "TTree diff: Found total events in Tree: ref=", NEvref," new=", NEvnew

            if filetype == isInput:
                Nref = 0
                Nnew = 0
                for evt in refTree:
                    Nref+=evt.HTTEventInputHeader.nHits()
                #for hit in evt.m_Hits:
                #    print hit.getX()

                for evt in newTree:
                    Nnew+=evt.HTTEventInputHeader.nHits()

                print "TTree diff: Found total mean hits: ", Nref/NEvref, Nnew/NEvnew 
                if Nref != Nnew:
                    print "ERROR: found different hits: ", Nref, Nnew    
                    diff+=1

            if filetype == isOutput and False:
                # tmp disable this because Tree objects have conflicts, wanrings about HTTClusters (TODO)
                Nref = 0
                Nnew = 0
                for evt in refTree:
                    for tower in evt.HTTLogicalEventInputHeader.towers():
                        Nref+= tower.hits().size()

                print "Found %d hits in ref"%(Nref)
                
                for evt2 in newTree:
                    for tower2 in evt2.HTTLogicalEventInputHeader.towers():
                        Nnew+= tower2.hits().size()

                
                print "Found %d hits in new"%(Nnew)
                print "TTree diff: Found total hits: ", Nref, Nnew
                print "TTree diff: Found total mean hits/event: ", Nref/NEvref, Nnew/NEvnew 
                if Nref != Nnew:
                    print "ERROR: found different hits: ", Nref, Nnew    
                    diff+=1
                
              
    tref.Close()
    tfile.Close()
    del tref
    del tfile

    return diff


def main():

    # Now import ROOT
    ## try:
    ##   from PerfMonAna import PyRootLib
    ##   ROOT = PyRootLib.importRoot( batch=True )
    ## except ImportError:
    ##   import ROOT

    sys.stdout.flush()
    sys.stderr.flush()
   

    ## from ROOT import gROOT, gStyle
    ## gROOT.SetStyle("Plain")
    ## gStyle.SetOptStat(111111)

    print "Running HTTrootcomp.py ",sys.argv[1],sys.argv[2]
#    print(sys.argv)
 
    result = diffFiles(sys.argv[1],sys.argv[2]) 

    print "Overall test result: ", result
    return result


if __name__ == "__main__":
    sys.exit(main())
