#! /bin/bash

# define paths
printf -v pathUNPDIR $UNPACKER
printf -v path    $PWD

# check whether path is authorized to run script
current=""
if [ "$path" == "$pathUNPDIR" ] ; then
   current="Unpacker"
else
   printf "\r\033[1;31m"
   printf "ERROR: unpacker-cleaner must be used in the following directory\n"
   printf "\t- %s\n" "$pathUNPDIR" 

   exit
fi ;

# if authorized path, warn the user and ask whether (s)he wants to proceed or not
printf "\r\033[1;31m"
printf "********************************************************************************\n"
printf "********************************************************************************\n"
printf "**** You are about to clean %s\n" "$current"
printf "****   -> The folder will be reverted to pre-CMake stage\n"
printf "****   -> Only generated files will be suppressed\n"
printf "****   -> %s will not be usable without recompilation\n" "$current"
printf "****   -> Do you want to proceed? (y/n)\n"
printf "\033[0m"

read answer
# yes case
if [ "$answer" == "y" ] || [ "$answer" == "Y" ] ; then
   printf "Proceeding....\n"
# no case
elif [ "$answer" == "n" ] || [ "$answer" == "N" ] ; then
   printf "Aborting....\n"
   exit
else
# other case
   printf "Reply by y (yes) or n (no), nothing done...\n"
fi ;

############################################################
# doing the actual cleaning 
############################################################

# include directory
for file in ./include/*
do
   if [ -f $file ] ; then
      rm $file
   fi ;
done

# ClassList.txt file
if [ -f ./ClassList.txt ] ; then
   rm ./ClassList.txt
fi

# CMakeCache.txt file
if [ -f ./CMakeCache.txt ] ; then
   rm ./CMakeCache.txt
fi

# install_manifest.txt file
if [ -f ./install_manifest.txt ] ; then
   rm ./install_manifest.txt
fi

# the executable 
if [ -f ./unpacker ] ; then
   rm ./unpacker
fi

# bin directory
if [ -d ./bin ] ; then
   rm -rf ./bin
fi ;

# lib directory
if [ -d ./lib ] ; then
   rm -rf ./lib
fi ;

# .so extention
rm -f `find . -maxdepth 3 -type f -name "*.so"`

# .dylib extention
rm -f `find . -maxdepth 3 -type f -name "*.dylib"`

# .rootmap extention
rm -f `find . -maxdepth 3 -type f -name "*.rootmap"`

# .pcm extention
rm -f `find . -maxdepth 3 -type f -name "*.pcm"`

# dictionnary files
rm -f `find . -maxdepth 3 -type f -name "*Dict*"`

# Makefile files
rm -f `find . -maxdepth 3 -type f -name Makefile`

# cmake_install.cmake files
rm -f `find . -maxdepth 3 -type f -name cmake_install.cmake`

# .ninja files
rm -f `find . -maxdepth 3 -type f -name "*.ninja"`

# CMakeFiles directories
rm -rf `find . -maxdepth 3 -type d -name CMakeFiles`

