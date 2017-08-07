#ifndef _UP_EXT_TIGRESS_H
#define _UP_EXT_TIGRESS_H

#include "TTigressData.h"
#include "TMidasEvent.h"


class Tigress{
  
public:
  Tigress();
  ~Tigress();
  
private:
  TTigressData* m_data;
  
public:
  TTigressData* GetData();
  void SetData(TTigressData* data);
  void Clear();
  
public: // Method and object to construct to fill the data event from a Tigress DAQ event tree
  void ReadFSPCFile(string FSPCPath);//!
  void FillData(TMidasEvent* TigEvent);//!
  void FillGe(unsigned short GeCloverNbr, unsigned short GeCrystalNbr , unsigned short GeSegmentNbr, int hit,TMidasEvent* TigEvent);
  void FillBGO(unsigned short BGOCloverNbr, unsigned short BGOPmNbr , unsigned short BGOSegmentNbr, int hit,TMidasEvent* TigEvent);
  
  map< int,vector<int> > GetFSPC2Detector() const {return m_FSPC2Detector;}//!
  
private:// Key is the FSPC channel,
        // vector[0] is the Clover Nbr
        // vector[1] is the crystal Nbr
        // vector[2] is the segment Nbr
        // vector[3] is 0: Ge , 1: BGO
  map< int,vector<int> > m_FSPC2Detector;//!
  
};
#endif