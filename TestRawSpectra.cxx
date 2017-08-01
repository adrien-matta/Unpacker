// cd Raw root data dir 
// upload file : TFile f("data41230_000.root")
// execute code: .x TestRawSpectra.cxx()

void TestRawSpectra(int entries=100000){

//sharc
  TCanvas* c0 = new TCanvas;
  c0->Divide(4,4);
  int det = 1; 
  for(unsigned int i = 1 ; i < 17 ; i++){
   c0->cd(i); 
   //rawdata->Draw(Form("fSharc_StripFront_Energy:fSharc_StripBack_Energy>>h0%i(10000,0,10000,1000,0,10000)",i),Form("fSharc_StripFront_DetectorNbr==%i && fSharc_StripFront_DetectorNbr==%i",i,i),"colz",entries,0) ;
  rawdata->Draw(Form("fSharc_StripBack_Energy>>h0%i%i(1000,0,10000)",det,i),Form("fSharc_StripFront_StripNbr==%i & fSharc_StripFront_DetectorNbr==%i",i, det),"colz",entries,0) ;
  }
 
 //sharc
  TCanvas* c1 = new TCanvas;
  c1->Divide(2,2);
  for(unsigned int i = 1 ; i < 5 ; i++){
   c1->cd(i); 
  rawdata->Draw(Form("fSharc_StripBack_Energy>>h1%i(1000,0,10000)",i),Form("fSharc_StripFront_DetectorNbr==%i",i),"colz",entries,0) ;
  }
 
  //sharc
  TCanvas* c4 = new TCanvas;
  c4->Divide(2,2);
   c4->cd(1); 
    rawdata->Draw(Form("fSharc_StripFront_DetectorNbr>>h41(10,0,10)"),"","",entries,0) ;
   c4->cd(2); 
    rawdata->Draw(Form("fSharc_StripBack_DetectorNbr>>h42(10,0,10)"),"","",entries,0) ;
   c4->cd(3); 
    rawdata->Draw(Form("fSharc_StripFront_StripNbr>>h43(20,0,20)"),"","",entries,0) ;
   c4->cd(4); 
    rawdata->Draw(Form("fSharc_StripBack_StripNbr>>h44(20,0,20)"),"","",entries,0) ;
 

//Tigress
  TCanvas* c2 = new TCanvas;
  c2->Divide(2,3);
   c2->cd(1); 
    rawdata->Draw(Form("fTIG_Ge_CloverNbr>>h21(17,0,17)"),"","",entries,0) ;
   c2->cd(2); 
    rawdata->Draw(Form("fTIG_Ge_CrystalNbr>>h22(5,0,5)"),"","",entries,0) ;
   c2->cd(3); 
    rawdata->Draw(Form("fTIG_Ge_SegmentNbr>>h23(11,0,11)"),"","",entries,0) ;
   c2->cd(4); 
    rawdata->Draw(Form("fTIG_Ge_Energy>>h24(2000,0,10000)"),"","",entries,0) ;
   c2->cd(5); 
    rawdata->Draw(Form("fTIG_Ge_TimeCFD"),"","",entries,0) ;
   c2->cd(6); 
    rawdata->Draw(Form("fTIG_Ge_TimeLED"),"","",entries,0) ;

  //sample Tigress
  TCanvas* c3 = new TCanvas;
  c3->Divide(2,2);
  for(unsigned int i = 1 ; i < 5 ; i++){
   c3->cd(i); 
    rawdata->Draw(Form("fTIG_Ge_Energy>>h3%i(2000,0,10000)",i),Form("fTIG_Ge_CloverNbr==6 && fTIG_Ge_CrystalNbr==%i && fTIG_Ge_SegmentNbr==0 ",i),"",entries,0) ; 
  }

 }
