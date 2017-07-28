#! /bin/bash  

#echo changes to the envir. variables will be printed on screen
 
# find script path
if [ -n "$ZSH_VERSION" ]; then
   SCRIPTPATH="$( cd "$( dirname "${(%):-%x}" )" && pwd )"
   #echo $ZSH_VERSION ok scriptpath $SCRIPTPATH
elif [ -n "$BASH_VERSION" ]; then
   SCRIPTPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
   #echo $BASH_VERSION ok scriptpath $SCRIPTPATH
else
   echo "neither bash or zsh is used, abort"
   exit 1
fi


# export UNPACKER environment variable
export UNPACKER=$SCRIPTPATH
#echo unpacker directory $UNPACKER

# mac os x case
if [ "${UNPARCH}" = "Darwin" ] ; 
then
  export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$UNPACKER/lib
  export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$UNPACKER/UpCore
  #echo updating the library path $DYLD_LIBRARY_PATH 
else 
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$UNPACKER/lib
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$UNPACKER/UpCore
	#echo updating the library path $LD_LIBRARY_PATH
fi

# no bin is used
export PATH=$PATH:$UNPACKER
#echo updating the path $PATH

alias unpacker='cd $UNPACKER'


