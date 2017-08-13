//c++
#include <fstream>
#include <sstream>
#include <bitset>
#include <cstdlib>
#include <iomanip>      // std::setw


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
  m_FragBankSize=0;
  m_TotalFragment=0;
  m_CompleteFragment=0;
  m_IncompleteFragment=0;
  m_UserBadFragment=0;
  m_MidasChannel = MidasChannelMap::getInstance();
  fill_fspc_list();
  ReadAnalysisConfig();
  m_CurrentMidasEvent = new TMidasEvent(3000);
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

  // Save the Tree
  if(m_RootTree)
    m_RootTree->AutoSave("Overwrite");
  
  // Close the file
  if(m_RootFile){
    m_RootFile->Close();
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
  m_FragBankSize=0;
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

  map< int, vector<EventFragment*> >::iterator it;
  int TreatedFragment = 0 ; // found and not bad
  int UnknownDigitizer = 0 ; // if digitizer is NOT recognised (subset of treated)
  int EventNumber = 0; // Good events (containing at least a good fragment)
  double AverageFragNumber = 0;
  int currentID = -1;

  cout << "Starting Bank Processing " << endl;
  // Loop over the rest of the element
  while(m_FragBankSize!=0){
    // loop over the fragment map
    for(it = m_FragmentBank.begin(); it!=m_FragmentBank.end();){
      currentID = it->first;
      m_EventFragmentVector = it->second;
      unsigned int numFrag = m_EventFragmentVector.size();
      unsigned int numGoodFrag = 0;
      // Clear the Fragment from previous stuff
      m_CurrentMidasEvent->Clear();
  
      // Fill the event with the new fragment
      for(unsigned int g = 0 ; g < numFrag ; g++){ // loops on all fragments in one event
        //process fragment only if it's good
        if (!m_EventFragmentVector[g]->IsBad){
          TreatedFragment++; // general counter for the whole file
          numGoodFrag++; // local, for every event
          m_CurrentMidasEvent->tig_midas_id.push_back( m_EventFragmentVector[g]->midasId);
          
          if(m_EventFragmentVector[g]->tig10){
            m_CurrentMidasEvent->tig_type.push_back(0);
          }
          else if(m_EventFragmentVector[g]->tig64){
            m_CurrentMidasEvent->tig_type.push_back(1);
          }
          else{
            cout <<"ERROR: Digitizer Card type is UNKNOWN" << endl ;
            UnknownDigitizer++;
          }

          m_CurrentMidasEvent->channel_number.push_back(m_EventFragmentVector[g]->channel);
          m_CurrentMidasEvent->channel_raw.push_back(m_EventFragmentVector[g]->channel_raw);

          m_CurrentMidasEvent->charge_raw.push_back(m_EventFragmentVector[g]->charge);
          m_CurrentMidasEvent->charge_cal.push_back(m_EventFragmentVector[g]->charge);

          m_CurrentMidasEvent->cfd_value.push_back(m_EventFragmentVector[g]->cfd);
          m_CurrentMidasEvent->led_value.push_back(m_EventFragmentVector[g]->led);

          m_CurrentMidasEvent->timestamp_low.push_back(m_EventFragmentVector[g]->timestamp_low);
          m_CurrentMidasEvent->timestamp_high.push_back(m_EventFragmentVector[g]->timestamp_high);
          m_CurrentMidasEvent->timestamp_live.push_back(m_EventFragmentVector[g]->timestamp_live);
          m_CurrentMidasEvent->timestamp_tr.push_back(m_EventFragmentVector[g]->timestamp_tr); 
          m_CurrentMidasEvent->timestamp_ta.push_back(m_EventFragmentVector[g]->timestamp_ta); 

          int name_offset = 0;
          while(gDirectory->FindObjectAny(Form("wf_%i",name_offset))){
            name_offset++;
          }
          name_offset++;

          if(m_EventFragmentVector[g]->samplesfound>0){
            TH1F h  = TH1F(Form("wf_%i",name_offset),Form("wf_%i",name_offset),m_EventFragmentVector[g]->samplesfound,0,m_EventFragmentVector[g]->samplesfound);
            h.SetDirectory(0);
            for(int wl = 0 ; wl<m_EventFragmentVector[g]->samplesfound ;wl++){
              h.Fill(wl,m_EventFragmentVector[g]->wave[wl]);
            }
            m_CurrentMidasEvent->waveform.push_back(h);
          }
          else{
            TH1F h  = TH1F(Form("wf_%i",name_offset),Form("wf_%i",name_offset),1,0,1);
            m_CurrentMidasEvent->waveform.push_back(h);  
          }

          if(EventNumber)
            AverageFragNumber+=(1./(EventNumber))*(m_EventFragmentVector.size()-AverageFragNumber);
        } // end of: if(not a bad fragment)

        delete m_EventFragmentVector[g];
      }// end loop on all fragments

      // if we have good fragments fill num_channel hit and the event ID and treat
      if(numGoodFrag>0){
        EventNumber++;
        m_CurrentMidasEvent->tig_num_chan=numGoodFrag;
        m_CurrentMidasEvent->tig_event_id=currentID;
        user_point->EventAction(m_CurrentMidasEvent);
      }

      // fill the bank tree
      if(m_RootFile!=NULL)
        m_RootTree->Fill();

      m_EventFragmentVector.clear();
      // remove the entry
      it = m_FragmentBank.erase(it); // the returned iterator points to the next element
      m_FragBankSize-=numFrag;
      // add some more to replace the removed one
      for(int i = 0 ; i < numFrag ; i++)
        PushBackFragment();
      
      if(EventNumber%10000==0){
        cout << "\r Treated Fragments: " << TreatedFragment/1000000. << " M |"
          <<" Built events: " << EventNumber/1000.<<" k |"
          <<" Avg. size: " << AverageFragNumber <<" |"
          <<" Bank size: " << m_FragBankSize/1000.<<" k |"
          <<" Unknown digitizer: " << UnknownDigitizer  << flush;
        //printf("\r Treated Fragments: %.2fM | Built events: %.2fk | Avg. size: %.2f | Bank size: %.2fk | Unknown digitizer: %d",
        //TreatedFragment/1e6,EventNumber/1e3,AverageFragNumber,m_FragBankSize/1e3, UnknownDigitizer);
      }

    }// end of for loop on Fragment Bank
  }// end of while loop on fragment bank size

  cout << endl << "Processing terminated: "
       << EventNumber << " Events reconstructed" << endl ;

   cout<< std::setw(10)<< m_TotalFragment      << " Fragments found, out of which: \n"  
       << std::setw(10)<< m_CompleteFragment   << " complete         (\% total): " << 100.0* m_CompleteFragment/m_TotalFragment << endl   
       << std::setw(10)<< m_IncompleteFragment << " incomplete       (\% total): " << 100.0* m_IncompleteFragment/m_TotalFragment << endl  
       << std::setw(10)<< m_UserBadFragment    << " bad channels     (\% total): " << 100.0* m_UserBadFragment/m_TotalFragment << endl  
       << std::setw(10)<< TreatedFragment      << " treated (good)   (\% total): " << 100.0* TreatedFragment/m_TotalFragment << endl;

  cout << "Missing " << m_FragmentBank.size() << endl ;
  cout << "Remaining BankSize " << m_FragBankSize << endl ;
  cout << "Last treated event " << currentID << endl ;
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
  EventFragment* fragment = new EventFragment; 
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
            fragment->wave[fragment->samplesfound++] = -temp;
          }
          else {
            fragment->wave[fragment->samplesfound++] = value & 0x00001fff;
          }

          if ((value >> 14) & 0x00002000) {
            int temp =  (value >> 14) & 0x00003fff;
            temp = ~temp;
            temp = (temp & 0x00001fff) + 1;
            fragment->wave[fragment->samplesfound++] = -temp;
          }
          else {
            fragment->wave[fragment->samplesfound++] = (value >> 14) & 0x00001fff;
          }
          break;

      case 0x1: // trapeze data
          //currently not used.
          break;

      case 0x4: // std::bitset<16> Time
          //time = true;
          fragment->found_time = true;
          fragment->slowrisetime = (value & 0x0000000f);
          fragment->cfd          = (value & 0x0ffffff0) >> 4;
          fragment->cfd = fragment->cfd*10+fragment->slowrisetime*0.625;
          break;

      case 0x5: // Charge
          fragment->found_charge = true;
          if(fragment->IsBad){ // ignore if channel is bad
            fragment->charge = -1;
            fragment->overflow  = -1;
            fragment->pileup   = -1;
            break;
          }

          if(fragment->tig10)	{
            double rand = m_Random->Uniform();
            fragment->overflow  = (value & 0x08000000)>>26;
            fragment->pileup   = (value & 0x02000000)>>25;
            if((value & 0x02000000) != 0u) { // true if there's pile-up
              fragment->charge = (-((~(static_cast<int32_t>(value) & 0x01ffffff)) & 0x01ffffff) + 1);
              fragment->charge = (rand + static_cast<double> (fragment->charge))/static_cast<double>(fragment->integration);
            } else {
               fragment->charge = (value & 0x03ffffff);
               fragment->charge = (rand + static_cast<double> (fragment->charge))/static_cast<double>(fragment->integration);
            }
          }
          else if(fragment->tig64) {
            double rand = m_Random->Uniform();  
            fragment->overflow = (value & 0x00800000)>>22;
            fragment->pileup   = (value & 0x00200000)>>21;  
            if((value & 0x00200000) != 0u) { // true if there's pile-up
               fragment->charge = (-((~(static_cast<int32_t>(value) & 0x001fffff)) & 0x001fffff) + 1);
               fragment->charge = (rand + static_cast<double> (fragment->charge))/static_cast<double>(fragment->integration);
            } else {
               fragment->charge = ((value & 0x003fffff));
               fragment->charge = (rand + static_cast<double> (fragment->charge))/static_cast<double>(fragment->integration);
            }
          }
          else{ 
            cout << "ERROR: " << (error++) << ". INFO: Charge extracting problem, [CHANNEL " << hex << fragment->channel_raw ;            
            cout << "] Unknown card type => Defaulting to tig-10 "<< endl ;
            fragment->found_charge = false;
            double rand = m_Random->Uniform();
            fragment->overflow = (value & 0x08000000)>>26;
            fragment->pileup   = (value & 0x02000000)>>25;
            if((value & 0x02000000) != 0u) { // true if there's pile-up
               fragment->charge = (-((~(static_cast<int32_t>(value) & 0x01ffffff)) & 0x01ffffff) + 1);
               fragment->charge = (rand + static_cast<double> (fragment->charge))/static_cast<double>(fragment->integration);
            } else {
               fragment->charge = (value & 0x03ffffff);
               fragment->charge = (rand + static_cast<double> (fragment->charge))/static_cast<double>(fragment->integration);
            }
          }
          break;

      case 0x6: // leading edge
          fragment->led = (value & 0x0ffffff0)>>4;
          break;

      case 0x8: // Event header
          fragment->found_eventID=true;
          current_eventId = (value & 0x00ffffff); // format: 0x80NNNNNN 
          m_TotalFragment++;
          break;

    /*  case 0xa0000000:{ // timestamp
            int time[5];
            time[0]  = *(data + x);
            x += 1;
            time[1] =	*(data + x);	//& 0x0fffffff;
            if( (time[1] & 0xf0000000) != 0xa0000000) {
              if( ( (time[1] & 0xf0000000) == 0xc0000000) &&  ( ((time[1] & 0x000000ff) == 0x0000003f) || ((time[1] & 0x000000ff) == 0x0000001f) ) ) { x-=1; break;}
              fragment->IsBad = true;
              //printf("timestamp probelm 1.\t%08x\t%08x\t%08x\t%i\n",time[0],time[1], 0xa0000000,x ); //PrintBank(data,size);
              break;
            }
            if( ((time[0] & 0x0f000000)==0) && ((time[1] & 0x0f000000)==0) ) { //tig64
              fragment->timestamp_low = time[0] & 0x00ffffff;
              fragment->timestamp_high = time[1] & 0x00ffffff;
            }
            else { //tig10
              fragment->timestamp_low = time[0] & 0x00ffffff;
              fragment->timestamp_high = time[1] & 0x00ffffff;
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
                    fragment->timestamp_low = (time[nstamp] & 0x00ffffff);
                    break;
                  case 0x01000000:
                    fragment->timestamp_high = (time[nstamp] & 0x00ffffff);
                    break;
                  case 0x02000000:
                    fragment->timestamp_live = (time[nstamp] & 0x00ffffff);
                    break;
                  case 0x04000000:
                    fragment->timestamp_tr = (time[nstamp] & 0x00ffffff);
                    break;
                  case 0x08000000:
                    fragment->timestamp_ta = (time[nstamp] & 0x00ffffff);
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
          fragment->timestamp_low = (value & 0x00ffffff);
        else if((value & 0x0f000000)==0x01000000)
          fragment->timestamp_high = (value & 0x00ffffff);
        else if((value & 0x0f000000)==0x02000000)
          fragment->timestamp_live = (value & 0x00ffffff);
        else if((value & 0x0f000000)==0x04000000)
          fragment->timestamp_tr = (value & 0x00ffffff);
        else if((value & 0x0f000000)==0x08000000)
          fragment->timestamp_ta = (value & 0x00ffffff);
        else{
          cout << "Time stamp datum incorrectly formatted : " << hex << dword << endl;
        }

      case 0xb: // Trigger Pattern
          fragment->triggerpattern = value;
          break;

      case 0xc: // port info, fspc,  New Channel
          fragment->found_channel = true;
          fragment->channel_raw =  dword & 0x00ffffff ; // 0x00[S00PCC]
          fragment->channel =  m_MidasChannel->GetChannelNumber(dword & 0x00ffffff);
          slave   = (dword & 0x00f00000)>>20;
          port    = (dword & 0x00000f00)>>8;
          channel = (dword & 0x000000ff);
          //cout << hex <<  fragment->channel_raw << endl; 
          //cout << std::bitset<32>(channel_raw) << "\n" << std::bitset<32>((channel_raw & 0x0ffffff0)) <<endl ;

          // tag bad channel and break
          if(m_MidasChannel->GetChannelNumber(fragment->channel_raw)<0){
          //if(m_BadChannel[fragment->channel_raw]){ 
          //cout <<dec<< m_MidasChannel->GetChannelNumber(fragment->channel_raw) 
          //  << " => " << hex<< fragment->channel_raw << endl;
            fragment->IsBad=true;
            fragment->tig10 = false;
            fragment->tig64 = false;
            fragment->integration = -1;
            m_UserBadFragment++;
            break; 
            }
          else
            fragment->IsBad=false;

          if(m_MidasChannel->GetDigitizerType(dword & 0x00ffffff)==64){
            fragment->tig10 = false;
            fragment->tig64 = true;
            fragment->integration = m_MidasChannel->GetIntegration(dword & 0x00ffffff);
          }
          else if (m_MidasChannel->GetDigitizerType(dword & 0x00ffffff)==10){
            fragment->tig10 = true;
            fragment->tig64 = false;
            fragment->integration = m_MidasChannel->GetIntegration(dword & 0x00ffffff);
          }
          else{
            fragment->tig10 = false;
            fragment->tig64 = false;
            fragment->integration = m_MidasChannel->GetIntegration(dword & 0x00ffffff);
          }

          break;

      case 0xe: // Event Trailer
         if(current_eventId!=(value&0x00ffffff)){
            fragment->found_trailer = false; 
         }
         fragment->found_trailer = true; 
          break;

      case 0xf: // EventBuilder Timeout
          cout << "ERROR: found type: " << hex <<  type << " word: " << hex << dword << dec << endl;
          break;

      default:
          cout << "ERROR: found unknown type: " << hex << type << " word: " << hex << dword  << endl;;
          break;
    };

    // if the fragment has no missing information, push back on the event and make a new fragment
    if(fragment->found_time && fragment->found_charge 
    && fragment->found_channel && fragment->found_trailer 
    && current_eventId>-1 ){
      m_CompleteFragment++;
      m_FragBankSize++; // this counter is dynamic!
      fragment->eventId = current_eventId;
      m_FragmentBank[current_eventId].push_back(fragment);

      if(x!=size-1){
        cout << " WARNING: Found a complete fragment before reaching the size!!! \n " << endl; 
        //cin.get();
        fragment = new EventFragment; // start a new fragment
      }
      else
        fragment = 0; // Flag the end of a complete fragment
    }  
  }// end of for loop iterating on ONE fragment of the given size

  // if there's a fragment, but missing an information: inform and delete
  if(fragment &&
   !(fragment->found_time && fragment->found_charge && 
    fragment->found_channel && fragment->found_trailer &&
    fragment->found_eventID) ){

    m_IncompleteFragment++;
    cout << "\nERROR: Incomplete fragment remnant, Fragment event ID: " <<  fragment->eventId << endl;
    /*cout << "\t?found time    " << fragment->found_time   << endl;
    cout << "\t?found charge  " << fragment->found_charge << endl;
    cout << "\t?found channel " << fragment->found_channel << endl;
    cout << "\t?found eventID " << fragment->found_eventID << endl;
    cout << "\t?found trailer " << fragment->found_trailer << endl;*/
    delete fragment;
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

  string outfile(infile,(infile.find(".mid")-9),infile.find(".mid")-(infile.find(".mid")-9));
  outfile += ".root";
  string temp = UnpackerOptionManager::getInstance()->GetBankOutputPath()+"bank";
  outfile = temp+outfile;
  printf("Bank tree: %s\n",outfile.c_str());
  //gDirectory->cd();
  m_RootFile = new TFile(outfile.c_str(),"RECREATE");
  if(!m_RootFile->IsOpen()) { printf("issues opening the root output file....\n");exit(1); }
 
  cout << "Creating bank tree : " << UnpackerOptionManager::getInstance()->GetBankOutputName() << endl;

}

//////////////
void MidasBank::InitTree(){
  if(m_RootFile!=NULL){
    m_RootTree = new TTree(UnpackerOptionManager::getInstance()->GetBankOutputName().c_str(),UnpackerOptionManager::getInstance()->GetBankOutputName().c_str());
    m_RootTree->Branch( "MidasEvent" , "TMidasEvent" , &m_CurrentMidasEvent );
  }
  else {
    printf("The bank root file is not opened....\n");exit(1);
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
