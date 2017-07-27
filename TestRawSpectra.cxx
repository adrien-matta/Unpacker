// cd Raw root data dir 
// upload file : TFile f("data41230_000.root")
// execute code: .x Test.cxx()

void Test(int entries=100000){

//sharc
  TCanvas* c1 = new TCanvas;
  c1->Divide(4,4);
  for(unsigned int i = 1 ; i < 13 ; i++){
   c1->cd(i); 
    dataTree->Draw(Form("fSharc_StripFront_Energy:fSharc_StripBack_Energy>>h1%i(10000,0,300000,1000,0,300000)",i),Form("fSharc_StripFront_DetectorNbr==%i && fSharc_StripFront_DetectorNbr==%i",i,i),"colz",entries,0) ;
  }
 
//Tigress
  TCanvas* c2 = new TCanvas;
  c2->Divide(2,3);
   c2->cd(1); 
    dataTree->Draw(Form("fTIG_Ge_CloverNbr>>h21(17,0,17)"),"","",entries,0) ;
   c2->cd(2); 
    dataTree->Draw(Form("fTIG_Ge_CrystalNbr>>h22(5,0,5)"),"","",entries,0) ;
   c2->cd(3); 
    dataTree->Draw(Form("fTIG_Ge_SegmentNbr>>h23(11,0,11)"),"","",entries,0) ;
   c2->cd(4); 
    dataTree->Draw(Form("fTIG_Ge_Energy/125>>h24(2000,0,10000)"),"","",entries,0) ;
   c2->cd(5); 
    dataTree->Draw(Form("fTIG_Ge_TimeCFD"),"","",entries,0) ;
   c2->cd(6); 
    dataTree->Draw(Form("fTIG_Ge_TimeLED"),"","",entries,0) ;

  //sample Tigress
  TCanvas* c3 = new TCanvas;
  c3->Divide(2,2);
  for(unsigned int i = 1 ; i < 5 ; i++){
   c3->cd(i); 
    dataTree->Draw(Form("fTIG_Ge_Energy/125>>h3%i(2000,0,10000)",i),Form("fTIG_Ge_CloverNbr==6 && fTIG_Ge_CrystalNbr==%i && fTIG_Ge_SegmentNbr==0 ",i),"",entries,0) ; 
  }

 }
