//c++
#include <fstream>
#include <sstream>
#include <bitset>
#include <cstdlib>

//Unpacker
#include "MidasBank.h"
#include "VUserPoint.h"
#include "UnpackerOptionManager.h"

MidasBank* MidasBank::instance = NULL;
////////////////////////////////
MidasBank* MidasBank::getInstance(){
  // A new instance of RootInput is created if it does not exist:
  if (instance == NULL) {
    instance = new MidasBank();
  }
  // The instance of RootInput is returned:
  return instance;
}

////////////////////////////////
void MidasBank::Destroy(){
  delete MidasBank::instance ;
}

////////////////////////////////
MidasBank::MidasBank(){
  m_Offset =1;
  m_BankSize=0;
  m_AllFragment=0;
  m_GoodFragment=0;
  m_MidasChannel = MidasChannelMap::getInstance();
  fill_fspc_list();
  ReadAnalysisConfig();
  m_CurrentEvent = new TMidasEvent(3000);
  m_RootFile = NULL;
  m_RootTree = NULL;
  m_Random = new TRandom3();

  string infile = UnpackerOptionManager::getInstance()->GetInputFileName();
  MidasFile* myMidasFile = new MidasFile();
  myMidasFile->Open(infile.c_str());

  SetMidasFile(myMidasFile);

  if(!UnpackerOptionManager::getInstance()->GetNoBankTree())
    SetRootFile(infile);
  TH1::AddDirectory(kFALSE);  
}

////////////////////////////////
MidasBank::~MidasBank(){
  MidasBank::instance = NULL;
  if(m_RootFile!=NULL){
    m_RootTree->AutoSave();
  }
}

////////////////////////////////
void MidasBank::Build(unsigned int NumberOfFragment){
  unsigned int i=0;
  for (i = 0 ; i< NumberOfFragment; i++) {
    if(i%1000==0){
      cout << "\r Initial Loading : " << i/1000. << "k fragments" << flush;
    }
    PushBackFragment();
  }
  cout << "\r Initial Loading : " << i/1000. << "k fragment" << endl;

}

////////////////////////////////
void MidasBank::Clear(){
  m_FragmentBank.clear();
  m_BankSize=0;
}

////////////////////////////////
void MidasBank::PushBackFragment(){
  MidasEventFragment* myFragment = new MidasEventFragment();

  if(m_MidasFile->Read(myFragment)){
    if(myFragment->GetEventId()==1){
      UnPackMidasBank(myFragment);
    }
  }

  delete myFragment;

}

////////////////////////////////
void MidasBank::Process(unsigned int NumberOfFragment){
  VUserPoint* user_point=VUserPoint::getInstance();
  user_point->BeginOfRunAction();
  InitTree();

  // build the initial fragment bank
  Build(NumberOfFragment);

  bool check_end = false;
  cout << "Starting Bank Processing " << endl;
  map< int, vector<EventFragment*> >::iterator it;
  int NumberFragment = 0 ; // only if digitizer is recognised
  int UnknownFragment = 0 ; // if digitizer is NOT recognised
  int currentID = 0;
  int position = 0;
  int EventNumber = 0;
  double average_fragment = 0;
  double average_pushback = 0;
  int sizeBefore= 0 ;
  int sizeAfter=0 ;
  int NumberOfPushBack=0;
  unsigned int LastPosition = 0 ;

  // Loop over the rest of the element
  while(m_BankSize!=0){
    // loop over the fragment map
    for(it = m_FragmentBank.begin(); it!=m_FragmentBank.end();){
      currentID = it->first;
      m_eventfragment = it->second;
      EventNumber++;
      // Clear the Fragment from precedent stuff
      m_CurrentEvent->Clear();
      // Fill the event with the new fragment
      unsigned int mysize = m_eventfragment.size();
      for(unsigned int g = 0 ; g < mysize ; g++){ // loops on all fragments in one event
        m_CurrentEvent->tig_num_chan=mysize;
        m_CurrentEvent->tig_event_id= currentID;
        m_CurrentEvent->tig_midas_id.push_back( m_eventfragment[g]->midasId);

        if(m_eventfragment[g]->tig10){
          m_CurrentEvent->tig_type.push_back(0);
          NumberFragment++;
        }
        else if(m_eventfragment[g]->tig64){
          m_CurrentEvent->tig_type.push_back(1);
          NumberFragment++;
        }
        else{
          cout <<"type unknown!" << endl ;
          UnknownFragment++;
        }

        m_CurrentEvent->channel_number.push_back(m_eventfragment[g]->channel);
        m_CurrentEvent->channel_raw.push_back(m_eventfragment[g]->channel_raw);
        m_CurrentEvent->cfd_value.push_back(m_eventfragment[g]->cfd);
        m_CurrentEvent->led_value.push_back(m_eventfragment[g]->led);
        m_CurrentEvent->charge_raw.push_back(m_eventfragment[g]->charge);
        m_CurrentEvent->charge_cal.push_back(m_eventfragment[g]->charge);

        m_CurrentEvent->timestamp_low.push_back(m_eventfragment[g]->timestamp_low);
        m_CurrentEvent->timestamp_high.push_back(m_eventfragment[g]->timestamp_high);
        m_CurrentEvent->timestamp_live.push_back(m_eventfragment[g]->timestamp_live);
        m_CurrentEvent->timestamp_tr.push_back(m_eventfragment[g]->timestamp_tr); 
        m_CurrentEvent->timestamp_ta.push_back(m_eventfragment[g]->timestamp_ta); 

        int name_offset = 0;
        while(gDirectory->FindObjectAny(Form("wf_%i",name_offset))){
          name_offset++;
        }
        name_offset++;

        if(m_eventfragment[g]->samplesfound>0){

          TH1F h  = TH1F(Form("wf_%i",name_offset),Form("wf_%i",name_offset),m_eventfragment[g]->samplesfound,0,m_eventfragment[g]->samplesfound);
          h.SetDirectory(0);
          for(int wl = 0 ; wl<m_eventfragment[g]->samplesfound ;wl++){
            h.Fill(wl,m_eventfragment[g]->wave[wl]);
          }
          m_CurrentEvent->waveform.push_back(h);
        }

        else{
          TH1F h  = TH1F(Form("wf_%i",name_offset),Form("wf_%i",name_offset),1,0,1);
          m_CurrentEvent->waveform.push_back(h);  

        }
        average_fragment+=(1./(EventNumber))*(m_eventfragment.size()-average_fragment);
        delete m_eventfragment[g];
      }

      user_point->EventAction(m_CurrentEvent);

      if(m_RootFile!=NULL)
        m_RootTree->Fill();

      m_eventfragment.clear();
      // remove the entry
      it = m_FragmentBank.erase(it);
      m_BankSize-=mysize;
      // add some more to replace the removed one
      for(int i = 0 ; i < mysize ; i++){
        PushBackFragment();
      }
      if(EventNumber%10000==0){
        cout << "\r  " << NumberFragment/1000000. << "M frag. treated |"
          <<" Build:  " << EventNumber<<" |"
          <<" Avg. size: " << average_fragment << " Avg.PB: " << average_pushback <<" |"
          <<" Bank size : " << m_BankSize<<" |"
          <<" Unknown digitizer : " << UnknownFragment  << flush;
      }
    }
  }

  cout << endl << "Processing terminated: "
    << NumberFragment<< " fragments treated"
    <<", " << EventNumber << " Events reconstructed " << endl ;

  cout << " All read fragments " << m_AllFragment 
       << "     good Fragments " << m_GoodFragment 
       << "      ratio:" << 100.0* m_GoodFragment/m_AllFragment << endl ;

  cout << "Missing " << m_FragmentBank.size() << endl ;
  user_point->EndOfRunAction();
}

////////////////////////////////
void MidasBank::SetMidasFile(MidasFile* FileName){
  m_MidasFile = FileName;
}

////////////////////////////////
MidasFile* MidasBank::GetMidasFile(){
  return m_MidasFile;
}

////////////////////////////////
void MidasBank::SetBankOffset(unsigned int Offset){
  m_Offset = Offset ;
}
/////////////////////////
void MidasBank::UnPackMidasBank(MidasEventFragment* fragment) {

  int NumberOfBanks = fragment->SetBankList();

  Bank32_t* banks = new Bank32_t[NumberOfBanks];
  void** d_ptr = new void*[NumberOfBanks];

  int *bank_name = new int[NumberOfBanks];
  int *bank_type = new int[NumberOfBanks];
  int *bank_size = new int[NumberOfBanks];

  for(int k=0;k<NumberOfBanks;k++)	{
    bank_name[k]=*(int*)(fragment->GetBankList()+k*4);
    memcpy(banks[k].fName,fragment->GetBankList()+k*4,4);
  };


  int temp1, temp2;

  for(int k=0;k<NumberOfBanks;k++)	{
    fragment->FindBank(banks[k].fName,&temp1,&temp2,d_ptr+k);
    bank_size[k] = temp1;
    banks[k].fDataSize = temp1;
    bank_type[k] = temp2;
    //datatype_counter[bank_type[k]]++;
    banks[k].fType = temp2;
  }
  for(int k=0;k<NumberOfBanks;k++)
    UnpackTigress((int*)(d_ptr[k]),bank_size[k]);

  delete[] bank_size;
  delete[] bank_type;
  delete[] bank_name;
  delete[] d_ptr;
  delete[] banks;

}

/////////////////////////
void MidasBank::UnpackTigress(int *data, int size)	{
  int error =0;
  int current_eventId = -1;

  //cout << " ----------------- " << size << endl ; 
  // the fragment contains data words of size 
  EventFragment* eventfragment = new EventFragment; 
  for(int x=0; x<size ;x++)	{
    int dword =	*(data+x);
    unsigned int type	 =	(dword & 0xf0000000)>>28;
    unsigned int value =	(dword & 0x0fffffff);
    int port,slave,channel;
  
    //std::cout << std::hex << type << " "  << value << std::endl;
    switch(type)	{

      case 0x0: // waveform data
          if (value & 0x00002000) {
            int temp =  value & 0x00003fff;
            temp = ~temp;
            temp = (temp & 0x00001fff) + 1;
            eventfragment->wave[eventfragment->samplesfound++] = -temp;
          }
          else {
            eventfragment->wave[eventfragment->samplesfound++] = value & 0x00001fff;
          }

          if ((value >> 14) & 0x00002000) {
            int temp =  (value >> 14) & 0x00003fff;
            temp = ~temp;
            temp = (temp & 0x00001fff) + 1;
            eventfragment->wave[eventfragment->samplesfound++] = -temp;
          }
          else {
            eventfragment->wave[eventfragment->samplesfound++] = (value >> 14) & 0x00001fff;
          }
          break;

      case 0x1: // trapeze data
          //currently not used.
          break;

      case 0x4: // std::bitset<16> Time
          //time = true;
          eventfragment->found_time = true;
          eventfragment->slowrisetime = (value & 0x0000000f);
          eventfragment->cfd          = (value & 0x0ffffff0) >> 4;
          //cout << hex << ((value & 0x0ffffff0) >> 2) << "\n" << ((value & 0x0ffffff0) >> 4) << endl; 
          //cout << dec << ((value & 0x0ffffff0) >> 2) << "\n" << ((value & 0x0ffffff0) >> 4) <<endl ;
          //cout << std::bitset<32>(value) << "\n" << std::bitset<32>((value & 0x0ffffff0)) <<endl ;
          //cout << std::bitset<32>((value & 0x0ffffff0) >> 2) << "\n" << std::bitset<32>((value & 0x0ffffff0) >> 4) <<endl ;
          //cin.get();
          eventfragment->cfd = eventfragment->cfd*10+eventfragment->slowrisetime*0.625;
          break;

      case 0x5: // Charge
          eventfragment->found_charge = true;
         /* if(eventfragment->IsBad){ // ignore if channel is bad
            break;
          }*/

          if(eventfragment->tig10)	{
            double rand = m_Random->Uniform();
            eventfragment->overflow  = (value & 0x08000000)>>26;
            eventfragment->pileup   = (value & 0x02000000)>>25;
            if((value & 0x02000000) != 0u) { // true if there's pile-up
              eventfragment->charge = (-((~(static_cast<int32_t>(value) & 0x01ffffff)) & 0x01ffffff) + 1);
              eventfragment->charge = (rand + static_cast<double> (eventfragment->charge))/static_cast<double>(eventfragment->integration);
            } else {
               eventfragment->charge = (value & 0x03ffffff);
               eventfragment->charge = (rand + static_cast<double> (eventfragment->charge))/static_cast<double>(eventfragment->integration);
            }
          }
          else if(eventfragment->tig64) {
            double rand = m_Random->Uniform();  
            eventfragment->overflow = (value & 0x00800000)>>22;
            eventfragment->pileup   = (value & 0x00200000)>>21;  
            if((value & 0x00200000) != 0u) { // true if there's pile-up
               eventfragment->charge = (-((~(static_cast<int32_t>(value) & 0x001fffff)) & 0x001fffff) + 1);
               eventfragment->charge = (rand + static_cast<double> (eventfragment->charge))/static_cast<double>(eventfragment->integration);
            } else {
               eventfragment->charge = ((value & 0x003fffff));
               eventfragment->charge = (rand + static_cast<double> (eventfragment->charge))/static_cast<double>(eventfragment->integration);
            }
          }
          else{ 
            cout << "ERROR: " << (error++) << ". INFO: Charge extracting problem, CHANNEL " << hex << eventfragment->channel_raw ;            cout << ". Unknown card type (Defaulting to tig-10) "<< endl ;
            eventfragment->found_charge = false;
            double rand = m_Random->Uniform();
            eventfragment->overflow = (value & 0x08000000)>>26;
            eventfragment->pileup   = (value & 0x02000000)>>25;
            if((value & 0x02000000) != 0u) { // true if there's pile-up
               eventfragment->charge = (-((~(static_cast<int32_t>(value) & 0x01ffffff)) & 0x01ffffff) + 1);
               eventfragment->charge = (rand + static_cast<double> (eventfragment->charge))/static_cast<double>(eventfragment->integration);
            } else {
               eventfragment->charge = (value & 0x03ffffff);
               eventfragment->charge = (rand + static_cast<double> (eventfragment->charge))/static_cast<double>(eventfragment->integration);
            }
          }
          break;

      case 0x6: // leading edge
          eventfragment->led = (value & 0x0ffffff0)>>4;
          break;

      case 0x8: // Event header
          current_eventId = (value & 0x00ffffff); // format: 0x80NNNNNN 
          m_AllFragment++;
          break;

    /*  case 0xa0000000:{ // timestamp
            int time[5];
            time[0]  = *(data + x);
            x += 1;
            time[1] =	*(data + x);	//& 0x0fffffff;
            if( (time[1] & 0xf0000000) != 0xa0000000) {
              if( ( (time[1] & 0xf0000000) == 0xc0000000) &&  ( ((time[1] & 0x000000ff) == 0x0000003f) || ((time[1] & 0x000000ff) == 0x0000001f) ) ) { x-=1; break;}
              eventfragment->IsBad = true;
              //printf("timestamp probelm 1.\t%08x\t%08x\t%08x\t%i\n",time[0],time[1], 0xa0000000,x ); //PrintBank(data,size);
              break;
            }
            if( ((time[0] & 0x0f000000)==0) && ((time[1] & 0x0f000000)==0) ) { //tig64
              eventfragment->timestamp_low = time[0] & 0x00ffffff;
              eventfragment->timestamp_high = time[1] & 0x00ffffff;
            }
            else { //tig10
              eventfragment->timestamp_low = time[0] & 0x00ffffff;
              eventfragment->timestamp_high = time[1] & 0x00ffffff;
              x += 1;
              time[2] =	*(data+x);//	& 0x0fffffff;
              if( (time[2] & 0xf0000000) != 0xa0000000) {
                x = x-1;  /// If a tig10 is missing some timestamp words.
                break;
              }

              x += 1;
              time[3] =	*(data+x);//	& 0x0fffffff;
              if( (time[3] & 0xf0000000) != 0xa0000000) {
                //printf("timestamp probelm 3.\t%08x\t%08x\t%08x\t%i\n",time[0],time[3], 0xa0000000,x ); 
                break;}
              x += 1;
              time[4] =	*(data+x);//	& 0x0fffffff;
              if( (time[4] & 0xf0000000) != 0xa0000000) {
                //printf("timestamp probelm 4.\t%08x\t%08x\t%08x\t%i\n",time[0],time[4], 0xa0000000,x ); 
                break;}
              for(int nstamp =0; nstamp<5; nstamp++)	{
                int subtype = (time[nstamp] & 0x0f000000);
                switch(subtype)	{
                  case 0x00000000:
                    eventfragment->timestamp_low = (time[nstamp] & 0x00ffffff);
                    break;
                  case 0x01000000:
                    eventfragment->timestamp_high = (time[nstamp] & 0x00ffffff);
                    break;
                  case 0x02000000:
                    eventfragment->timestamp_live = (time[nstamp] & 0x00ffffff);
                    break;
                  case 0x04000000:
                    eventfragment->timestamp_tr = (time[nstamp] & 0x00ffffff);
                    break;
                  case 0x08000000:
                    eventfragment->timestamp_ta = (time[nstamp] & 0x00ffffff);
                    break;
                  default:
                    //printf("timestamp probelm default.\t%08x\t%08x\t%i\n",time[1], 0xa0000000,x );
                    break;
                };
              }
            }
          }
          //has_timestamp = true;
          break;
      */

      case 0xa:
        if((value & 0x0f000000)==0x00000000)
          eventfragment->timestamp_low = (value & 0x00ffffff);
        else if((value & 0x0f000000)==0x01000000)
          eventfragment->timestamp_high = (value & 0x00ffffff);
        else if((value & 0x0f000000)==0x02000000)
          eventfragment->timestamp_live = (value & 0x00ffffff);
        else if((value & 0x0f000000)==0x04000000)
          eventfragment->timestamp_tr = (value & 0x00ffffff);
        else if((value & 0x0f000000)==0x08000000)
          eventfragment->timestamp_ta = (value & 0x00ffffff);
        else{
          cout << "Time stamp datum incorrectly formatted : " << hex << dword << endl;
        }

      case 0xb: // Trigger Pattern
          eventfragment->triggerpattern = value;
          break;

      case 0xc: // port info, fspc,  New Channel
          eventfragment->found_channel = true;
          eventfragment->channel_raw =  dword & 0x00ffffff ; // 0x00[S00PCC]
          eventfragment->channel =  m_MidasChannel->GetChannelNumber(dword & 0x00ffffff);
          slave   = (dword & 0x00f00000)>>20;
          port    = (dword & 0x00000f00)>>8;
          channel = (dword & 0x000000ff);
          if(m_MidasChannel->GetDigitizerType(dword & 0x00ffffff)==64){
            eventfragment->tig10 = false;
            eventfragment->tig64 = true;
            eventfragment->integration = m_MidasChannel->GetIntegration(dword & 0x00ffffff);
          }
          else if (m_MidasChannel->GetDigitizerType(dword & 0x00ffffff)==10){
            eventfragment->tig10 = true;
            eventfragment->tig64 = false;
            eventfragment->integration = m_MidasChannel->GetIntegration(dword & 0x00ffffff);
          }
          else{
            eventfragment->tig10 = false;
            eventfragment->tig64 = false;
            eventfragment->integration = m_MidasChannel->GetIntegration(dword & 0x00ffffff);
          }
          // tag bad channel
         /* if(m_BadChannel[eventfragment->channel_raw]){ 
            eventfragment->IsBad=true;
            }*/

          break;

      case 0xe: // Event Trailer
         if(current_eventId!=(value&0x00ffffff)){
            eventfragment->found_trailer = false; 
         }
         eventfragment->found_trailer = true; 
         //cin.get();
          break;

      case 0xf: // EventBuilder Timeout
          cout << "Event builder error, builder timed out ,found type: " << hex <<  type << " word: " << hex << dword ;
          break;

      default:
          cout << "Unpacking error: found unknown type: " << hex << type << " word: " << hex << dword  << endl;;
          break;
    };

    // if the fragment is good and no missing information, push back on the event and make a new fragment
    if(/*!eventfragment->IsBad 
    &&*/ eventfragment->found_time && eventfragment->found_charge 
    && eventfragment->found_channel && eventfragment->found_trailer 
    && current_eventId>-1 ){
      m_GoodFragment++;
      //cout << " storing event " << current_eventId << "\n xxxxxxxxxxxxxxxx " << endl; 
      eventfragment->eventId = current_eventId;
      m_FragmentBank[current_eventId].push_back(eventfragment);
      ++m_BankSize;
      if(x!=size-1){
        cout << " Found a new fragment before reaching the size!!! \n " << endl; 
        cin.get();
        eventfragment = new EventFragment; // start a new fragment
      }
      else
        eventfragment = 0; // end of fragment
    }

  }// end of for loop iterating on one fragment of the given size

  // if there's a fragment, that is not bad, but missing an information, inform and delete
  if(eventfragment /*&& !eventfragment->IsBad */
  && ! (eventfragment->found_time && eventfragment->found_charge 
  && eventfragment->found_channel && eventfragment->found_trailer
  && eventfragment->found_eventID) ){
    cout << "\nincomplete fragment remain" <<  eventfragment->eventId << endl;
    /*cout << "\t?found time    " << eventfragment->found_time   << endl;
    cout << "\t?found charge  " << eventfragment->found_charge << endl;
    cout << "\t?found channel " << eventfragment->found_channel << endl;
    cout << "\t?found eventID " << eventfragment->found_eventID << endl;
    cout << "\t?found trailer " << eventfragment->found_trailer << endl;*/
    delete eventfragment;
  }

}


//////////////
void MidasBank::fill_fspc_list()	{
 
  string FSPCPath="Config.txt";
  ifstream FSPCFile;
  FSPCFile.open(FSPCPath.c_str());
  
  if(!FSPCFile.is_open()) {cout << "Error: FSPC File: " << FSPCPath << " not found " << endl ; exit(1);}
  else {cout << "Midas Bank is Reading FSPC file : " << FSPCPath << endl ;}
  
  string LineBuffer, DataBuffer;
  string ChannelNumberStr, FSPCAddressStr, CardTypeStr, Mnemonic;
  int    ChannelNumber,FSPCAddress, CardType;
  string CalibCoeffStr;
  vector<double> CalibCoeff;

  while(!FSPCFile.eof()){
    getline(FSPCFile,LineBuffer);
    if (LineBuffer.size()==0) continue;
    CalibCoeff.clear();
    stringstream myLine(LineBuffer);
    myLine >> DataBuffer >> ChannelNumberStr 
           >> DataBuffer >> FSPCAddressStr 
           >> DataBuffer >> CardTypeStr 
           //>> DataBuffer >> IntegrationStr 
           >> Mnemonic
           >> DataBuffer;
    
    unsigned index=0;
    double coef; 
    while(myLine>>CalibCoeffStr){
      //cout << index << " " << CalibCoeffStr << " ";
      stringstream(CalibCoeffStr) >> coef;
      CalibCoeff.push_back(coef);
      index++;       
    }

    ChannelNumberStr = ChannelNumberStr.substr(0,ChannelNumberStr.length()-1);
    stringstream(ChannelNumberStr) >> ChannelNumber;

    FSPCAddressStr = FSPCAddressStr.substr(0,FSPCAddressStr.length()-1);
    FSPCAddress = (int)strtol(FSPCAddressStr.c_str(),NULL,16);

    //IntegrationStr = IntegrationStr.substr(0,IntegrationStr.length()-1);
    //stringstream(IntegrationStr) >> Integration;

    CardTypeStr = CardTypeStr.substr(0,CardTypeStr.length()-1);
    CardTypeStr = CardTypeStr.substr(3,CardTypeStr.length());
    stringstream(CardTypeStr) >> CardType;
  
    int Integration = 125;
    if (CardType==64) 
      Integration=25; 

    if(FSPCAddress==-1) continue; // skip the empty channels
    m_MidasChannel->PushBackChannel(ChannelNumber,FSPCAddress,CardType, Integration, Mnemonic, CalibCoeff);
    //m_MidasChannel->PrintChannel(FSPCAddress);
  }

  cout << " Total number of operational channels " << m_MidasChannel->GetSize() << endl;

}


//////////////
void MidasBank::SetRootFile(string infile){
  if(infile=="")
    return;

  if(infile.find(".mid") == infile.npos)
  { printf("can't read midas file\n"); return ; }

  string outfile(infile,(infile.find(".mid")-5),infile.find(".mid")-(infile.find(".mid")-5));
  outfile += ".root";
  string temp = UnpackerOptionManager::getInstance()->GetBankOutputPath()+"bank";
  outfile = temp+outfile;
  printf("Bank tree: %s\n",outfile.c_str());
  m_RootFile = new TFile(outfile.c_str(),"RECREATE");
  if(!m_RootFile->IsOpen()) { printf("issues opening the root output file....\n");exit(1); }

}

//////////////
void MidasBank::InitTree(){
  if(m_RootFile!=NULL){
    m_RootTree = new TTree(UnpackerOptionManager::getInstance()->GetBankOutputName().c_str(),UnpackerOptionManager::getInstance()->GetBankOutputName().c_str());
    m_RootTree->Branch( "MidasEvent" , "TMidasEvent" , &m_CurrentEvent );
  }
}


//////////////
///////////////////////////////////////////////////////////////////////////
void MidasBank::ReadAnalysisConfig(){
  bool ReadingStatus = false;

  // path to file
  string FileName = "BadChannels.txt";

  // open analysis config file
  ifstream AnalysisConfigFile;
  AnalysisConfigFile.open(FileName.c_str());

  if (!AnalysisConfigFile.is_open()) {
    cout << "INFO: The file " << FileName << " is not found, all addresses are considered operational "<< endl;
    return;
  }
  cout << " INFO: Loading user restrictions on Bad FSPC addresses from " << FileName << endl;

  // read analysis config file
  string LineBuffer,DataBuffer,whatToDo;
  while (!AnalysisConfigFile.eof()) {
    // Pick-up next line
    getline(AnalysisConfigFile, LineBuffer);

    // search for "header"
    if (LineBuffer.compare(0, 12, "ConfigFSPC") == 0) ReadingStatus = true;

    // loop on tokens and data
    while (ReadingStatus ) {

      whatToDo="";
      AnalysisConfigFile >> whatToDo;

      // Search for comment symbol (%)
      if (whatToDo.compare(0, 1, "%") == 0) {
        AnalysisConfigFile.ignore(numeric_limits<streamsize>::max(), '\n' );
      }
      else if (whatToDo== "DISABLE_CHANNEL") { //disable this channel number 
        AnalysisConfigFile >> DataBuffer;
        cout << whatToDo << "  " << DataBuffer << endl; // e.g. DataBuffer = CLOVER03
        int channel = (int)strtol(DataBuffer.c_str(),NULL,16);
        m_BadChannel[channel] = true;
      }
      else {
        ReadingStatus = false;
      }
    }
  }
}
