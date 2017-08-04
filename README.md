Preparation:
 - Create the configuration ascii file (strict name "Config.txt") that links the detector to the DAQ address.
 - Create the bad channels ascii file (strict name "BadChannels.txt") to ignore the data from specific DAQ addresses if necessary.

Installation 
 - make sure that unpacker.sh and scripts/unpacker-cleaner.sh etc.. are all executables (> chmod u+x filename.sh)
 - > cd unpacker
 - > source unpacker.sh (or add it to bashrc)
 -  > './scripts/unpacker-cleaner' (Cleaning gets you to the pre-cmake stage! only use it if need be)
 - > cmake ./    
 - > make -jn install

Execute:
> ./unpacker  path-to-Midas-file/runxxxxx_yyy.mid --user-output-name rawData --no-bank-tree --user-output-path   path-to-/<Raw-Root-Data-dir>/  --intial-load 100000 --maximum-load 500000

----------------------------------------------------------

Future enhancement: 
 - Give the config file as an argument