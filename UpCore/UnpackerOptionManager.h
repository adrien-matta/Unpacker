// This class is designed to read the user option and store them
// the rest of the code can access them by requesting the instance the UnpackerOptionManager (UOM) singleton

#ifndef UnpackerOptionManager_H
#define UnpackerOptionManager_H

#include<string>
using namespace std;

class UnpackerOptionManager{
  
private: // Constructor and destructor are private to insure only one instance of the UOM is in memory
  UnpackerOptionManager();
  ~UnpackerOptionManager();
  
private:
  static UnpackerOptionManager* instance ;
  
public: // check if the singleton is already instantiate and return its pointer
  static UnpackerOptionManager* getInstance();
  void   Destroy();
  
public:
  void ReadUserOption(int argc, char*argv[]);
  void DisplayHelp();
  
private: // Output trees related
  string m_InputFileName;
  string m_UserOutputPath; // Path for the UserPoint issued tree
  string m_BankOutputPath; // Path for the MidasBank issued tree
  string m_UserOutputName; // Name of the UserPoint issued tree
  string m_BankOutputName; // Name of the Midas Bank issued tree
  int    m_RunNumber; // extract the run number from the mid file name, so it can be use by the user
  int    m_RunNumberMinor; // extract the run number from the mid file name, so it can be use by the user

private: // Bank related
  unsigned int m_InitialBankLoad; // Number of fragment to load initially in the bank
  unsigned int m_MaximumBankLoad; // Maximum Number to be load in the bank, no limit if negative (default)
  
private:
  bool m_NoBankTree ; // if true, the Midas Bank does not issue any tree (run faster)
  bool m_NoUserPoint; // if true the UserPoint is not instantiate (a VUserPOint doing nothing is instantiate instead)

public:
  inline string GetInputFileName() { return m_InputFileName;}
  inline string GetUserOutputPath(){ return m_UserOutputPath;}
  inline string GetBankOutputPath(){ return m_BankOutputPath;}
  inline string GetUserOutputName(){ return m_UserOutputName;}
  inline string GetBankOutputName(){ return m_BankOutputName;}
  inline int GetRunNumber(){return m_RunNumber;}
  inline int GetRunNumberMinor(){return m_RunNumberMinor;}

  inline unsigned int GetInitialBankLoad(){ return m_InitialBankLoad;}
  inline unsigned int GetMaximumBankLoad(){ return m_MaximumBankLoad;}
  inline bool GetNoBankTree() { return m_NoBankTree;}
  inline bool GetNoUserPoint(){ return m_NoUserPoint;} 
  
  inline void SetInputFileName(string InputFileName){ m_InputFileName = InputFileName;}
  inline void SetUserOutputPath(string UserOutputPath){ m_UserOutputPath = UserOutputPath;}
  inline void SetBankOutputPath(string BankOutputPath){ m_BankOutputPath = BankOutputPath;}
  inline void SetUserOutputName(string UserOutputName){ m_UserOutputName = UserOutputName;}
  inline void SetBankOutputName(string BankOutputName){ m_BankOutputName = BankOutputName;}
  inline void SetInitialBankLoad(unsigned int InitialBankLoad){ m_InitialBankLoad = InitialBankLoad;}
  inline void SetMaximumBankLoad(unsigned int MaximumBankLoad){ m_MaximumBankLoad = MaximumBankLoad;}
  inline void SetRunNumber(int RunNumber) {m_RunNumber = RunNumber;};
  inline void SetNoBankTree(bool NoBankTree) { m_NoBankTree = NoBankTree;}
  inline void SetNoUserPoint(bool  NoUserPoint){ m_NoUserPoint = NoUserPoint;}
};

#endif
