#include "Sharc.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
using namespace std;

/////////////////////////
Sharc::Sharc(){
  m_data = new TSharcData();
  m_DetNbrMap[1] =  16;
  m_DetNbrMap[2] =  13;
  m_DetNbrMap[3] =  14;
  m_DetNbrMap[4] =  15;
  m_DetNbrMap[5] =  9;
  m_DetNbrMap[6] =  10 ;
  m_DetNbrMap[7] =  11;
  m_DetNbrMap[8] =  12;
  m_DetNbrMap[9] =  5 ;
  m_DetNbrMap[10] = 6  ;
  m_DetNbrMap[11] = 7  ;
  m_DetNbrMap[12] = 8  ;
  m_DetNbrMap[13] = 4  ;
  m_DetNbrMap[14] = 1  ;
  m_DetNbrMap[15] = 2  ;
  m_DetNbrMap[16] = 3  ;
}
/////////////////////////
Sharc::~Sharc(){
  delete m_data;
}
/////////////////////////
TSharcData* Sharc::GetData(){
  return m_data;
}
/////////////////////////
void Sharc::SetData(TSharcData* data){
  m_data = data;
}
/////////////////////////
void Sharc::Clear(){
  m_data->Clear();
}
/////////////////////////

void Sharc::ReadFSPCFile(string FSPCPath){
  ifstream FSPCFile;
  FSPCFile.open(FSPCPath.c_str());

  if(!FSPCFile.is_open()) {cout << "Error: FSPC File: " << FSPCPath << " not found " << endl ; exit(1);}
  else {cout << "Sharc is Reading FSPC file : " << FSPCPath << endl ;}

  string LineBuffer;
  string DataBuffer;
  string DetectorName;
  string FrontBack;
  string ChannelNumberStr;
  int    ChannelNumber;
  int    StripNumber;
  while(!FSPCFile.eof()){
    getline(FSPCFile,LineBuffer);
    // Typical line to analyse: #  960: Address 0x00100700, type 6, UmCD1 Front 00 [TIG64-SHARC Up CD 1 Front (UmCD1f 00) 0x0100700]
    stringstream myLine(LineBuffer);
    myLine >> DataBuffer >> ChannelNumberStr >> DataBuffer >> DataBuffer >> DataBuffer >> DataBuffer >> DetectorName >> FrontBack >> StripNumber ;

    ChannelNumberStr = ChannelNumberStr.substr(0,ChannelNumberStr.length()-1);
    stringstream(ChannelNumberStr) >> ChannelNumber;

    //cout << ChannelNumber << " " << DetectorName << " " << FrontBack << " " << StripNumber << endl ;

    // Breakdown the channel name in usefull information:

    vector<int> FSPCInfo;
    if(DetectorName.length()==9 || DetectorName.length()==10) {

      string type_str;
      type_str.push_back( DetectorName[0]);
      type_str.push_back( DetectorName[1]);

      string box_qqq_str;
      box_qqq_str.push_back(DetectorName[2]);

      string det_nbr_str;
      det_nbr_str.push_back(DetectorName[3]);
      det_nbr_str.push_back(DetectorName[4]);  

      string pn_str;
      pn_str.push_back(DetectorName[6]);

      string strip_nbr_str;
      strip_nbr_str.push_back(DetectorName[7]);
      strip_nbr_str.push_back(DetectorName[8]); 

      string pad_dssd ;
      pad_dssd.push_back(DetectorName[5]);

      // Key is the FSPC channel,
      // vector[0] is the Detector Nbr
      // vector[1] is 0: QQQ , 1: Box, 2: PAD
      // vector[2] the type: 0: Front , 1: Back
      // vector[3] strip nbr (FSPC ref)
      if(type_str=="SH"){
        FSPCInfo.push_back(atoi(det_nbr_str));
        if (box_qqq_str=="Q")
          FSPCInfo.push_back(0);
        else if (box_qqq_str=="B" && pad_dssd=="D")
          FSPCInfo.push_back(1);
        else if (box_qqq_str=="B" && pad_dssd=="E"){
          FSPCInfo.push_back(2);
        }
        
        else
          FSPCInfo.push_back(-1);
        /// 
        if (pn_str=="N")
          FSPCInfo.push_back(1);

        else if (pn_str=="P")
          FSPCInfo.push_back(0);

        else
          FSPCInfo.push_back(-1);
        /// 

        FSPCInfo.push_back(atoi(strip_nbr_str));
        m_FSPC2Detector[ChannelNumber]= FSPCInfo;
      }


      /*    else if(DetectorName=="SHARC"){
            if(FrontBack=="DoBx"){;}
            else {cout << "Error: FSPC File Incorrectly formatted" << endl ; exit(1);}
            int DetectorNbr=0;

            if(ChannelNumber==1984) DetectorNbr = 9;
            else if(ChannelNumber==1985) DetectorNbr = 10;
            else if(ChannelNumber==1986) DetectorNbr = 12;
            else if(ChannelNumber==1987) DetectorNbr = 11;

            if(DetectorNbr!=0){
            FSPCInfo.push_back(DetectorNbr);
            FSPCInfo.push_back(2);
            FSPCInfo.push_back(StripNumber);
            m_FSPC2Detector[ChannelNumber]= FSPCInfo;
            }
            } */

    }
  }
  return;
}

void Sharc::FillData(TMidasEvent* TigEvent){
  //cout << TigEvent->channel_number[0] << endl;
  unsigned int size = TigEvent->channel_number.size();
  for(unsigned int i = 0 ; i < size ; i++){

    if(m_FSPC2Detector.find(TigEvent->channel_number[i])!=m_FSPC2Detector.end()){
     int DetNbr = m_FSPC2Detector[TigEvent->channel_number[i]][0];
      int type = m_FSPC2Detector[TigEvent->channel_number[i]][1];
      int FB = m_FSPC2Detector[TigEvent->channel_number[i]][2];

      if(type == 0 ){
        if (FB == 0 ) FillQQQFront(DetNbr , i , TigEvent);
        else if (FB == 1 ) FillQQQBack (DetNbr , i , TigEvent);
      }

      else if (type == 1 ){
        if (FB == 0 ) FillBoxFront (DetNbr , i , TigEvent);
        else if (FB == 1 ) FillBoxBack (DetNbr , i , TigEvent);
      }

      else if (type == 2 ){
        FillPAD(DetNbr , i , TigEvent);
      }

      else{cout << "ERROR: FSPC map incorrectly instantiate" << endl ; exit(1);}
    }
  }
}

// Fill Box
void Sharc::FillBoxFront(int DetNbr, int hit,TMidasEvent* TigEvent){

  // FSPC file is numbered from 0 to 23
  int  StripNbr = m_FSPC2Detector[TigEvent->channel_number[hit]][3] + 1;
  if(StripNbr<25&&StripNbr>0){
    m_data->SetFront_DetectorNbr(m_DetNbrMap[DetNbr]);
    m_data->SetFront_StripNbr(24-StripNbr+1);
    m_data->SetFront_Energy(TigEvent->charge_raw[hit]);
    m_data->SetFront_TimeCFD(TigEvent->cfd_value[hit]);
    m_data->SetFront_TimeLED(TigEvent->led_value[hit]);
  }
  else
    cout << "Wrong Strip Front " << StripNbr << endl;
}
void Sharc::FillBoxBack(int DetNbr, int hit,TMidasEvent* TigEvent){

  // FSPC file is numbered from 0 to 47
  int StripNbr = m_FSPC2Detector[TigEvent->channel_number[hit]][3] + 1;

  // Box Case, ordering is reversed
  if(StripNbr<49 || StripNbr>0){

    m_data->SetBack_DetectorNbr(m_DetNbrMap[DetNbr]);
    m_data->SetBack_StripNbr(48-StripNbr+1);
    m_data->SetBack_Energy(TigEvent->charge_raw[hit]);
    m_data->SetBack_TimeCFD(TigEvent->cfd_value[hit]);
    m_data->SetBack_TimeLED(TigEvent->led_value[hit]);
  }
  else
    cout << "Wrong Strip Back " << StripNbr << endl;
}


void Sharc::FillBoxBack1(int DetNbr, int hit,TMidasEvent* TigEvent){

  // FSPC file is numbered from 0 to 31
  int StripNbr = m_FSPC2Detector[TigEvent->channel_number[hit]][3] + 1;
  // Box Case, ordering is reversed
  //StripNbr = 32 - StripNbr+1;
  if(StripNbr<25){
    StripNbr = 25 - StripNbr;
    m_data->SetBack_DetectorNbr(m_DetNbrMap[DetNbr]);
    m_data->SetBack_StripNbr(StripNbr);
    m_data->SetBack_Energy(TigEvent->charge_raw[hit]);
    m_data->SetBack_TimeCFD(TigEvent->cfd_value[hit]);
    m_data->SetBack_TimeLED(TigEvent->led_value[hit]);
  }
}

void Sharc::FillBoxBack2(int DetNbr, int hit,TMidasEvent* TigEvent){

  // FSPC file is numbered from 0 to 31
  int StripNbr = m_FSPC2Detector[TigEvent->channel_number[hit]][3] + 1;
  // Box Case, ordering is reversed
  //StripNbr = 32 - StripNbr+1;
  if(StripNbr<25){
    StripNbr = 25 - StripNbr;
    m_data->SetBack_DetectorNbr(m_DetNbrMap[DetNbr]);
    m_data->SetBack_StripNbr(StripNbr+24);
    m_data->SetBack_Energy(TigEvent->charge_raw[hit]);
    m_data->SetBack_TimeCFD(TigEvent->cfd_value[hit]);
    m_data->SetBack_TimeLED(TigEvent->led_value[hit]);

  }
}


// Fill QQQ
void Sharc::FillQQQFront(int DetNbr, int hit,TMidasEvent* TigEvent){

  // FSPC file is numbered from 0 to 31
  int  StripNbr = m_FSPC2Detector[TigEvent->channel_number[hit]][3] + 1;

  if(StripNbr<25){
    m_data->SetFront_DetectorNbr(m_DetNbrMap[DetNbr]);
    m_data->SetFront_StripNbr(StripNbr);
    m_data->SetFront_Energy(TigEvent->charge_raw[hit]);
    m_data->SetFront_TimeCFD(TigEvent->cfd_value[hit]);
    m_data->SetFront_TimeLED(TigEvent->led_value[hit]);

  }
}

void Sharc::FillQQQBack(int DetNbr, int hit,TMidasEvent* TigEvent){

  // FSPC file is numbered from 0 to 31
  int StripNbr = m_FSPC2Detector[TigEvent->channel_number[hit]][3] + 1;
  if(StripNbr<25){
    m_data->SetBack_DetectorNbr(m_DetNbrMap[DetNbr]);
    m_data->SetBack_StripNbr(24-StripNbr+1);
    m_data->SetBack_Energy(TigEvent->charge_raw[hit]);
    m_data->SetBack_TimeCFD(TigEvent->cfd_value[hit]);
    m_data->SetBack_TimeLED(TigEvent->led_value[hit]);

  }
}

// Fill PAD
void Sharc::FillPAD(int DetNbr,int hit,TMidasEvent* TigEvent){
  m_data->SetPAD_DetectorNbr(m_DetNbrMap[DetNbr]);
  m_data->SetPAD_Energy(TigEvent->charge_raw[hit]);
  m_data->SetPAD_TimeCFD(TigEvent->cfd_value[hit]);
  m_data->SetPAD_TimeLED(TigEvent->led_value[hit]);

}

map< int,vector<int> > Sharc::GetFSPC2Detector()const{
  return m_FSPC2Detector;
}

int Sharc::atoi(string s){
  int res;
  stringstream(s) >> res;
  return res;

}
