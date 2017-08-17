#include "UnpackerOptionManager.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
UnpackerOptionManager* UnpackerOptionManager::instance = 0;

////////////////////////////////////
UnpackerOptionManager* UnpackerOptionManager::getInstance(){
  if(instance==0)
    instance = new UnpackerOptionManager();
    
    return instance;
}

////////////////////////////////////
UnpackerOptionManager::UnpackerOptionManager(){
  m_InputFileName="";
  m_UserOutputPath="";
  m_BankOutputPath="";
  m_UserOutputName="UnpackerUserTree";
  m_BankOutputName="UnpackerBankTree";
  
  m_InitialBankLoad = 100000;
  m_MaximumBankLoad = -1;
  m_NoBankTree = false;
  m_NoUserPoint = false;
  m_RunNumber=0;
  m_RunNumberMinor=0;
  m_VerboseLevel=1; // Defautl will show Errors only
}

////////////////////////////////////
UnpackerOptionManager::~UnpackerOptionManager(){
}

////////////////////////////////////
void UnpackerOptionManager::ReadUserOption(int argc, char*argv[]){
  
  if(argc==1){
    cout << "At least one argument is needed, run unpacker -h for help display" << endl ;
    exit(1);
    
  }
  
  bool check = false ;
  for (int i = 1; i < argc; i++) {
    string argument = argv[i];
    
    if (argument == "-H" || argument == "-h" || argument == "--help") DisplayHelp();
    else if ((argument == "-ut" || argument == "--user-output-tree-name") && argc >= i + 1) {
      m_UserOutputName = argv[++i] ;
    }
    else if ((argument == "-bt" || argument == "--bank-output-tree-name") && argc >= i + 1) {
      m_BankOutputName = argv[++i] ;
    }
    else if ((argument == "-up" || argument == "--user-output-path") && argc >= i + 1) {
      m_UserOutputPath = argv[++i] ;
    }
    else if ((argument == "-bp" || argument == "--bank-output-path") && argc >= i + 1) {
      m_BankOutputPath = argv[++i] ;
    }
    else if ((argument == "-il" || argument == "--intial-load") && argc >= i + 1) {
      m_InitialBankLoad = atoi(argv[++i]) ;
    }
    else if ((argument == "-ml" || argument == "--maximum-load") && argc >= i + 1) { 
      m_MaximumBankLoad = atoi(argv[++i]) ;
    }
    else if ((argument == "-v" || argument == "--verbose") && argc >= i + 1)  {
      m_VerboseLevel = atoi(argv[++i]); 
      cout << "Verbose Level: "<< m_VerboseLevel<<endl;
    } 
    else if (argument == "-nb" || argument == "--no-bank-tree"){
      m_NoBankTree = true ;
    }
    else if (argument == "-nu" || argument == "--no-user-point"){
      m_NoUserPoint = true ;
    }

    // assume it is the input file name
    else if(!check){
      check = true ;
      m_InputFileName = argv[i];
      if(m_InputFileName.find(".mid") == m_InputFileName.npos) {
        printf("Input file is not a midas file\n"); exit(1) ;
      }
      
      string RunNumber(m_InputFileName,(m_InputFileName.find(".mid")-9),m_InputFileName.find(".mid")-(m_InputFileName.find(".mid")-5));
      string RunNumberMinor(m_InputFileName,(m_InputFileName.find(".mid")-3),m_InputFileName.find(".mid")-(m_InputFileName.find(".mid")-3));

      m_RunNumber = atoi(RunNumber.c_str());
      m_RunNumberMinor = atoi(RunNumberMinor.c_str());
      cout << "Run Number Major " << m_RunNumber << endl;
      cout << "Run Number Minor " << m_RunNumberMinor << endl;
    }
    
    else{
      cout << "Wrong option, run unpacker -h for help display" << endl;
      exit(1);
    }
  }

}

////////////////////////////////////
void UnpackerOptionManager::DisplayHelp(){
  cout << "/////////////////////////// Unpacker Help Menu: ///////////////////////////" << endl;
  cout << "Typical command line: ./unpacker <option> <midasfile>" << endl ;
  cout << "  -> .mid extension is mandatory for <midasfile> " << endl ;
  cout << "  -> only one midas file is allowed " << endl <<endl ;
  
  cout << "/////////////////////////////// Option List ///////////////////////////////" << endl;
  cout << "-h -H --help \t \t \t Display this help" << endl ;
  cout << "-ut or --user-output-tree-name <arg> \t Set <arg> as the UserPoint issued TTree name " << endl;
  cout << "-bt or --bank-output-tree-name <arg> \t Set <arg> as the Bank issued TTree name " << endl;
  cout << "-up or --user-output-path <arg> \t Create the UserPoint issued TTree in path <arg> " << endl;
  cout << "-bp or --bank-output-path <arg> \t Create the Bank issued TTree in path <arg> " << endl;
  cout << "-il or --intial-load <arg> \t \t Load the bank initially with <arg> fragment  " << endl;
  cout << "-v or --verbose <arg> \t \t sets the verbose level to <arg>; silent 0, errors 1, +warnings 2 etc..  " << endl;

  cout << "-nb or --no-bank-tree \t \t \t The Bank issued TTree is not generated (Run faster)" << endl;
  cout << "-nu or --no-user-point \t \t The UserPoint is not run at all" << endl;
  cout << "///////////////////////////////////////////////////////////////////////////" << endl;

  exit(1);
}
