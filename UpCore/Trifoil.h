#ifndef _UP_EXT_TRIFOIL_H
#define _UP_EXT_TRIFOIL_H

#include"TTrifoilData.h"
#include"TMidasEvent.h"

class Trifoil{
  
public:
  Trifoil();
  ~Trifoil();
  
private:
  TTrifoilData* m_data;
  
public:
  TTrifoilData* GetData();
  void SetData(TTrifoilData*);
  void Clear();
  
public: // Method and object to construct to fill the data event from a Tigress DAQ event tree
  void ReadFSPCFile(string FSPCPath);//!
  void FillData(TMidasEvent* TigEvent);//!
  
private:
  int m_FSPC_Channel;//!
  
};
#endif