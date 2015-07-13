/// Reader Class for MIDAS .mid files from Triumf ISAAC-II DAQ
#ifndef MidasFILE_H
#define MidasFILE_H

#include <string>
using namespace std;

#include "MidasEventFragment.h"

class MidasFile
{
public: // Constructor and Destructor
  MidasFile();
  ~MidasFile(); 

public: // File managing
  bool Open(const char* filename);
  void Close(); 
  const char* GetFilename()  const { return fFilename.c_str();  }
  
public: // Event collection
  bool Read(MidasEventFragment* event);
  //Get error text for the last file error
  const char* GetLastError() const { return fLastError.c_str(); }
  ///< Get error value for the last file error
  const int GetLastErrno() const { return fLastErrno; }
  
private:
  string fFilename; ///< name of the currently open file
  int    fLastErrno; ///< errno from the last operation
  string fLastError; ///< error string from last errno
  bool   fDoByteSwap; ///< "true" if file has to be byteswapped
  int    fFile; ///< open file descriptor
  void*  fGzFile; ///< zlib compressed file reader
  void*  fPoFile; ///< popen() file reader
};

#endif
