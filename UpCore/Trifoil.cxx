#include"Trifoil.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include<cstdlib>
using namespace std;

/////////////////////////
Trifoil::Trifoil(){
  m_data = new TTrifoilData();
}
/////////////////////////
Trifoil::~Trifoil(){
  delete m_data;
}
/////////////////////////
TTrifoilData* Trifoil::GetData(){
  return m_data;
}
/////////////////////////
void Trifoil::SetData(TTrifoilData* data){
  m_data = data;
}
/////////////////////////
void Trifoil::Clear(){
  m_data->Clear();
}
/////////////////////////
void Trifoil::ReadFSPCFile(string FSPCPath){
  ifstream FSPCFile;
  FSPCFile.open(FSPCPath.c_str());
  
  if(!FSPCFile.is_open()) {cout << "Error: FSPC File: " << FSPCPath << " not found " << endl ; exit(1);}
  else {cout << "Trifoil is Reading FSPC file : " << FSPCPath << endl ;}
  
  string LineBuffer;
  string DataBuffer;
  string DetectorName;
  string ChannelNumberStr;
  int    ChannelNumber;
  while(!FSPCFile.eof()){
    getline(FSPCFile,LineBuffer);
    /* Typical line to analyse:
     # 2005: Address 0x00900800, type 8, TRF00XL00X [Trifoil (Waveform) 0x0a00700] 
     */
    stringstream myLine(LineBuffer);
    myLine >> DataBuffer >> ChannelNumberStr >> DataBuffer >> DataBuffer >> DataBuffer >> DataBuffer >> DetectorName  ;
    ChannelNumberStr = ChannelNumberStr.substr(0,ChannelNumberStr.length()-1);
    stringstream(ChannelNumberStr) >> ChannelNumber;
    
    if(DetectorName=="TRF00XL00X"){
      m_FSPC_Channel=ChannelNumber;
    }
  }
}

///////////////////////////////////////////
void Trifoil::FillData(TMidasEvent* TigEvent){
  unsigned int mysize =  TigEvent->channel_number.size();
  for(unsigned int i = 0 ; i < mysize ; i++){
    if(TigEvent->channel_number[i]==m_FSPC_Channel){
      if(i < TigEvent->waveform.size()){
      TH1F h = TigEvent->waveform[i];
      //m_data->SetWaveform(&TigEvent->waveform[i]);
      m_data->SetWaveform(h);
      m_data->SetTimeCFD(TigEvent->cfd_value[i]);
      m_data->SetTimeLED(TigEvent->led_value[i]);
      }
    }
  }
}
