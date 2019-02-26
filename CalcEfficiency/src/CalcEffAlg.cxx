// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include <iostream>
#include <fstream>
#include <vector>

// xAOD 
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "GoodRunsLists/IGoodRunsListSelectorTool.h"
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"

// My class
#include "CalcEfficiency/CalcEffAlg.h"
#include "CalcEfficiency/TagAndProbe.h"
#include "CalcEfficiency/MuonExtUtils.h"
#include "CalcEfficiency/EventTree.h"

using namespace std;

///THis function is constructor.
CalcEffAlg::CalcEffAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
  m_configSvc( "TrigConf::TrigConfigSvc/TrigConfigSvc", name ),
  m_dsSvc( "TrigConf::DSConfigSvc/DSConfigSvc", name ),
  m_trigDecTool( "Trig::TrigDecisionTool/TrigDecisionTool" ),
  m_grlTool( "GoodRunsListSelectionTool/MyGRLTool" ),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_vrtfitter("Trk::TrkVKalVrtFitter")
{
  ///(https://twiki.cern.ch/twiki/bin/view/Sandbox/WritingYourOwnAthenaAlgorithms)Notice the calls of the declareProperty("electron_Et_min_cut", m_electron_Et_min_cut = 20*GeV) in the constructor. This makes the C++ m_electron_Et_min_cut variable configurable from the Python job options. The first argument is a string containing the Python name of the variable. The second is the C++ variable with an optional equals-sign followed by a default value. Configuration from Python will be explained more later when we get to the job options.
  declareProperty( "message", m_message=2);
  declareProperty( "TrigDecToolName", m_trigDecTool );
  declareProperty( "OutputFile", m_etname );
  declareProperty( "TapMethod", m_tapmethod );
  declareProperty( "Extrapolate", m_useExt );
  declareProperty( "GRL", m_useGRL );
  declareProperty( "DataType", m_dataType );
}

StatusCode CalcEffAlg::initialize() {
  ATH_MSG_INFO("initialize()");
  ATH_MSG_INFO("My message: " << m_message);
  m_isFirstEvent = true; 

  //==============================================================
  //==  GRL Tools
  //==============================================================
  ATH_CHECK( m_grlTool.retrieve() );



  //==============================================================
  //==  Trigger Tools
  //==============================================================
  ATH_CHECK(m_configSvc.retrieve());
  ATH_CHECK(m_dsSvc.retrieve());
  ATH_CHECK(m_trigDecTool.retrieve());



  //==============================================================
  //==  Tools
  //==============================================================
  ATH_CHECK(m_extrapolator.retrieve());
  ATH_CHECK(m_vrtfitter.retrieve());



  //==============================================================
  //==  MuonExtrapolatorUtils Class
  //==============================================================
  m_ext.initialize( m_extrapolator );



  //==============================================================
  //==  VrtFitterUtils Class
  //==============================================================
  m_vft.initialize( m_vrtfitter );



  //==============================================================
  //==  EventTree Class
  //==============================================================
  m_et.initialize( m_etname );



  //==============================================================
  //==  TagAndProbe Class
  //
  ///Check TagAndProbe.h about declare of addMesChain.
  ///int addMesChain( const string& name, const L1Item& mesL1, const string& mesHLT, const string& mesAddTag); 
  ///int initialize( const int& message,const bool& useExt,const std::string method,MuonExtUtils ext,VrtFitUtils vft,ToolHandle<Trig::TrigDecisionTool> tdt,const std::string dataType ); //!
  //==============================================================
  m_tap.initialize( 0, m_useExt, m_tapmethod, m_ext, m_vft, m_trigDecTool, m_dataType );
  m_tap.addMesChain( "mu4", TagAndProbe::L1_MU4, "HLT_mu4", "none" );
  m_tap.addMesChain( "mu6", TagAndProbe::L1_MU6, "HLT_mu6", "none" );
  m_tap.addMesChain( "mu6ms", TagAndProbe::L1_MU6, "HLT_mu6_msonly", "none" );
  m_tap.addMesChain( "mu10", TagAndProbe::L1_MU10, "HLT_mu10", "none" );
  m_tap.addMesChain( "mu10nc", TagAndProbe::L1_MU10, "HLT_mu10_nomucomb", "none" );
  m_tap.addMesChain( "mu11", TagAndProbe::L1_MU11, "HLT_mu11", "none" );
  m_tap.addMesChain( "mu11nc", TagAndProbe::L1_MU11, "HLT_mu11_nomucomb", "none" );
  m_tap.addMesChain( "mu14", TagAndProbe::L1_MU10, "HLT_mu14", "none" );
  m_tap.addMesChain( "mu18", TagAndProbe::L1_MU20, "HLT_mu18", "none" );
  m_tap.addMesChain( "mu20", TagAndProbe::L1_MU20, "HLT_mu20", "none" );
  m_tap.addMesChain( "mu20il", TagAndProbe::L1_MU20, "HLT_mu20_iloose_L1MU15", "none" );
  m_tap.addMesChain( "mu20ms", TagAndProbe::L1_MU20, "HLT_mu20_msonly", "none" );
  //m_tap.addMesChain( "mu24il", TagAndProbe::L1_MU20, "HLT_mu24_iloose_L1MU15", "none" );
  //m_tap.addMesChain( "mu24im", TagAndProbe::L1_MU20, "HLT_mu24_imedium", "none" );
  m_tap.addMesChain( "mu26im", TagAndProbe::L1_MU20, "HLT_mu26_imedium", "none" );
  m_tap.addMesChain( "mu26ivm", TagAndProbe::L1_MU20, "HLT_mu26_ivarmedium", "none" );
  m_tap.addMesChain( "mu50", TagAndProbe::L1_MU20, "HLT_mu50", "none" );
  m_tap.addMesChain( "mu60", TagAndProbe::L1_MU20, "HLT_mu60_0eta105_msonly", "none" );
  //m_tap.addMesChain( "mu4FS", TagAndProbe::NOTHING, "HLT_mu20_mu8noL1", "HLT_mu20_mu8noL1" );
  m_tap.addMesChain( "mu4FS", TagAndProbe::NOTHING, "HLT_mu4noL1", "none" );
  m_tap.addMesChain( "mu4FSMU6", TagAndProbe::L1_MU6, "HLT_mu20_mu8noL1", "HLT_mu20_mu8noL1" );
  //m_tap.addMesChain( "mu6FS", TagAndProbe::NOTHING, "HLT_mu20_mu8noL1", "HLT_mu20_mu8noL1" );
  m_tap.addMesChain( "mu6FS", TagAndProbe::NOTHING, "HLT_mu4noL1", "none" );
  m_tap.addMesChain( "mu6FSMU6", TagAndProbe::L1_MU6, "HLT_mu20_mu8noL1", "HLT_mu20_mu8noL1" );
  //m_tap.addMesChain( "mu8FS", TagAndProbe::NOTHING, "HLT_mu20_mu8noL1", "HLT_mu20_mu8noL1" );
  m_tap.addMesChain( "mu8FS", TagAndProbe::NOTHING, "HLT_mu4noL1", "none" );
  //m_tap.addMesChain( "2mu4", TagAndProbe::L1_MU4, "HLT_2mu4", "HLT_mu4" );
  m_tap.addMesChain( "2mu6", TagAndProbe::L1_MU6, "HLT_2mu6", "HLT_mu6" );
  m_tap.addMesChain( "2mu10", TagAndProbe::L1_MU10, "HLT_2mu10", "HLT_mu10" );
  m_tap.addMesChain( "2mu10nocb", TagAndProbe::L1_MU10, "HLT_2mu10_nomucomb", "HLT_mu10" );
  m_tap.addMesChain( "mu11mu6", TagAndProbe::NOTHING, "HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6_bTau", "HLT_mu11");

  // Trigger rate hists
  StatusCode sc = service("THistSvc", m_thistSvc);
  if(sc.isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve THistSvc");
    return sc;
  }

  m_trigL1.push_back(TagAndProbe::L1_MU4);
  m_trigEvent.push_back("HLT_noalg_L1MU4");
  m_trigHLT.push_back("HLT_mu4");
  m_trigThreshold.push_back( 4.0 );

  m_trigL1.push_back(TagAndProbe::L1_MU6);
  m_trigEvent.push_back("HLT_noalg_L1MU6");
  m_trigHLT.push_back("HLT_mu6");
  m_trigThreshold.push_back( 6.0 );

  m_trigL1.push_back(TagAndProbe::L1_MU20);
  m_trigEvent.push_back("HLT_noalg_L1MU20");
  m_trigHLT.push_back("HLT_mu26_ivarmedium");
  m_trigThreshold.push_back( 26.0 );

  m_trigL1.push_back(TagAndProbe::L1_MU20);
  m_trigEvent.push_back("HLT_noalg_L1MU20");
  m_trigHLT.push_back("HLT_mu50");
  m_trigThreshold.push_back( 50.0 );

  m_trigL1.push_back(TagAndProbe::L1_MU20);
  m_trigEvent.push_back("HLT_mu50");
  m_trigHLT.push_back("HLT_mu50");
  m_trigThreshold.push_back( 50.0 );

  m_trigL1.push_back(TagAndProbe::L1_MU4);
  m_trigEvent.push_back("HLT_mu4");
  m_trigHLT.push_back("HLT_mu4");
  m_trigThreshold.push_back( 4.0 );

  //m_trigL1.push_back(TagAndProbe::L1_MU4);
  //m_trigEvent.push_back("HLT_noalg_L1MU4");
  //m_trigHLT.push_back("HLT_noalg_L1MU4");
  //m_trigThreshold.push_back( 4.0 );

  const int NChain = m_trigEvent.size();

  for ( int i = 0; i< NChain; i++){
    m_h_countEvent.push_back(new TH1D(Form("countEvent_%s_%s",       m_trigEvent[i].data(), m_trigHLT[i].data()), "countEvent;Run number;Counts",    100, -3000000000,  3000000000));
    m_h_countL1.push_back(new TH1F(Form("countL1_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countL1;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countSA.push_back(new TH1F(Form("countSA_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countSA;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countSA4GeV.push_back(new TH1F(Form("countSA4_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countSA;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countSA8GeV.push_back(new TH1F(Form("countSA8_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countSA;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countSA12GeV.push_back(new TH1F(Form("countSA12_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countSA;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countSA16GeV.push_back(new TH1F(Form("countSA16_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countSA;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countSA20GeV.push_back(new TH1F(Form("countSA20_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countSA;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countSA24GeV.push_back(new TH1F(Form("countSA24_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countSA;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countCB.push_back(new TH1F(Form("countCB_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countCB;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countEF.push_back(new TH1F(Form("countEF_%s_%s",             m_trigEvent[i].data(), m_trigHLT[i].data()), "countEF;#eta_{RoI};Counts",       100, -3, 3));
    m_h_countOff.push_back(new TH1F(Form("countOff_%s_%s",           m_trigEvent[i].data(), m_trigHLT[i].data()), "countOff;#eta_{RoI};Counts",      100, -3, 3));
    m_h_countOffPtCut.push_back(new TH1F(Form("countOffPtCut_%s_%s", m_trigEvent[i].data(), m_trigHLT[i].data()), "countOffPtCut;#eta_{RoI};Counts", 100, -3, 3));
  }

  return StatusCode::SUCCESS;
}

StatusCode CalcEffAlg::finalize() {
  ATH_MSG_INFO("finalize()");
  m_et.finalize();
  return StatusCode::SUCCESS;
}

StatusCode CalcEffAlg::execute() {
  ATH_MSG_INFO("execute()");

  ///main function to do "Tag and Probe" .

  //==============================================================
  //=  Event information
  //==============================================================
  const xAOD::EventInfo* eventInfo = 0;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if(StatusCode::SUCCESS!=sc || !eventInfo) {
    ATH_MSG_WARNING("Could not retrieve EventInfo");
    return StatusCode::SUCCESS;
  }

  uint32_t runNumber = eventInfo->runNumber();
  unsigned long long eventNumber = eventInfo->eventNumber();
  int lumiBlock = eventInfo-> lumiBlock();
  float averageInteractionsPerCrossing =  eventInfo -> averageInteractionsPerCrossing();
  ATH_MSG_INFO("Run = " << runNumber << " : Event = " << eventNumber);
  bool isMC = true;
  if(!eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION ) ){
    isMC = false;
  }

  // GRL
  if( !isMC && m_useGRL ){
    ATH_MSG_INFO("Skip this event via GRL");
    if(!m_grlTool->passRunLB(*eventInfo)) return StatusCode::SUCCESS; //checks the GRL and skips to next event if not passing
  } // end if not MC

  //==============================================================
  //==  Trigger Tools
  //==============================================================
  vector< string > passlist;
  bool passany = false;
  auto cgs = m_trigDecTool->getChainGroup( "HLT.*mu.*|L1_.*MU.*|HLT_noalg_L1.*MU.*" );
  //auto cgs = m_trigDecTool->getChainGroup( "HLT.*" );
  for ( auto &trig : cgs->getListOfTriggers() ) {
    auto cg = m_trigDecTool->getChainGroup( trig );
    bool isPassedCurrent = cg->isPassed();
    if( m_isFirstEvent ) cout << trig << " / " << isPassedCurrent << endl;
    //cout << trig << " / " << isPassedCurrent << endl;
    if ( isPassedCurrent ) {
      passlist.push_back( trig );
      //cout << "TRIGGER: " << trig << endl;
      passany = true;
    }
  }
  //if( !passany )return StatusCode::SUCCESS;
  m_tap.eventTriggerDecision( passlist );
  // retrieve muons
  const xAOD::MuonContainer* mus = 0;
  ATH_CHECK( evtStore()->retrieve( mus, "Muons" ) );
  // retrieve muon rois
  const xAOD::MuonRoIContainer* rois = 0;
  ATH_CHECK( evtStore()->retrieve( rois, "LVL1MuonRoIs" ) );

  FillRateHist(mus, rois).setChecked();
  //StatusCode scFillRateHist = FillRateHist(rois);
  //if(StatusCode::SUCCESS!=scFillRateHist) {
  //  ATH_MSG_WARNING("Could not FillRateHist(rois)");
  //}

  // retrieve muon rois
  const xAOD::TrackParticleContainer* ftfs = 0;
  //ATH_CHECK( evtStore()->retrieve( ftfs, "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF" ) );
  // do tag and probe
  cout << "eventNumber==========" << eventNumber << "========" << endl;
  m_tap.clear();
  cout << "end m_tap.clear()" << endl;
  m_tap.setProbes( mus, rois );
  cout << "end m_tap.setProbes()" << endl;
  m_tap.doProbeMatching( rois, ftfs );
  cout << "end m_tap.doProbeMatching()" << endl;
  //cout << m_tap.tagPhi()[0] << endl;
  // fill results
  m_et.filltree( m_tap, eventNumber, runNumber, lumiBlock, averageInteractionsPerCrossing );
  cout << "end m_et.filltree()" << endl;

  m_isFirstEvent = false;
  return StatusCode::SUCCESS;
}



StatusCode CalcEffAlg::FillRateHist(const xAOD::MuonContainer* muons, const xAOD::MuonRoIContainer* rois ) {

  ATH_MSG_INFO("");
  ATH_MSG_INFO("For Loop in FillRateHist()");

  const xAOD::EventInfo* eventInfo = 0;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if(StatusCode::SUCCESS!=sc || !eventInfo) {
    ATH_MSG_WARNING("Could not retrieve EventInfo in FillRateHist()");
    return StatusCode::SUCCESS;
  }

  uint32_t runNumber = eventInfo->runNumber();
  unsigned long long eventNumber = eventInfo->eventNumber();

  for( unsigned int iChain = 0; iChain < m_trigL1.size(); ++iChain){
    // Set HLT TE names
    std::string trigSAName = "";
    getSATEName( m_trigHLT[iChain], trigSAName );
    std::string trigCBName = "";
    getCBTEName( m_trigHLT[iChain], trigCBName );

    ATH_MSG_INFO("");
    ATH_MSG_INFO("[iChain = " << iChain
        << ", trigEvent = " << m_trigEvent[iChain]
        << ", trigHLT = " << m_trigHLT[iChain]
        << ", trigL1 = " << m_trigL1[iChain]
        << ", trigSAName = " << trigSAName
        << ", trigCBName = " << trigCBName
        << ", trigThreshold = " << m_trigThreshold[iChain]
        << "]");

    bool EventTriggerPassed = m_trigDecTool->isPassed(m_trigEvent[iChain],TrigDefs::Physics);
    //ATH_MSG_INFO("PreScale: " << m_trigDecTool -> getPrescale(m_trigEvent[iChain]));
    //ATH_MSG_INFO("eventAccepted: " << m_trigDecTool->isPassed(m_trigEvent[iChain],TrigDefs::eventAccepted));
    //ATH_MSG_INFO("passedThrough: " << m_trigDecTool->isPassed(m_trigEvent[iChain],TrigDefs::passedThrough));
    //ATH_MSG_INFO("Physics | passedThrough: " << m_trigDecTool->isPassed(m_trigEvent[iChain],TrigDefs::Physics | TrigDefs::passedThrough));
    //ATH_MSG_INFO("requireDecision: " << m_trigDecTool->isPassed(m_trigEvent[iChain],TrigDefs::requireDecision));
    //ATH_MSG_INFO("allowResurrectedDecision: " << m_trigDecTool->isPassed(m_trigEvent[iChain],TrigDefs::allowResurrectedDecision));
    //ATH_MSG_INFO("enforceLogicalFlow: " << m_trigDecTool->isPassed(m_trigEvent[iChain],TrigDefs::enforceLogicalFlow));
    //ATH_MSG_INFO("allowResurrectedDecision | requireDecision | enforceLogicalFlow: " << m_trigDecTool->isPassed(m_trigEvent[iChain],TrigDefs::allowResurrectedDecision | TrigDefs::requireDecision | TrigDefs::enforceLogicalFlow));
    if (!EventTriggerPassed){
      ATH_MSG_INFO("Event Trigger is not passed: " << m_trigEvent[iChain]);
      //ATH_MSG_INFO("Check eventNumber: " << - eventNumber);
      //m_h_countEvent[iChain] -> Fill(- eventNumber);
      continue;
    } else {
      ATH_MSG_INFO("Event Trigger is passed: " << m_trigEvent[iChain]);
      //ATH_MSG_INFO("Check eventNumber: " << + eventNumber);
      m_h_countEvent[iChain] -> Fill(+ eventNumber);
    }


    //L1
    xAOD::MuonRoIContainer::const_iterator roi_itr = rois->begin();
    xAOD::MuonRoIContainer::const_iterator roi_end = rois->end();
    for(; roi_itr!=roi_end; ++roi_itr){
      int roiThr       = (*roi_itr)->getThrNumber();
      double roiEta    = (*roi_itr)->eta();
      const int roiNum = (*roi_itr)->getRoI();
      const int roiPt  = (*roi_itr)->thrValue();
      ATH_MSG_INFO("L1: roiNum = " << roiNum
          << ", roiThr = " << roiThr
          << ", roiPt = " << roiPt
          << ", roiEta = " << roiEta);
      // Check L1 pass
      if(roiThr >= m_trigL1[iChain]){
        m_h_countL1[iChain]->Fill(roiEta);
        ATH_MSG_INFO("Passed L1 in FillRateHist()");
      }
    }

    //SA
    Trig::FeatureContainer fc = m_trigDecTool->features( m_trigHLT[iChain], TrigDefs::alsoDeactivateTEs );
    const std::vector< Trig::Feature<xAOD::L2StandAloneMuonContainer> > fL2SAs = fc.get<xAOD::L2StandAloneMuonContainer>( "", TrigDefs::alsoDeactivateTEs );
    ATH_MSG_INFO("fL2SAs size = " << fL2SAs.size());
    Trig::ExpertMethods* expert = m_trigDecTool -> ExperimentalAndExpertMethods();
    expert->enable();
    for(auto fL2SA : fL2SAs){
      const HLT::TriggerElement *trigElementSA = fL2SA.te();
      std::vector<HLT::TriggerElement*> TEsuccessors = expert->getNavigation()->getDirectSuccessors(trigElementSA);
      Bool_t isActiveTE = kFALSE;
      for(auto te : TEsuccessors){
        if ( te -> getActiveState() ){
          TString teName = Trig::getTEName( *te );
          if ( (teName.Contains( "L2_mu_SAhyp") && teName.Contains( trigSAName.c_str() ) )|| ( teName.Contains("L2_mu_hypo1" ) ) )
            isActiveTE = kTRUE;
        }
        ATH_MSG_INFO("TEName = " << Trig::getTEName( *te ) << ", getActiveState = " << te -> getActiveState() << ", isActiveTE = " << isActiveTE);
      }
      const xAOD::L2StandAloneMuonContainer* cont = fL2SA.cptr();
      ATH_MSG_INFO("L2SA cont size = " << cont->size());
      for( const auto& l2sa : *cont ) {
        int l2saRoINum  = l2sa->roiNumber();
        double l2saPt  = l2sa->pt();
        double l2saRoIEta  = l2sa->roiEta();
        ATH_MSG_INFO("L2SA: roiNum = " << l2saRoINum << ", Pt = " << l2saPt <<  ", roiEta = " << l2saRoIEta);
        // tmp fill for check
        // pass and not pass
        m_h_countCB[iChain]->Fill(l2saRoIEta);
        // only pass L2MuonSA
        if(isActiveTE) m_h_countSA[iChain]->Fill(l2saRoIEta);
        if(isActiveTE == 1 && l2saPt > 4.0) m_h_countSA4GeV[iChain]->Fill(l2saRoIEta);
        if(isActiveTE == 1 && l2saPt > 8.0) m_h_countSA8GeV[iChain]->Fill(l2saRoIEta);
        if(isActiveTE == 1 && l2saPt > 12.0) m_h_countSA12GeV[iChain]->Fill(l2saRoIEta);
        if(isActiveTE == 1 && l2saPt > 16.0) m_h_countSA16GeV[iChain]->Fill(l2saRoIEta);
        if(isActiveTE == 1 && l2saPt > 20.0) m_h_countSA20GeV[iChain]->Fill(l2saRoIEta);
        if(isActiveTE == 1 && l2saPt > 24.0) m_h_countSA24GeV[iChain]->Fill(l2saRoIEta);
      }
    }


    // CB
    Trig::FeatureContainer fcL2CB = m_trigDecTool->features( m_trigHLT[iChain], TrigDefs::alsoDeactivateTEs );
    const std::vector< Trig::Feature<xAOD::L2CombinedMuonContainer> > fL2CBs = fcL2CB.get<xAOD::L2CombinedMuonContainer>( "", TrigDefs::alsoDeactivateTEs );
    ATH_MSG_INFO("fL2CBs size = " << fL2CBs.size());
    Trig::ExpertMethods* expertCB = m_trigDecTool -> ExperimentalAndExpertMethods();
    expertCB->enable();
    for(auto fL2CB : fL2CBs){
      const HLT::TriggerElement *trigElementCB = fL2CB.te();
      std::vector<HLT::TriggerElement*> TEsuccessors = expertCB->getNavigation()->getDirectSuccessors(trigElementCB);
      Bool_t isActiveTE = kFALSE;
      for(auto te : TEsuccessors){
        if ( te -> getActiveState() ){
          TString teName = Trig::getTEName( *te );
          if ( ( teName.Contains( "L2_mucombhyp") && teName.Contains( trigCBName.c_str() ) ) || teName.Contains( "L2_mu_hypo2" ) )
            isActiveTE = kTRUE;
        }
        ATH_MSG_INFO("TEName = " << Trig::getTEName( *te ) << ", getActiveState = " << te -> getActiveState() << ", isActiveTE = " << isActiveTE);
      }
      const xAOD::L2CombinedMuonContainer* cont = fL2CB.cptr();
      ATH_MSG_INFO("L2CB cont size = " << cont->size());
      for( const auto& l2cb : *cont ) {
        const double l2cbPt  = l2cb->pt();
        const double l2cbEta  = l2cb->eta();
        const double l2cbPhi  = l2cb->phi();
        //const double l2cbdR = m_utils.deltaR( muEta, muPhi, l2cbEta, l2cbPhi);
        if (!l2cb->muSATrack()) {
          ATH_MSG_INFO("[No muSATrack] l2cbPt = " << l2cbPt <<  ", l2cbEta = " << l2cbEta << ", l2cbPhi = " << l2cbPhi);
        } else {
          ATH_MSG_INFO("[muSATrack] l2cbPt = " << l2cbPt <<  ", l2cbEta = " << l2cbEta << ", l2cbPhi = " << l2cbPhi);
          ATH_MSG_INFO("[muSATrack] l2saPt = " << l2cb->muSATrack()->pt() <<  ", l2saEta = " << l2cb->muSATrack()->eta() << ", l2saPhi = " << l2cb->muSATrack()->phi());
        }
        //ATH_MSG_INFO("L2CB: roiNum = " << l2saRoINum << ", Pt = " << l2saPt <<  ", roiEta = " << l2saRoIEta);
        //if(isActiveTE) m_h_countCB[iChain]->Fill(l2CBRoIEta);
        //bool dRpass = ( 0.02 > l2cbdR );
        //ATH_MSG_INFO("matchCB, dR: " << l2cbdR);
        //if ( (isActiveTE) && dRpass ){
        //  isPassedCB = 1;
        //  goto RoIMatchingAndCBPassed;
        //}
      }
    }

    // offline
    double tmp_offlineMatchedRoiEta = -9999.;
    double tmp_offlinePtCutMatchedRoiEta = -9999.;
    xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
    xAOD::MuonContainer::const_iterator mu_end  = muons->end();
    for ( ; mu_itr!=mu_end; ++mu_itr ) {
      const double muPt     = (*mu_itr)->pt();
      ATH_MSG_INFO("offline muon loop, muPt: " << muPt/1000. << ", chain threshold: " << m_trigThreshold[iChain]);
      // off
      tmp_offlineMatchedRoiEta = offlineMatching((*mu_itr), rois, m_trigL1[iChain], m_trigHLT[iChain]);
      if (tmp_offlineMatchedRoiEta > -99) {
        m_h_countOff[iChain] -> Fill(tmp_offlineMatchedRoiEta);
        // off pt cut
        if ( muPt/1000. >= m_trigThreshold[iChain] ) {
          ATH_MSG_DEBUG("offline pT >= chain threshold");
          m_h_countOffPtCut[iChain] -> Fill(tmp_offlineMatchedRoiEta);
        }
      }
    }

  } //end of loop for each chain


  return StatusCode::SUCCESS;
}


double CalcEffAlg :: offlineMatching ( const xAOD::Muon *muon, const xAOD::MuonRoIContainer *rois, const TagAndProbe::L1Item trigL1, const string trigHLT){
  ATH_MSG_INFO("offlineMatching()");
  ATH_MSG_INFO("trigL1 = " << trigL1 << ", trigHLT = " << trigHLT);

  int matchedRoINumber = -1;
  double matchedRoIEta = -9999.;

  //L1
  ATH_MSG_DEBUG("L1 matching in offlineMatching()");
  int isPassedL1 = -1;
  const double muPt   = muon->pt();
  const double muEta  = muon->eta();
  const double muPhi  = muon->phi();
  const xAOD::TrackParticle* mutrk = muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
  pair< double, double > extEtaAndPhi;
  //ATH_MSG_DEBUG("mutrk1" << extEtaAndPhi.first << ", " << extEtaAndPhi.second);
  if (mutrk) extEtaAndPhi = m_ext.extTrack( mutrk );
  //ATH_MSG_DEBUG("mutrk2" << extEtaAndPhi.first << ", " << extEtaAndPhi.second);
  const double muExtEta = ( mutrk )? extEtaAndPhi.first:muEta;
  const double muExtPhi = ( mutrk )? extEtaAndPhi.second:muPhi;
  //ATH_MSG_DEBUG("mutrk3" << extEtaAndPhi.first << ", " << extEtaAndPhi.second);

  xAOD::MuonRoIContainer::const_iterator roi_itr = rois->begin();
  xAOD::MuonRoIContainer::const_iterator roi_end = rois->end();
  double reqdR = ( muPt < 1000.) ? -0.00001*muPt + 0.18 : 0.08;
  for(; roi_itr!=roi_end; ++roi_itr){
    int roiThr       = (*roi_itr)->getThrNumber();
    double roiEta    = (*roi_itr)->eta();
    double roiPhi    = (*roi_itr)->phi();
    const int roiNum = (*roi_itr)->getRoI();
    const int roiPt  = (*roi_itr)->thrValue();
    ATH_MSG_DEBUG("L1: roiNum = " << roiNum
        << ", roiThr = " << roiThr
        << ", roiPt = " << roiPt
        << ", roiEta = " << roiEta);
    // Check dR
    const double roidRof  = m_utils.deltaR( muEta, muPhi, roiEta, roiPhi);
    const double roidRex  = m_utils.deltaR( muExtEta, muExtPhi, roiEta, roiPhi);
    bool isdR = ( m_useExt )? ( reqdR > roidRex ):( reqdR > roidRof );
    ATH_MSG_DEBUG("isdR = " << isdR << ", reqdR = " << reqdR << ", roidRex = " << roidRex << ", roidRof = " << roidRof);
    // Check L1 pass
    if( (roiThr >= trigL1) && isdR ){
      matchedRoINumber = roiNum;
      reqdR = ( m_useExt )? ( reqdR > roidRex ):( reqdR > roidRof );
      matchedRoIEta = roiEta;
      isPassedL1 = 1;
      ATH_MSG_DEBUG("Matched in L1");
    } else {
      continue;
    }
  } // roi loop

  ATH_MSG_DEBUG("isPassedL1:" << isPassedL1 << ", L1 roiNum: " << matchedRoINumber);

  if ( isPassedL1 < 0 ){
    return -9999;
  }

  //SA
  ATH_MSG_DEBUG("SA matching in offlineMatching()");
  int isPassedSA = -1;
  int l2saRoINum = -1;
  std::string trigSAName = "";
  getSATEName( trigHLT, trigSAName );
  Trig::FeatureContainer fcL2SA = m_trigDecTool->features( trigHLT, TrigDefs::alsoDeactivateTEs );
  const std::vector< Trig::Feature<xAOD::L2StandAloneMuonContainer> > fL2SAs = fcL2SA.get<xAOD::L2StandAloneMuonContainer>( "", TrigDefs::alsoDeactivateTEs );
  ATH_MSG_DEBUG("fL2SAs size = " << fL2SAs.size());
  Trig::ExpertMethods* expert = m_trigDecTool -> ExperimentalAndExpertMethods();
  expert->enable();
  for(auto fL2SA : fL2SAs){
    const HLT::TriggerElement *trigElementSA = fL2SA.te();
    std::vector<HLT::TriggerElement*> TEsuccessors = expert->getNavigation()->getDirectSuccessors(trigElementSA);
    Bool_t isActiveTE = kFALSE;
    for(auto te : TEsuccessors){
      if ( te -> getActiveState() ){
        TString teName = Trig::getTEName( *te );
        if ( (teName.Contains( "L2_mu_SAhyp") && teName.Contains( trigSAName.c_str() ) )|| ( teName.Contains("L2_mu_hypo1" ) ) )
          isActiveTE = kTRUE;
      }
      ATH_MSG_DEBUG("TEName = " << Trig::getTEName( *te ) << ", getActiveState = " << te -> getActiveState() << ", isActiveTE = " << isActiveTE);
    }
    const xAOD::L2StandAloneMuonContainer* cont = fL2SA.cptr();
    ATH_MSG_DEBUG("L2SA cont size = " << cont->size());
    for( const auto& l2sa : *cont ) {
      l2saRoINum  = l2sa->roiNumber();
      double l2saPt  = l2sa->pt();
      double l2saRoIEta  = l2sa->roiEta();
      if ( l2saRoINum == matchedRoINumber && isActiveTE ){
        isPassedSA = 1;
        goto RoIMatchingAndSAPassed;
      }
    }
  }
RoIMatchingAndSAPassed:

  ATH_MSG_DEBUG("isPassedSA:" << isPassedSA << ", SA roiNum: " << l2saRoINum);

  if ( isPassedSA < 0 ){
    return -9999;
  }

  // CB
  ATH_MSG_DEBUG("CB matching in offlineMatching()");
  int isPassedCB = -1;
  int l2cbRoINum = -1;
  std::string trigCBName = "";
  getCBTEName( trigHLT, trigCBName );
  Trig::FeatureContainer fcL2CB = m_trigDecTool->features( trigHLT, TrigDefs::alsoDeactivateTEs );
  const std::vector< Trig::Feature<xAOD::L2CombinedMuonContainer> > fL2CBs = fcL2CB.get<xAOD::L2CombinedMuonContainer>( "", TrigDefs::alsoDeactivateTEs );
  ATH_MSG_DEBUG("fL2CBs size = " << fL2CBs.size());
  Trig::ExpertMethods* expertCB = m_trigDecTool -> ExperimentalAndExpertMethods();
  expertCB->enable();
  for(auto& fL2CB : fL2CBs){
    const HLT::TriggerElement *trigElementCB = fL2CB.te();
    std::vector<HLT::TriggerElement*> TEsuccessors = expertCB->getNavigation()->getDirectSuccessors(trigElementCB);
    Bool_t isActiveTE = kFALSE;
    for(auto te : TEsuccessors){
      if ( te -> getActiveState() ){
        TString teName = Trig::getTEName( *te );
        if ( ( teName.Contains( "L2_mucombhyp") && teName.Contains( trigCBName.c_str() ) ) || teName.Contains( "L2_mu_hypo2" ) )
          isActiveTE = kTRUE;
      }
      ATH_MSG_DEBUG("TEName = " << Trig::getTEName( *te ) << ", getActiveState = " << te -> getActiveState() << ", isActiveTE = " << isActiveTE);
    }
    const xAOD::L2CombinedMuonContainer* cont = fL2CB.cptr();
    ATH_MSG_DEBUG("L2CB cont size = " << cont->size());
    for( const auto& l2cb : *cont ) {
      const double l2cbEta  = l2cb->eta();
      const double l2cbPhi  = l2cb->phi();
      const double l2cbdR = m_utils.deltaR( muEta, muPhi, l2cbEta, l2cbPhi);
      bool dRpass = ( 0.02 > l2cbdR );
      ATH_MSG_DEBUG("matchCB, dR: " << l2cbdR);
      if ( (isActiveTE) && dRpass ){
        isPassedCB = 1;
        goto RoIMatchingAndCBPassed;
      }
    }
  }
RoIMatchingAndCBPassed:

  ATH_MSG_DEBUG("isPassedCB:" << isPassedCB );

  if ( isPassedCB < 0 ){
    return -9999;
  }


  // EF
  ATH_MSG_DEBUG("EF matching in offlineMatching()");
  int isPassedEF = -1;
  Trig::FeatureContainer fcEF = m_trigDecTool->features( trigHLT, TrigDefs::alsoDeactivateTEs );
  const std::vector< Trig::Feature<xAOD::MuonContainer> > fEFs = fcEF.get<xAOD::MuonContainer>( "", TrigDefs::alsoDeactivateTEs );

  double efPt     = -99999;
  double efEta    = -99999;
  double efPhi    = -99999;
  double efCharge = -99999;
  double efdRmu   = 0.02;
  for ( auto& fEF : fEFs ) {
    const HLT::TriggerElement* efTE = ( fEF.te() );
    const xAOD::MuonContainer* cont = fEF.cptr();
    for( const auto& ef : *cont ) {
      if( 0.02 < m_utils.deltaR( muEta, muPhi, ef->eta(), ef->phi() ) ) continue;
      if( !efTE->getActiveState() ) continue;
      efPt      = ef->pt();
      efEta     = ef->eta();
      efPhi     = ef->phi();
      efCharge  = ef->charge();
      efdRmu    = m_utils.deltaR( muEta, muPhi, efEta, efPhi );
    }
  }
  bool pass = ( m_utils.muonEFThreshold( efEta, efPt*0.001, trigHLT ) );
  //if( m_message>1 ) cout << "EF : " << muPt << ", " << efPt << " ==> " << chain << ": " << pass << endl;
  if( pass ) {
    isPassedEF = 1;
  }

  ATH_MSG_DEBUG("isPassedEF:" << isPassedEF);

  if ( isPassedEF < 0 ){
    return -9999;
  }

  return matchedRoIEta;
}


Bool_t CalcEffAlg::getSATEName( const std::string& mesHLT, std::string& teName )
{
  ///This function is used in "int TagAndProbe::addMesChain ".
  ///mesHLT and teName is parameters.
  TString hlt = mesHLT.c_str();
  TObjArray* toa = hlt . Tokenize( "_" );
  Int_t thrValue = 0;
  Bool_t isBarrelOnly = kFALSE;
  for( Int_t i = 0; i < toa -> GetEntries(); i++ ){
    TString tsToken = (static_cast<TObjString*>( toa -> At(i) ) ) -> String();
    if ( tsToken . Contains( "0eta105" ) ){
      isBarrelOnly = kTRUE;
    }
    if ( !( tsToken.Contains( "mu" ) || tsToken.Contains( "MU" ) ) ) continue;
    tsToken = tsToken.ReplaceAll( "noL1", "" );
    tsToken = tsToken.ReplaceAll( "L1", "" );
    tsToken = tsToken.ReplaceAll( "3mu", "" );
    tsToken = tsToken.ReplaceAll( "2mu", "" );
    tsToken = tsToken.ReplaceAll( "mu", "" );
    tsToken = tsToken.ReplaceAll( "MU", "" );
    if ( !tsToken . IsDec() )
      continue;
    thrValue = tsToken . Atoi();
  }
  if ( thrValue == 0 )
    return 1;
  if ( thrValue == 4 || thrValue == 2 || isBarrelOnly ){
  } else {
    thrValue = 6;
  }
  teName = Form( "%dGeV%s_v15a", thrValue, (isBarrelOnly)?("_barrelOnly"):("") );
  ATH_MSG_DEBUG("CalcEffAlg: Derived TESAName=" << teName);
  return 0;
}

Bool_t CalcEffAlg::getCBTEName( const std::string& mesHLT, std::string& teName )
{
  TString hlt = mesHLT.c_str();
  TObjArray* toa = hlt . Tokenize( "_" );
  Int_t thrValue = 0;
  for( Int_t i = 0; i < toa -> GetEntries(); i++ ){
    TString tsToken = (static_cast<TObjString*>( toa -> At(i) ) ) -> String();
    if ( !( tsToken.Contains( "mu" ) || tsToken.Contains( "MU" ) ) ) continue;
    tsToken = tsToken.ReplaceAll( "noL1", "" );
    tsToken = tsToken.ReplaceAll( "L1", "" );
    tsToken = tsToken.ReplaceAll( "3mu", "" );
    tsToken = tsToken.ReplaceAll( "2mu", "" );
    tsToken = tsToken.ReplaceAll( "mu", "" );
    tsToken = tsToken.ReplaceAll( "MU", "" );
    if ( !tsToken . IsDec() )
      continue;
    thrValue = tsToken . Atoi();
  }
  if ( thrValue == 0 )
    return 1;
  if ( thrValue >= 24 )
    thrValue = 22;
  teName = Form( "_mu%d_", thrValue );
  ATH_MSG_DEBUG("CalcEffAlg: Derived TECBName=" << teName);
  return 0;
}
