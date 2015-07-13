#ifndef _UP_EXT_SHARC_H
#define _UP_EXT_SHARC_H

#include "TSharcData.h"
#include "TMidasEvent.h"

class Sharc{
  
public:
  Sharc();
  ~Sharc();
  
private:
  TSharcData* m_data;
  
public:
  TSharcData* GetData();
  void SetData(TSharcData* data);
  void Clear();

public: // Method and object to construct to fill the data event from a Tigress DAQ event tree
  void ReadFSPCFile(string FSPCPath);
  void FillData(TMidasEvent* TigEvent);
  void FillBoxFront(int DetNbr,int hit,TMidasEvent* TigEvent);
  void FillBoxBack(int DetNbr,int hit,TMidasEvent* TigEvent);

  void FillBoxBack1(int DetNbr,int hit,TMidasEvent* TigEvent);
  void FillBoxBack2(int DetNbr,int hit,TMidasEvent* TigEvent);
  
  void FillQQQFront(int DetNbr,int hit,TMidasEvent* TigEvent);
  void FillQQQBack(int DetNbr,int hit,TMidasEvent* TigEvent);
  
  void FillPAD(int DetNbr,int hit,TMidasEvent* TigEvent)  ;
  map< int,vector<int> > GetFSPC2Detector() const ;
  int atoi(string s); 
private:// Key is the FSPC channel,
        // vector[0] is the Detector Nbr
        // vector[1] is 0: QQQ , 1: Box, 2: PAD
        // vector[2] the type: 0: Front , 1: Back1 or Back , 2: Back2
        // vector[3] strip nbr (FSPC ref)
  map< int,vector<int> > m_FSPC2Detector;//!

  map<int,int> m_DetNbrMap;
};
#endif
