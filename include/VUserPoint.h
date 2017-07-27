#ifndef _UpCoreVUserPoint_
#define _UpCoreVUserPoint_

#include <iostream>
#include <string>
using namespace std ;

#include "TFile.h"
#include "TTree.h"

#include "TMidasEvent.h"

class VUserPoint{
  
public:
  static VUserPoint* getInstance();
  virtual void InitOutput(string );
  virtual void EventAction(TMidasEvent*);
  virtual void BeginOfRunAction();
  virtual void EndOfRunAction();
  virtual TTree* GetRootTree();
  virtual void Destroy() ;
  
protected:
  VUserPoint();
  ~VUserPoint();
  
protected:
  static VUserPoint* instance ;

};

#endif