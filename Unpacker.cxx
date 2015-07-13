#include<string>
#include<iostream>
using namespace std;

#include "MidasFile.h"
#include "MidasBank.h"
#include "UserPoint.h"
#include "TFile.h"
#include "UnpackerOptionManager.h"

int main(int argc, char*argv[])	{
  
  // it is mandatory that the UnpoackerOptionManager is the first to be instantiate as other depend on it
  // The singleton parse the user option and then dispatch them to whoever request it
  UnpackerOptionManager::getInstance()->ReadUserOption(argc,argv);

  if(!UnpackerOptionManager::getInstance()->GetNoUserPoint())
    UserPoint::getInstance();
  
  // Process the Midas File
  MidasBank::getInstance()->Process( UnpackerOptionManager::getInstance()->GetInitialBankLoad() );
  
  // Save all the Tree information
    MidasBank::getInstance()->Destroy();
	return 0;
}
