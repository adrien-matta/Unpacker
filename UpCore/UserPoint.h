#ifndef ____UserPoint__
#define ____UserPoint__

#include <iostream>
#include <string>
using namespace std ;

#include "VUserPoint.h"

#include "TFile.h"
#include "TTree.h"

#include "TMidasEvent.h"
#include "Sharc.h"
#include "Tigress.h"
#include "Trifoil.h"

class UserPoint:public VUserPoint{
public:
  static VUserPoint* getInstance();
  void InitOutput(string );
  void EventAction(TMidasEvent*);
  void BeginOfRunAction();
  void EndOfRunAction();
  TTree* GetRootTree();
  void Destroy();
  
protected:
  UserPoint();
  ~UserPoint();
  
private: // subject to change on a user basis
  TFile* m_outputFile;
  TTree* m_tree;
  Tigress* m_tig;
  Sharc* m_sharc;
  Trifoil* m_trifoil;
  int m_RunNumber;
  int m_RunNumberMinor;

};

#endif
