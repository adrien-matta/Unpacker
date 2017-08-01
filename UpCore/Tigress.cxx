#include "Tigress.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
using namespace std;
/////////////////////////
Tigress::Tigress(){
  m_data = new TTigressData();
}

/////////////////////////
Tigress::~Tigress(){
  delete m_data;
}

/////////////////////////
TTigressData* Tigress::GetData(){
  return m_data;
}

/////////////////////////
void Tigress::SetData(TTigressData* data){
  m_data = data;
}
/////////////////////////
void Tigress::Clear(){
  m_data->Clear();
}
/////////////////////////
// Key is the FSPC channel,
// vector[0] is the Clover Nbr
// vector[1] is the crystal Nbr
// vector[2] is the segment Nbr
// vector[3] is 0: Ge , 1: BGO
void Tigress::ReadFSPCFile(string FSPCPath){
  ifstream FSPCFile;
  FSPCFile.open(FSPCPath.c_str());
  
  if(!FSPCFile.is_open()) {cout << "Error: FSPC File: " << FSPCPath << " not found " << endl ; exit(1);}
  else {cout << "Tigress is Reading FSPC file : " << FSPCPath << endl ;}
  
  string LineBuffer;
  string DataBuffer;
  string DetectorName;
  string ChannelNumberStr;
  int CoreNbr; // This is used to distinguished the two core of the same crystal
  int    ChannelNumber;
  while(!FSPCFile.eof()){
    
    getline(FSPCFile,LineBuffer);
    /* Typical line to analyse: 1st for Ge, 2nd for BGO
     #    9: Address 0x00e00109, type 3, Ge01Bc0 [HPGe Pos01 Blue  Core 9 0x0e00109]
     #   10: Address 0x00e00200, type 4, Sup01Bs0 [Suppressor Pos01 Blue  Seg 0 0x0e00200]
     */
    
    stringstream myLine(LineBuffer);
    myLine >> DataBuffer >> ChannelNumberStr >> DataBuffer >> DataBuffer >> DataBuffer >> DataBuffer >> DetectorName ;
    ChannelNumberStr = ChannelNumberStr.substr(0,ChannelNumberStr.length()-1);
    stringstream(ChannelNumberStr) >> ChannelNumber;
    
    vector<int> FSPCInfo;
    if(DetectorName.length()==10) {
      if(DetectorName.compare(0,3,"TIG")==0 ){ // Very specific to avoid conflict with possible future change
        string DetectorName2 = DetectorName.substr(3,2);
        int CloverNbr=0;
        stringstream(DetectorName2) >> CloverNbr;
        
        DetectorName2 = DetectorName.substr(5,1);
        int CrystalNbr=0;
        if (DetectorName2 == "B") CrystalNbr=1;
        else if (DetectorName2 == "G") CrystalNbr=2;
        else if (DetectorName2 == "R") CrystalNbr=3;
        else if (DetectorName2 == "W") CrystalNbr=4;
        else {cout << "ERROR: FSPC File incorrect" << endl ; exit(1);}
        
        DetectorName2 = DetectorName.substr(8,2);
        int SegmentNbr;
        stringstream(DetectorName2) >> SegmentNbr;
        
        FSPCInfo.push_back(CloverNbr);
        FSPCInfo.push_back(CrystalNbr);
      
        string type = DetectorName.substr(9,1);
        if(type=="B")
          SegmentNbr=9;

        FSPCInfo.push_back(SegmentNbr);
        FSPCInfo.push_back(0);
        
        if(CloverNbr!=0) {
          m_FSPC2Detector[ChannelNumber]= FSPCInfo;
        }
      }
    }
  }
  
  return;
}

/////////////////////////
void Tigress::FillData(TMidasEvent* TigEvent){
  unsigned int size = TigEvent->channel_number.size();
  
  for(unsigned int i = 0 ; i < size ; i++){
    if(m_FSPC2Detector.find(TigEvent->channel_number[i])!=m_FSPC2Detector.end()){
     
      int CloverNbr = m_FSPC2Detector[TigEvent->channel_number[i]][0];
      int CrystalNbr = m_FSPC2Detector[TigEvent->channel_number[i]][1];
      int SegmentNbr = m_FSPC2Detector[TigEvent->channel_number[i]][2];
      int type = m_FSPC2Detector[TigEvent->channel_number[i]][3];

      if (type == 0) {
        FillGe(CloverNbr,CrystalNbr,SegmentNbr,i,TigEvent);
      }
      
      else if(type == 1){
        FillBGO(CloverNbr,CrystalNbr,SegmentNbr,i,TigEvent);
      }
      
      else{cout << "ERROR: FSPC map incorrectly instantiate" << endl ; exit(1);}
    }
  }
 
}

/////////////////////////
void Tigress::FillGe(unsigned short GeCloverNbr, unsigned short GeCrystalNbr , unsigned short GeSegmentNbr, int hit,TMidasEvent* TigEvent){
  if(TigEvent->charge_raw[hit]>0){
  m_data->SetGeCloverNbr( GeCloverNbr);
  m_data->SetGeCrystalNbr( GeCrystalNbr);
  m_data->SetGeSegmentNbr( GeSegmentNbr);
  m_data->SetGeEnergy(TigEvent->charge_raw[hit]);
  m_data->SetGeTimeCFD( TigEvent->cfd_value[hit]);
  m_data->SetGeTimeLED( TigEvent->led_value[hit]);
  }
}

/////////////////////////
void Tigress::FillBGO(unsigned short BGOCloverNbr, unsigned short BGOCrystalNbr , unsigned short BGOPmNbr, int hit,TMidasEvent* TigEvent){
  m_data->SetBGOCloverNbr( BGOCloverNbr);
  m_data->SetBGOCrystalNbr( BGOCrystalNbr);
  m_data->SetBGOPmNbr( BGOPmNbr);
  m_data->SetBGOEnergy( TigEvent->charge_raw[hit]);
  m_data->SetBGOTimeCFD( TigEvent->cfd_value[hit]);
  m_data->SetBGOTimeLED( TigEvent->led_value[hit]);
}
