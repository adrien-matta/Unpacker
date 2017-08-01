// This class is designed to manage in memory of Bank of Midas Event Fragment waiting to be reassemble
// The class is designed as a Singleton in order to be accessible any place in the code

#ifndef MidasChannelMap_H
#define MidasChannelMap_H

#include <list>
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;


struct MidasChannel	{
  int ChannelNumber;  
	int FSPCAddress; // e.g.  0x0900a00
  int DigitizerType; // 10 for tig10, 64 for tig64 
  int Integration; // e.g. 125 for Tig10, 25 for Tig64
  string Mnemonic; // e.g. TIG05BN00A
  vector<double> CalibCoeff; // e.g. [2]x2+[1]x+[0]

  MidasChannel():ChannelNumber(-1), FSPCAddress(0xffffffff), DigitizerType(0),Integration(1),Mnemonic(""){;}
  ~MidasChannel(){;}
};


class MidasChannelMap{
  
private: // Constructor and destructor are private to insure only one instance of the Bank is in memory
  MidasChannelMap();
  ~MidasChannelMap();
  
public: // check if the singleton is already instantiate and return its pointer
  static MidasChannelMap* getInstance();
  void   Destroy();
private: 
  map< int, MidasChannel* > m_ChannelMap; // a map storing the channel info against the channel number (index)
  map< int, MidasChannel* >::iterator it; // iterator

private: // Manage the Fragment Bank
  void Clear();

private:
   static MidasChannelMap* instance;
   
public:
//Setter 
  void PushBackChannel(int ChannelNumber,int FSPCAddress,int DigitizerType,int Integration,
                       string Mnemonic,vector<double> CalibCoeff);
//Getters 
  int GetSize() {return m_ChannelMap.size(); };
  int GetChannelNumber(int FSPC) ;
  int GetDigitizerType(int FSPC) ;
  int GetIntegration(int FSPC)   ;
  string GetMnemonic(int FSPC)   ;
  vector<double> GetCalibCoeff(int FSPC) ;

  void PrintChannel(int FSPC) ;
  void PrintAllChannels();

};

#endif
