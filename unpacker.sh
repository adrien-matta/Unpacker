#!bin/bash   
CUR_DIR="$PWD" 
SCRIPTFILE=$0 
if [ "${SCRIPTFILE}" = "-bash" ] ; then  
  SCRIPTFILE=${BASH_ARGV[0]} 
elif [ "${SCRIPTFILE}" = "bash" ] ; then  
  SCRIPTFILE=${BASH_ARGV[0]} 
fi 

SCRIPTPATH="${SCRIPTFILE}" 
export UNPACKER=$(dirname $SCRIPTPATH)
# mac os x case
if [ "${NPARCH}" = "Darwin" ] ; 
then
  export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$UNPACKER/lib
else 
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$UNPACKER/lib
fi
