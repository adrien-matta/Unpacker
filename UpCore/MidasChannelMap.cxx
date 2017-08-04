#include "MidasChannelMap.h"
#include <cstdlib>
#include <bitset>
MidasChannelMap* MidasChannelMap::instance = NULL;
////////////////////////////////
MidasChannelMap* MidasChannelMap::getInstance(){
  // A new instance of RootInput is created if it does not exist:
  if (instance == NULL) {
    instance = new MidasChannelMap();
  }
  // The instance of RootInput is returned:
  return instance;
}

////////////////////////////////
void MidasChannelMap::Destroy(){
  delete MidasChannelMap::instance ;
}

////////////////////////////////
MidasChannelMap::MidasChannelMap(){
}

////////////////////////////////
MidasChannelMap::~MidasChannelMap(){
  MidasChannelMap::instance = NULL;
}

////////////////////////////////
void MidasChannelMap::Clear(){
  m_ChannelMap.clear();
}

////////////////////////////////
void MidasChannelMap::PushBackChannel (int ChannelNumber,int FSPCAddress,
                                     int DigitizerType,int Integration,
                                    string Mnemonic,vector<double> CalibCoeff){
  MidasChannel* aChannel = new MidasChannel();
  aChannel->ChannelNumber = ChannelNumber;  
  aChannel->FSPCAddress = FSPCAddress;
  aChannel->DigitizerType = DigitizerType;  
  aChannel->Integration = Integration; 
  aChannel->Mnemonic = Mnemonic;
  aChannel->CalibCoeff = CalibCoeff;
  //fill the channel 
  m_ChannelMap[FSPCAddress] = aChannel;
}

////////////////////////////////
void MidasChannelMap::PrintChannel(int FSPCAddress){
  printf(" FSPC        : %d \n",m_ChannelMap[FSPCAddress]->FSPCAddress); 
  printf(" Number      : %d \n",m_ChannelMap[FSPCAddress]->ChannelNumber);
  printf(" Mnemonic    : %s \n",m_ChannelMap[FSPCAddress]->Mnemonic.c_str());
  printf(" Digitizer   : %d \n",m_ChannelMap[FSPCAddress]->DigitizerType);
  printf(" Integration : %d \n",m_ChannelMap[FSPCAddress]->Integration);
  printf(" CalibCoeff  :");
  for (unsigned i = 0 ;  i <m_ChannelMap[FSPCAddress]->CalibCoeff.size(); i++ ){
    printf("  %.2f  ",m_ChannelMap[FSPCAddress]->CalibCoeff[i]);
  }
  printf("\n");

}

void MidasChannelMap::PrintAllChannels(){

  printf("----------- Printing all channels \n"); 
  for (it=m_ChannelMap.begin(); it!=m_ChannelMap.end(); ++it){
    int fspc = it->first;
    PrintChannel(fspc);
    printf("--\n"); 
  }
  printf("----------- All channels printed \n"); 

}


  int MidasChannelMap::GetChannelNumber(int FSPC){ 
    it = m_ChannelMap.find(FSPC); 
    if (it == m_ChannelMap.end()) {
      //cout << " Warning: address " << std::hex << FSPC << " is not found in the list " << endl;   
      return -1 ; 
    }
    return m_ChannelMap[FSPC]->ChannelNumber; 
  }

  int MidasChannelMap::GetDigitizerType(int FSPC){
    it = m_ChannelMap.find(FSPC); 
    if (it == m_ChannelMap.end()) return -1 ; 
    return m_ChannelMap[FSPC]->DigitizerType; 
  }

  int MidasChannelMap::GetIntegration(int FSPC){
    it = m_ChannelMap.find(FSPC); 
    if (it == m_ChannelMap.end()) return -1 ; 
    return m_ChannelMap[FSPC]->Integration; 
  }

  string MidasChannelMap::GetMnemonic(int FSPC){
    it = m_ChannelMap.find(FSPC); 
    if (it == m_ChannelMap.end()){
      return "empty" ; 
    }
   return m_ChannelMap[FSPC]->Mnemonic; 
 }

  vector<double> MidasChannelMap::GetCalibCoeff(int FSPC) {
    vector<double> zero(0);
    it = m_ChannelMap.find(FSPC); 
    if (it == m_ChannelMap.end()) 
      return zero; 
    return m_ChannelMap[FSPC]->CalibCoeff; 
  }
