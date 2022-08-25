void func_get_contours(TH2D *h2_CL, TGraph *gh_CL[3], int index)
{
  gROOT->SetBatch( 1 );
  
  TString roostr = "";
  
  const int Ncontour = 3;
  double contours[Ncontour] = {0};
  contours[0] = 0.9;
  contours[1] = 0.95;
  contours[2] = 0.9973;
  
  ///////
  roostr = TString::Format("canv_h2_CL_%d", index);
  TCanvas *canv_h2_CL = new TCanvas(roostr, roostr, 800, 600);
  h2_CL->SetStats(0);
  h2_CL->SetContour(Ncontour, contours);
  h2_CL->Draw("cont z list");
  canv_h2_CL->Update(); // Needed to force the plotting and retrieve the contours in TGraphs
     
  // Get Contours
  TObjArray *conts_mc = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");
  TList* contLevel_mc = NULL;
  TGraph* gh_curv_mc[10] = {0};

  Int_t nGraphs_mc    = 0;
  Int_t TotalConts_mc = 0;

  if (conts_mc == NULL){
    printf("*** No Contours Were Extracted!\n");
    TotalConts_mc = 0;
    exit(1);
  } else {
    TotalConts_mc = conts_mc->GetSize();
  }

  //printf("TotalConts_mc = %d\n", TotalConts_mc);

  for(int i = 0; i < TotalConts_mc; i++){
    contLevel_mc = (TList*)conts_mc->At(i);
    //printf("Contour %d has %d Graphs\n", i, contLevel_mc->GetSize());
    nGraphs_mc += contLevel_mc->GetSize();
  }

  nGraphs_mc = 0;
  for(int i = 0; i < TotalConts_mc; i++){
    contLevel_mc = (TList*)conts_mc->At(i);

    // Get first graph from list on curves on this level
    gh_curv_mc[i] = (TGraph*)contLevel_mc->First();     
  }
  
  /////// convert the log-scale to nominal-scale
  for(int ic=0; ic<Ncontour; ic++) {
    int np_curv = gh_curv_mc[ic]->GetN();
    for(int idx=0; idx<np_curv; idx++) {
      double t14 = 0;
      double m41 = 0;
      gh_curv_mc[ic]->GetPoint(idx, t14, m41);
      
      double xx_value = pow(10., t14);
      double yy_value = pow(10., m41);

      gh_CL[ic]->SetPoint(gh_CL[ic]->GetN(), xx_value, yy_value);
      
    }    
  }

  delete canv_h2_CL;

  gROOT->SetBatch( 0 );
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////// MAIN /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void plot_contours()
{
  ////////////////////////////////////////// Draw style
  
  gStyle->SetOptStat(0);
  //gStyle->SetPalette(kBird);

  double snWidth = 2;

  // use medium bold lines and thick markers
  gStyle->SetLineWidth(snWidth);
  gStyle->SetFrameLineWidth(snWidth);
  gStyle->SetHistLineWidth(snWidth);
  gStyle->SetFuncWidth(snWidth);
  gStyle->SetGridWidth(snWidth);
  gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetEndErrorSize(4);  
  gStyle->SetEndErrorSize(0);
  
  ////////////////////////////////////////// input map of confidence level
  
  TString roostr = "";

  TFile *roofile = new TFile("h2_CL_map.root", "read");
  TH2D *h2d_space = (TH2D*)roofile->Get("h2d_space");

  //////// define three graphs for the exclusion curves

  const int NUM = 3;// according to "const int Ncontour = 3;" in func_get_contours
  TGraph *graph_CL[NUM];
  for(int idx=0; idx<NUM; idx++) {
    roostr = TString::Format("graph_CL_%d", idx);
    graph_CL[idx] = new TGraph();
    graph_CL[idx]->SetName(roostr);
  }

  ///////

  func_get_contours( h2d_space, graph_CL, 101 );

  ///////
  
  TCanvas *canv_CL = new TCanvas("canv_CL", "canv_CL", 900, 650);
  canv_CL->SetLogx(); canv_CL->SetLogy();
  graph_CL[1]->Draw("al");
  
}
