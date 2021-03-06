// This class is designed to manage in memory of Bank of Midas Event Fragment waiting to be reassemble
// The class is designed as a Singleton in order to be accessible any place in the code

#ifndef MidasBank_H
#define MidasBank_H

#include <list>
#include <iostream>
#include <string>
using namespace std;

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TH1F.h"

// Unpacker
#include "MidasChannelMap.h"
#include "MidasEventFragment.h"
#include "MidasFile.h"
#include "TMidasEvent.h"


struct EventFragment	{
  
	bool tig10;
	bool tig64;
	bool IsBad;
  
	bool pileup;
  bool overflow;

	bool slowrisetime;
	bool has_waveform;
  
	bool found_eventID;
	bool found_time;
	bool found_charge;
	bool found_channel;
  bool found_trailer;
  bool complete;

	float corrected_charge;
  
	int charge;
  int integration;
	int channel;
	int channel_raw;
  
	int eventId;
	int midasId;
  
	int cfd;
	int led;
	int triggerpattern;
  
	int timestamp_low;
	int timestamp_high;
	int timestamp_live;
	int timestamp_tr;  // triggers requested
	int timestamp_ta;  // triggers accepted
  
	int samplesfound;
  int* wave;
  
  //intiate with default
  EventFragment():tig10(false),tig64(false),IsBad(false),
  found_time(false), found_charge(false), found_channel(false), found_trailer(false), found_eventID(false),complete(false),
  charge(-1),channel(-1),channel_raw(-1),eventId(-1),midasId(-1),
  cfd(-1),led(-1),triggerpattern(-1),
  timestamp_low(-1),timestamp_high(-1),timestamp_live(-1),timestamp_tr(-1),timestamp_ta(-1),
  samplesfound(0){wave=new int[2880];}
  ~EventFragment(){delete wave;}
  
};


class MidasBank{
  
private: // Constructor and destructor are private to insure only one instance of the Bank is in memory
  MidasBank();
  ~MidasBank();
  
public: // check if the singleton is already instantiate and return its pointer
  static MidasBank* getInstance();
  void   Destroy();
private: // Bank of fragment and associate midas file
  map< int, vector<EventFragment*> > m_FragmentBank;
  vector<EventFragment*> m_EventFragmentVector;// store the different fragment until event is completed
  
  unsigned int m_Offset; // this offset is how many event to add at the end of the bank for each found fragments
  MidasFile* m_MidasFile;
  MidasChannelMap* m_MidasChannel;
  
private: // Manage the Fragment Bank
  void Build(unsigned int NumberOfFragment);
  void SetBankOffset(unsigned int Offset);
  void Clear();
  void PushBackFragment();
  void InitTree();
  
private: // channel status
  map< int,bool > m_BadChannel;

public: // call by the user to process the bank
  void Process(unsigned int NumberOfFragment);

public: // Manage the Midas File
  void SetMidasFile(MidasFile* FileName);
  MidasFile* GetMidasFile();
  
private:
  unsigned int m_FragBankSize; // Dynamic number of the fragments in the bank
  unsigned int m_TotalFragment; // Fragments found
  unsigned int m_CompleteFragment;  // Fragments having all expected info
  unsigned int m_IncompleteFragment;   // Fragments not having all expected info
  unsigned int m_UserBadFragment; // Fragments suppressed from user-defined bad channels

private:
   static MidasBank* instance;
  
private: // Process the data from the file
  void UnPackMidasBank(MidasEventFragment* fragment);
  void UnpackTigress(int *data, int size);
  void ReadAnalysisConfig();
  
private: // Root output
  TFile* m_RootFile;
  TTree* m_RootTree;
  TRandom3* m_Random;
  TMidasEvent* m_CurrentMidasEvent;
  
public: // Manage the Root Output
  void SetRootFile(string infile="");
  
};

#endif
