
#include "VUserPoint.h"

VUserPoint* VUserPoint::instance = 0;

VUserPoint* VUserPoint::getInstance(){
  if(instance==0)
    instance = new VUserPoint();
  
  return instance;
}

void VUserPoint::Destroy(){
  if (instance != 0) {
    delete instance;
    instance = 0;
  }
}

VUserPoint::VUserPoint(){
}

VUserPoint::~VUserPoint(){
  
}

void VUserPoint::InitOutput(string){
}

void VUserPoint::EventAction(TMidasEvent*){
}

void VUserPoint::EndOfRunAction(){
  
}

void VUserPoint::BeginOfRunAction(){

}

TTree* VUserPoint::GetRootTree(){
  return NULL;
}