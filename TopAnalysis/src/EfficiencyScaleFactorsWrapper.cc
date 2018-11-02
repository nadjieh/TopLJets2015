#include "TopLJets2015/TopAnalysis/interface/EfficiencyScaleFactorsWrapper.h"
#include "TopLJets2015/TopAnalysis/interface/JSONWrapper.h"

#include "TFile.h"
#include "TSystem.h"

#include <iostream>


using namespace std;

//
EfficiencyScaleFactorsWrapper::EfficiencyScaleFactorsWrapper(bool isData,TString era)
{
  if(isData) return;
  init(era);
}

//
void EfficiencyScaleFactorsWrapper::init(TString era)
{
  if(era.Contains("era2017"))
    {
      era_=2017;
      cout << "[EfficiencyScaleFactorsWrapper]" << endl
           << "\tStarting efficiency scale factors for 2017" << endl
           << "\tWarnings: no trigger SFs for any object" << endl
           << "\t          missing tk eff for muons, reco eff for photons" << endl
           << "\tDon't forget to fix these and update these items!" << endl;

      //PHOTONS
      TString url(era+"/2017_PhotonsMVAwp80.root");
      gSystem->ExpandPathName(url);
      TFile *fIn=TFile::Open(url);
      scaleFactorsH_["g_id"]=(TH2 *)fIn->Get("EGamma_SF2D")->Clone();
      scaleFactorsH_["g_id"]->SetDirectory(0);
      fIn->Close();

      //MUONS
      /*
      url=era+"/muons/EfficienciesAndSF_RunBtoF_Nov17Nov2017.root";
      gSystem->ExpandPathName(url);
      fIn=TFile::Open(url);
      scaleFactorsH_["m_trig"]=(TH2 *)fIn->Get("IsoMu27_PtEtaBins/abseta_pt_ratio")->Clone();
      scaleFactorsH_["m_trig"]->SetDirectory(0);
      fIn->Close();

      url=era+"/muons/MuonTracking_EfficienciesAndSF_BCDEFGH.root";
      gSystem->ExpandPathName(url);
      fIn=TFile::Open(url);
      scaleFactorsGr_["m_tk"]=(TGraphAsymmErrors *)fIn->Get("ratio_eff_aeta_dr030e030_corr");
      fIn->Close();
      */

      url=era+"/RunBCDEF_SF_MuID.root";
      gSystem->ExpandPathName(url);
      fIn=TFile::Open(url);
      scaleFactorsH_["m_id"]=(TH2F *)fIn->Get("NUM_TightID_DEN_genTracks_pt_abseta")->Clone();
      scaleFactorsH_["m_id"]->SetDirectory(0);

      url=era+"/RunBCDEF_SF_MuISO.root";
      gSystem->ExpandPathName(url);
      fIn=TFile::Open(url);
      scaleFactorsH_["m_iso"]=(TH2F *)fIn->Get("NUM_TightRelIso_DEN_TightIDandIPCut_pt_abseta")->Clone();
      scaleFactorsH_["m_iso"]->SetDirectory(0);


        //ELECTRONS
      /*
        url=era+"/SingleElectron_TriggerSF_Run2016"+period+"_v3_prel.root";
        gSystem->ExpandPathName(url);
        fIn=TFile::Open(url)3 ;
        scaleFactorsH_["e_singleleptrig"+period]=(TH2 *)fIn->Get("SF")->Clone();
        scaleFactorsH_["e_singleleptrig"+period]->SetDirectory(0);
        fIn->Close();
      */
        
      url=era+"/egammaEffi.txt_EGM2D.root";
      gSystem->ExpandPathName(url);
      fIn=TFile::Open(url);
      scaleFactorsH_["e_rec"]=(TH2 *)fIn->Get("EGamma_SF2D")->Clone();
      scaleFactorsH_["e_rec"]->SetDirectory(0);     
      fIn->Close();
      
      url=era+"/2017_ElectronMVA80.root";
      gSystem->ExpandPathName(url);
      fIn=TFile::Open(url);      
      scaleFactorsH_["e_id"]=(TH2 *)fIn->Get("EGamma_SF2D")->Clone();
      scaleFactorsH_["e_id"]->SetDirectory(0);
      fIn->Close();
      
    }
}


//
EffCorrection_t EfficiencyScaleFactorsWrapper::getTriggerCorrection(std::vector<Particle> leptons, 
                                                                    std::vector<Particle> photons,
                                                                    std::vector<Particle> jets,
                                                                    TString period)
{
  EffCorrection_t corr(1.0,0.0);
  if(era_==2017)
    {
      if(leptons.size()==1)
        {
          TString hname(abs(leptons[0].id())==11 ? "e_trig" : "m_trig");          
          if( abs(leptons[0].id())==13 and scaleFactorsH_.find(hname)!=scaleFactorsH_.end() )
            {
              TH1 *h=scaleFactorsH_[hname];
              float minEtaForEff( h->GetXaxis()->GetXmin() ), maxEtaForEff( h->GetXaxis()->GetXmax()-0.01 );
              float etaForEff=TMath::Max(TMath::Min(float(fabs(leptons[0].eta())),maxEtaForEff),minEtaForEff);
              Int_t etaBinForEff=h->GetXaxis()->FindBin(etaForEff);

              float minPtForEff( h->GetYaxis()->GetXmin() ), maxPtForEff( h->GetYaxis()->GetXmax()-0.01 );
              float ptForEff=TMath::Max(TMath::Min(float(leptons[0].pt()),maxPtForEff),minPtForEff);
              Int_t ptBinForEff=h->GetYaxis()->FindBin(ptForEff);

              corr.first=h->GetBinContent(etaBinForEff,ptBinForEff);
              corr.second=h->GetBinError(etaBinForEff,ptBinForEff);
            }
          //electron histogram uses eta, not abs(eta)
          else if( abs(leptons[0].id())==11 and scaleFactorsH_.find(hname)!=scaleFactorsH_.end() )
            {
              TH1 *h=scaleFactorsH_[hname];
              float minEtaForEff( h->GetXaxis()->GetXmin() ), maxEtaForEff( h->GetXaxis()->GetXmax()-0.01 );
              float etaForEff=TMath::Max(TMath::Min(float(leptons[0].eta()),maxEtaForEff),minEtaForEff);
              Int_t etaBinForEff=h->GetXaxis()->FindBin(etaForEff);

              float minPtForEff( h->GetYaxis()->GetXmin() ), maxPtForEff( h->GetYaxis()->GetXmax()-0.01 );
              float ptForEff=TMath::Max(TMath::Min(float(leptons[0].pt()),maxPtForEff),minPtForEff);
              Int_t ptBinForEff=h->GetYaxis()->FindBin(ptForEff);

              corr.first=h->GetBinContent(etaBinForEff,ptBinForEff);
              corr.second=h->GetBinError(etaBinForEff,ptBinForEff);
            }
        }
    }

  return corr;
}

//
EffCorrection_t EfficiencyScaleFactorsWrapper::getOfflineCorrection(Particle p,TString period)
{
  return getOfflineCorrection(p.id(),p.pt(),p.eta(),period);
}

//
EffCorrection_t EfficiencyScaleFactorsWrapper::getOfflineCorrection(int pdgId,float pt,float eta,TString period)
{
  EffCorrection_t corr(1.0,0.0);

  //update correction from histo, if found
  TString idstr("e");
  if(abs(pdgId)==13) idstr="m";
  if(abs(pdgId)==22) idstr="g";
  TString hname(idstr+"_id"+period);
  if( scaleFactorsH_.find(hname)!=scaleFactorsH_.end() )
    {      
      //identification
      TH2 *h=scaleFactorsH_[hname];
      float minEtaForEff( h->GetXaxis()->GetXmin() ), maxEtaForEff( h->GetXaxis()->GetXmax()-0.01 );
      float etaForEff;
      if (minEtaForEff >= 0.) //axis is abseta
        etaForEff=TMath::Max(TMath::Min(float(fabs(eta)),maxEtaForEff),minEtaForEff);
      else //axis is signed eta
        etaForEff=TMath::Max(TMath::Min(float(eta),maxEtaForEff),minEtaForEff);
      Int_t etaBinForEff=h->GetXaxis()->FindBin(etaForEff);
      float minPtForEff( h->GetYaxis()->GetXmin() ), maxPtForEff( h->GetYaxis()->GetXmax()-0.01 );
      float ptForEff=TMath::Max(TMath::Min(pt,maxPtForEff),minPtForEff);
      Int_t ptBinForEff=h->GetYaxis()->FindBin(ptForEff);
      corr.first=h->GetBinContent(etaBinForEff,ptBinForEff);
      corr.second=h->GetBinError(etaBinForEff,ptBinForEff);

      //isolation (if available separately)
      hname=idstr+"_iso";
      if(scaleFactorsH_.find(hname)!=scaleFactorsH_.end() )
	{
	  TH2 *h=scaleFactorsH_[hname];
          float minEtaForEff( scaleFactorsH_[hname]->GetXaxis()->GetXmin() ), maxEtaForEff( scaleFactorsH_[hname]->GetXaxis()->GetXmax()-0.01 );
          float etaForEff( minEtaForEff >= 0. ?
                           TMath::Max(TMath::Min(float(fabs(eta)),maxEtaForEff),minEtaForEff) :
                           TMath::Max(TMath::Min(float(eta),maxEtaForEff),minEtaForEff) );
          Int_t etaBinForEff=scaleFactorsH_[hname]->GetXaxis()->FindBin(etaForEff);
	  
	  float minPtForEff( h->GetYaxis()->GetXmin() ), maxPtForEff( h->GetYaxis()->GetXmax()-0.01 );
	  float ptForEff=TMath::Max(TMath::Min(pt,maxPtForEff),minPtForEff);
	  Int_t ptBinForEff=h->GetYaxis()->FindBin(ptForEff);
	  
	  corr.second = sqrt(pow(h->GetBinError(etaBinForEff,ptBinForEff)*corr.first,2)+pow(h->GetBinContent(etaBinForEff,ptBinForEff)*corr.second,2));
	  corr.first  = corr.first*h->GetBinContent(etaBinForEff,ptBinForEff);
	}

      //reco efficiency (if available)
      hname=idstr+"_rec"+period;
      if(scaleFactorsH_.find(hname)!=scaleFactorsH_.end())
        {
	  float minEtaForEff( scaleFactorsH_[hname]->GetXaxis()->GetXmin() ), maxEtaForEff( scaleFactorsH_[hname]->GetXaxis()->GetXmax()-0.01 );
	  float etaForEff( minEtaForEff >= 0. ?
			   TMath::Max(TMath::Min(float(fabs(eta)),maxEtaForEff),minEtaForEff) :
			   TMath::Max(TMath::Min(float(eta),maxEtaForEff),minEtaForEff) );
	  Int_t etaBinForEff=scaleFactorsH_[hname]->GetXaxis()->FindBin(etaForEff);

	  float minPtForEff( scaleFactorsH_[hname]->GetYaxis()->GetXmin() ), maxPtForEff( scaleFactorsH_[hname]->GetYaxis()->GetXmax()-0.01 );
	  float ptForEff=TMath::Max(TMath::Min(pt,maxPtForEff),minPtForEff);
	  Int_t ptBinForEff=scaleFactorsH_[hname]->GetYaxis()->FindBin(ptForEff);
	  
	  float recEff=scaleFactorsH_[hname]->GetBinContent(etaBinForEff,ptBinForEff);
	  float recEffUnc=scaleFactorsH_[hname]->GetBinError(etaBinForEff,ptBinForEff);

	  corr.second=sqrt(pow(recEff*corr.second,2)+pow(recEffUnc*corr.first,2));
	  corr.first*=recEff;
	}

      //tracking efficiency (if available)
      hname=idstr+"_tk"+period;
      if(scaleFactorsGr_.find(hname)!=scaleFactorsGr_.end())
        {
          Double_t x(0.),xdiff(9999.),y(0.);
          float tkEffSF(1.0),tkEffSFUnc(0);
          for(Int_t ip=0; ip<scaleFactorsGr_[hname]->GetN(); ip++)
            {
              scaleFactorsGr_[hname]->GetPoint(ip,x,y);
              float ixdiff(TMath::Abs(fabs(eta)-x));
              if(ixdiff>xdiff) continue;
              xdiff=ixdiff;
              tkEffSF=y;
              tkEffSFUnc=scaleFactorsGr_[hname]->GetErrorY(ip);
            }
          corr.second = sqrt(pow(tkEffSFUnc*corr.first,2)+pow(tkEffSF*corr.second,2));
          corr.first  = corr.first*tkEffSF;
        }
    }
  
  //
  return corr;
}

//
EfficiencyScaleFactorsWrapper::~EfficiencyScaleFactorsWrapper()
{
}
