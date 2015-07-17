#include"MidasBank.h"
#include"VUserPoint.h"
#include"UnpackerOptionManager.h"
#include<cstdlib>
#include<bitset>
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
  fill_fspc_list();
  m_CurrentEvent = new TMidasEvent(3000);
  m_RootFile = NULL;
  m_RootTree = NULL;
  
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
list<EventFragment*>::iterator MidasBank::PopElementFragment(list<EventFragment*>::iterator it){
  m_eventfragment.push_back((*it));

  // The pointer is placed to the next fragment
  it=m_FragmentBank.erase(it);
  // return to the previous fragment (current fragment)
  --it;
  --m_BankSize;
  return it;
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
  list<EventFragment*>::iterator it;
  int NumberFragment = 0 ;
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
    EventNumber++;
    // Took the first fragment and considere it belong to the current event
    it=m_FragmentBank.begin();
    currentID = (*it)->eventId;
    PopElementFragment(it);
    PushBackFragment();
    // Clear the event from previous value

    LastPosition=0;

    for (it=m_FragmentBank.begin() , position = 1; it!=m_FragmentBank.end(); ++it , ++position) {
      LastPosition++;
      if((*it)->eventId==currentID){
        it = PopElementFragment(it);
        NumberFragment++;

        // Take the longest distance between two fragment so far and add 10% as a safety offset
        if(LastPosition>m_Offset/1.1)
          
            m_Offset=LastPosition*1.1;
        
        /* every time a fragment belonging to the current event is found,
           we check how many fragments are ahead and load the appropriate
           number of fragments */
        

        while(!check_end && (m_BankSize - position) < m_Offset){
          if(!check_end){
            if(UnpackerOptionManager::getInstance()->GetMaximumBankLoad()>0 && m_BankSize>UnpackerOptionManager::getInstance()->GetMaximumBankLoad())
              break;
            sizeBefore = m_BankSize;
            PushBackFragment();
            NumberOfPushBack++;
            sizeAfter =  m_BankSize;
            average_pushback +=(1./(NumberOfPushBack))*((sizeAfter-sizeBefore)-average_pushback);
          }
          
          if(!check_end && string(m_MidasFile->GetLastError())=="EOF"){
            cout << endl << "\t Reaching end of file " << endl;
            check_end=true;
          }
        }
      }
      
      if(NumberFragment%100000==0){
        cout << "\r  " << NumberFragment/1000000. << "M frag. treated |"
        <<" Build:  " << EventNumber
        <<" Avg. size: " << average_fragment << " Avg.PB: " << average_pushback <<" | "
        <<"Bank Status: size : " << m_BankSize << " , "
        <<"Offset : " << m_Offset << " " << flush;
      }
    }
    
    average_fragment+=(1./(EventNumber))*(m_eventfragment.size()-average_fragment);
    // Clear the Fragment from precedent stuff
    m_CurrentEvent->Clear();
    // Fill the event with the new fragment
    unsigned int size = m_eventfragment.size();
    for(unsigned int g = 0 ; g < size ; g++){
      m_CurrentEvent->tig_num_chan=size;
      m_CurrentEvent->tig_event_id= currentID;
     
      m_CurrentEvent->tig_midas_id.push_back( m_eventfragment[g]->midasId);
     
      if(m_eventfragment[g]->tig10)
        m_CurrentEvent->tig_type.push_back(0);
      else if(m_eventfragment[g]->tig64)
        m_CurrentEvent->tig_type.push_back(1);
      else
        cout <<"type unknown!" << endl ;
      
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

       delete m_eventfragment[g];
    }

    user_point->EventAction(m_CurrentEvent);
    
    if(m_RootFile!=NULL)
      m_RootTree->Fill();
    
    m_eventfragment.clear();

  }
  cout << endl << "\r Processing terminated: "
       << NumberFragment<< " fragments treated"
       <<", " << EventNumber << " Events reconstructed " << endl ;
  
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
  EventFragment* eventfragment = new EventFragment;

	for(int x=0; x<size ;x++)	{
		int dword =	*(data+x);
		unsigned int type	=	(dword & 0xf0000000); //>> 28;
		int slave =	(dword & 0x0ff00000) >> 20;
	  int value =	(dword & 0x0fffffff);
   // int value =	(dword & 0x00ffffff);

		switch(type)	{
			case 0x00000000: // waveform data
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
				//print = false;
				break;
      case 0x10000000: // trapeze data
                       //currently not used.
      	break;
      case 0x40000000: // CFD Time
                       //time = true;
				eventfragment->found_time = true;
				eventfragment->slowrisetime = value & 0x08000000;
				eventfragment->cfd = value & 0x07ffffff;
        break;

      case 0x50000000: // Charge
				eventfragment->found_charge = true;

				if(eventfragment->tig10)	{
				//	eventfragment->pileup = value & 0x00010000;
					//eventfragment->charge	= (value &	0x0000ffff);
				  eventfragment->charge	= (value &	0x0fffffff);
        }
				else if(eventfragment->tig64) {
          eventfragment->charge	= value; //(value &	0x0fffffff);
				}
				else{ 
          printf("%i  problem extracting charge.\n", error++);
          eventfragment->found_charge = false;
        }
        break;
        
      case 0x60000000: // led ?? leading edge!
				eventfragment->led = value & 0x07ffffff;
        break;
      case 0x80000000: // Event header
        current_eventId = value;

				break;
      case 0xa0000000:{ // timestamp
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
      case 0xb0000000: // Trigger Pattern
				eventfragment->triggerpattern = value;
				break;
      case 0xc0000000: // port info,  New Channel
                       // if(eventfragment->found_channel)
                       //cout<<"reaching new event before first one finnish" << endl ;
				eventfragment->found_channel = true;
				eventfragment->channel =  FSPC_to_channel(dword & 0x00ffffff);
        eventfragment->channel_raw =  dword & 0x00ffffff ;
       // cout << "eee " <<std::hex<< value << " " << std::dec <<value<<  endl;

				if(slave<3) {eventfragment->tig64 = true;}
				else{eventfragment->tig10 = true;}
        break;
      case 0xe0000000: // Event Trailer
				break;
      case 0xf0000000: // EventBuilder Timeout
        printf("Event builder error, builder timed out ,found type: %08x\n", type);
				break;
      default:
				printf("Unpacking error: found unknown type.\t%08x\t%i\n",type,x);
				break;
		};
    
    if(eventfragment->found_time && eventfragment->found_charge && eventfragment->found_channel&&current_eventId>-1){
      eventfragment->eventId = current_eventId;
      m_FragmentBank.push_back(eventfragment);
      ++m_BankSize;
      eventfragment = new EventFragment;
    }
	}
  if(!(eventfragment->found_time && eventfragment->found_charge && eventfragment->found_channel&&eventfragment->found_eventID))
    delete eventfragment;
}


//////////////
void MidasBank::fill_fspc_list()	{
	fstream infile;
	infile.open("fspc2ch.h");
	int index = 0;
	if(infile.is_open() )	{
		string line;
		while( getline(infile,line) )	{
			fspc_list[index] = (int)strtol(line.c_str(),NULL,16);		  
      index++;
		}
	}
  
	if(index < 2048)	{
		while(index<2048)	{
			fspc_list[index++] = 0;
		}
	}
}

//////////////
int MidasBank::FSPC_to_channel(int fspc)	{
  for(int i=0;i<2048;i++) {
    if(fspc==fspc_list[i]){return i;}
  }
  return -1;
}

//////////////
void MidasBank::SetRootFile(string infile){
  if(infile=="")
      return;
  
  if(infile.find(".mid") == infile.npos)
    { printf("can't read midas file\n"); return ; }
	
  string outfile(infile,(infile.find(".mid")-5),infile.find(".mid")-(infile.find(".mid")-5));
	outfile += ".root";
	string temp = UnpackerOptionManager::getInstance()->GetBankOutputPath()+"raw";
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
