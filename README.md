Preparation:
• create the Configuration file that links the detector to the DAQ address
• This file should be named "Config.txt"
• create the $UNPACKER/fspc2ch.h 

Installation 
• make sure that unpacker.sh and scripts/unpacker-cleaner.sh etc... are all executables
	∘ chmod u+x <filename>.sh ...
• source unpacker.sh (or add it to bashrc)
• Cleaning: see below (this cleaning gets you to the pre-cmake stage!)
• cmake ./    
• make -jn install

Execute:
./unpacker  path-to-Midas-file/runxxxxx_yyy.mid --user-output-name rawData --no-bank-tree --user-output-path   path-to-/<Raw-Root-Data-dir>/  --intial-load 100000 --maximum-load 500000

----------------------------------------------------------

Future enhancement: 
• Give the config file as an argument
• Write a code to produce the fspc2ch.h file automatically









