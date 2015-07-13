#ifndef MidasEventFragment_H
#define MidasEventFragment_H

///
/// C++ class representing one midas event.
///
/// Objects of this class are created by reading
/// midas events from a file, by reading
/// them from a midas shared memory buffer or by
/// receiving them through the mserver
///

// First defines the data structures written
// into MIDAS .mid files. They define the on-disk
// data format, they cannot be arbitrarily changed.
#include <stdint.h>
#include <iostream>
using namespace std;
/// Event header
struct EventHeader_t {
  uint16_t fEventId;      ///< event id
  uint16_t fTriggerMask;  ///< event trigger mask
  uint32_t fSerialNumber; ///< event serial number
  uint32_t fTimeStamp;    ///< event timestamp in seconds
  uint32_t fDataSize;     ///< event size in bytes
};

/// Bank header

struct BankHeader_t {
  uint32_t fDataSize;
  uint32_t fFlags;
};

/// 16-bit data bank

struct Bank_t {
  char fName[4];      ///< bank name
  uint16_t fType;     ///< type of data (see midas.h TID_xxx)
  uint16_t fDataSize;
};

/// 32-bit data bank

struct Bank32_t {
  char fName[4];      ///< bank name
  uint32_t fType;     ///< type of data (see midas.h TID_xxx)
  uint32_t fDataSize;
};

/// MIDAS event
class MidasEventFragment
{
 public:
  // houskeeping functions
  MidasEventFragment(); ///< default constructor
  MidasEventFragment(const MidasEventFragment &); ///< copy constructor
  ~MidasEventFragment(); ///< destructor
  MidasEventFragment& operator=(const MidasEventFragment &); ///< assignement operator
  void Clear(); ///< clear event for reuse
  void Copy(const MidasEventFragment &); ///< copy helper
  void Print(const char* option = "") const; ///< show all event information

  // get event information
  uint16_t GetEventId() const;      ///< return the event id
  uint16_t GetTriggerMask() const;     ///< return the triger mask
  uint32_t GetSerialNumber() const; ///< return the serial number
  uint32_t GetTimeStamp() const; ///< return the time stamp (unix time in seconds)
  uint32_t GetDataSize() const; ///< return the event size

  // get data banks
  const char* GetBankList() const; ///< return a list of data banks
  int FindBank(const char* bankName, int* bankLength, int* bankType, void **bankPtr) const;
  int LocateBank(const void *unused, const char* bankName, void **bankPtr) const;

  bool IsBank32() const; ///< returns "true" if event uses 32-bit banks
  int IterateBank(Bank_t **, char **pdata) const; ///< iterate through 16-bit data banks
  int IterateBank32(Bank32_t **, char **pdata) const; ///< iterate through 32-bit data banks

  // helpers for event creation
  EventHeader_t* GetEventHeader(); ///< return pointer to the event header
  char* GetData(); ///< return pointer to the data buffer

  void AllocateData(); ///< allocate data buffer using the existing event header
  void SetData(uint32_t dataSize, char* dataBuffer); ///< set an externally allocated data buffer

  int SetBankList(); ///< create the list of data banks, return number of banks
  bool IsGoodSize() const; ///< validate the event length

  void SwapBytesEventHeader(); ///< convert event header between little-endian (Linux-x86) and big endian (MacOS-PPC) 
  int  SwapBytes(bool); ///< convert event data between little-endian (Linux-x86) and big endian (MacOS-PPC) 

protected:
  EventHeader_t fEventHeader; ///< event header
  char* fData;     ///< event data buffer
  int  fBanksN;    ///< number of banks in this event
  char* fBankList; ///< list of bank names in this event
  bool fAllocatedByUs; ///< "true" if we own the data buffer
};

#endif
