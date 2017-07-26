
#include "UserPoint.h"
#include "UnpackerOptionManager.h"
#include<cstdlib>

VUserPoint* UserPoint::getInstance(){
  
  if(instance==0)
    instance = new UserPoint();
  
  return instance;
}

void UserPoint::Destroy(){
  if (instance != 0) {
    delete (UserPoint*)instance;
    instance = 0 ;
  }
}

UserPoint::UserPoint(){
  m_sharc = new Sharc();
  m_tig = new Tigress();
  m_trifoil = new Trifoil();
  string infile = UnpackerOptionManager::getInstance()->GetInputFileName();
  InitOutput(infile);
  
  m_RunNumber = UnpackerOptionManager::getInstance()->GetRunNumber();
  m_RunNumberMinor = UnpackerOptionManager::getInstance()->GetRunNumberMinor();

}

UserPoint::~UserPoint(){
  // Save the Tree
  if(m_tree)
    m_tree->AutoSave("Overwrite");
  
  // Close the file
  if(m_outputFile){
    m_outputFile->Close();
  }
}

TTree* UserPoint::GetRootTree(){
  return m_tree;
}

void UserPoint::InitOutput(string infile){
  if(infile.find(".mid") == infile.npos) { printf("can't read midas file\n"); return ; }
	string outfile(infile,(infile.find(".mid")-9),infile.find(".mid")-(infile.find(".mid")-9));
	outfile += ".root";
	string temp = UnpackerOptionManager::getInstance()->GetUserOutputPath()+"data";
	outfile = temp+outfile;
	printf("user tree: %s\n",outfile.c_str());
  gDirectory->cd();
	m_outputFile = new TFile(outfile.c_str(),"RECREATE");
	if(!m_outputFile->IsOpen()) { printf("issues opening the root output file....\n");exit(1); }
 
  cout << "Creating user tree : " << UnpackerOptionManager::getInstance()->GetUserOutputName() << endl;
  
  m_tree = new TTree(UnpackerOptionManager::getInstance()->GetUserOutputName().c_str(),UnpackerOptionManager::getInstance()->GetUserOutputName().c_str());
  TSharcData* m_sharcdata = m_sharc->GetData();
  TTigressData* m_tigressdata = m_tig->GetData();
  TTrifoilData* m_trifoildata = m_trifoil->GetData();
  
  m_tree->Branch( "Sharc" , "TSharcData" , m_sharcdata );
  m_tree->Branch( "Tigress" , "TTigressData" , m_tigressdata );
  m_tree->Branch( "Trifoil" , "TTrifoilData" , m_trifoildata );
  m_tree->Branch( "RunNumber" , &m_RunNumber ,"m_RunNumber/I");
  m_tree->Branch( "RunNumberMinor" , &m_RunNumberMinor ,"m_RunNumberMinor/I");

}

void UserPoint::EventAction(TMidasEvent* TigEvent){
  m_sharc->Clear();
  m_tig->Clear();
  m_trifoil->Clear();
  
  m_sharc->FillData(TigEvent);
  m_tig->FillData(TigEvent);  

  m_trifoil->FillData(TigEvent);
  m_tree->Fill();
}

void UserPoint::EndOfRunAction(){
  m_tree->AutoSave();
  Destroy();
}

void UserPoint::BeginOfRunAction(){
  m_sharc->ReadFSPCFile("Config.txt");
  m_tig->ReadFSPCFile("Config.txt");
  m_trifoil->ReadFSPCFile("Config.txt");
}
