#include <string>
#include <iostream>
using namespace std;

#include "MidasFile.h"
#include "MidasBank.h"
#include "UserPoint.h"
#include "TFile.h"
#include "UnpackerOptionManager.h"

int main(int argc, char*argv[])	{


//time at start
  time_t timer_start,timer_stop;
  char buffer[38];
  struct tm* tm_info;
  time(&timer_start);
  tm_info = localtime(&timer_start);
  strftime(buffer, 26, "Start time: %Y-%m-%d %H:%M:%S\n", tm_info);
  puts(buffer);

  //time elapsed (accurate in case required)
  struct timespec start, finish;
  double elapsed;
  //get time at start
  clock_gettime(CLOCK_MONOTONIC, &start);

//Treatment
    // it is mandatory that the UnpoackerOptionManager is the first to be instantiate as other depend on it
    // The singleton parse the user option and then dispatch them to whoever request it
    UnpackerOptionManager::getInstance()->ReadUserOption(argc,argv);

    if(!UnpackerOptionManager::getInstance()->GetNoUserPoint())
      UserPoint::getInstance();
    
    // Process the Midas File
    MidasBank::getInstance()->Process( UnpackerOptionManager::getInstance()->GetInitialBankLoad() );
    
    // Save all the Tree information
      MidasBank::getInstance()->Destroy();
 
  //get time at the end
  clock_gettime(CLOCK_MONOTONIC, &finish);
  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  
  //Show time
  int seconds = elapsed;
  int hours = seconds/3600;
  seconds = seconds%3600;
  int minutes = seconds/60;
  seconds = seconds%60;
  printf("Time elapsed %.2d:%.2d:%.2d:%.2f\n",hours,minutes,seconds,elapsed-seconds);

	return 0;
}
