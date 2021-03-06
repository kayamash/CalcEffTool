
#LocalInputFileList="local_valid_r9026.list"
#LocalInputFileList="r9311.list"
#LocalInputFileList="r9539.list"
#LocalInputFileList="r9539_Zmumu.list"
#LocalInputFileList="r9193.list"
#LocalInputFileList="inEITrkConfFalsev3.list"
#print "LocalInputFileList is"
#print LocalInputFileList
#f = open( LocalInputFileList, 'r' )
#InputFileList = f.read().splitlines()
#print InputFileList
#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#-----------------------------------------------------------------------------
# Message Service
#-----------------------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = ERROR
import AthenaServices
AthenaServices.AthenaServicesConf.AthenaEventLoopMgr.OutputLevel = ERROR

#-----------------------------------------------------------------------------
# Input Datasets
#-----------------------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
if not acf.EvtMax.is_locked():
  acf.EvtMax=-1
#acf.FilesInput = InputFileList
acf.FilesInput += [
        #"/gpfs/fs7001/kayamash/mc16_13TeV.410472.PhPy8EG_A14_ttbar_hdamp258p75_dil.deriv.DAOD_MUON0.e6348_e5984_s3126_r10724_r10726_p3613/DAOD_MUON0.15084884._000599.pool.root.1"
        #"/gpfs/fs6001/kayamash/mc16_13TeV.345100.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV70_140_VBFfilt.recon.AOD.e5679_e5984_s3126_r9781/AOD.12538886._000087.pool.root.1"
        "/gpfs/fs6001/kayamash/data18_13TeV.00364160.physics_Main.merge.AOD.f1002_m2037/data18_13TeV.00364160.physics_Main.merge.AOD.f1002_m2037._lb0232._0001.1"
        #"/gpfs/fs2001/yfukuhar/data/mc16_13TeV.424108.Pythia8B_A14_CTEQ6L1_Jpsimu6.merge.AOD.e5441_s3126_r9781_r9778/AOD.11857841._001391.pool.root.1"
        #"/gpfs/fs6001/yyazawa/data/valid1.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4.recon.AOD.e5112_s3091_r9539/AOD.11362347._000307.pool.root.1"
#"/gpfs/fs6001/yyazawa/data/valid1.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4.recon.AOD.e5112_s3091_r9122_tid10750758_00/AOD.10750758._000194.pool.root.1",
#"/gpfs/fs6001/yyazawa/data/valid1.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4.recon.AOD.e5112_s2887_r9026_tid10522817_00/AOD.10522817._000212.pool.root.1"
]
#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------
rec.doCBNT=False
from RecExConfig.RecFlags import rec
rec.doTrigger=True
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doTrigger=True
recAlgs.doAtlfast=False
recAlgs.doMonteCarloReact=False
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly=True
#rec.doWriteAOD=False
#rec.doWriteESD=False
#rec.doWriteTAG=False
#rec.doAOD=False
rec.doDPD=False
#rec.doESD=False
rec.doTruth=False
rec.doRecoTiming=False
rec.doDetStatus=False
rec.doShowSizeStatistics=False
rec.readTAG=False
rec.readRDO=False
rec.doHist=False
rec.doContainerRemapping=False
rec.doJiveXML=False
rec.doEdmMonitor=False
rec.doDumpPoolInputContent=False
rec.doHeavyIon=False
rec.doHIP=False
rec.doWriteBS=False
rec.doPhysValMonHists=False
rec.doVP1=False
rec.doJiveXML=False
rec.doMuon=False
rec.doCheckDictionary=False
rec.doFileMetaData=False
rec.doCalo=False
rec.doAODCaloCells=False
rec.doEgamma=False

rec.doESD.set_Value_and_Lock(False) # uncomment if do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG

include("RecExCommon/RecExCommon_topOptions.py")

#ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
from TrigNavigation.TrigNavigationConf import HLT__Navigation
ToolSvc += CfgMgr.Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc += CfgMgr.HLT__Navigation( "Navigation" )
ToolSvc.TrigDecisionTool.Navigation.ReadonlyHolders=True
ServiceMgr.MessageSvc.setWarning += [ "", "HolderFactory" ]
ServiceMgr.MessageSvc.infoLimit = 99999999
ServiceMgr.MessageSvc.debugLimit = 99999999

# GRL
ToolSvc += CfgMgr.GoodRunsListSelectionTool("MyGRLTool",GoodRunsListVec=["data18_13TeV.periodAllYear_HEAD_Unknown_PHYS_StandardGRL_All_Good_25ns.xml"])

from CalcEfficiency.CalcEfficiencyConf import *
job += CalcEffAlg( 'CalcEffAlg', 
                   OutputLevel = DEBUG,
                   OutputFile = "test.root",
                   TapMethod = "JPZtap",
                   Extrapolate = True,
                   GRL = False,
                   DataType = "data17"
                 )
                   #GRL = True
                   #TapMethod = "Ztap",
                   #OutputFile = "test_inEITrkConfFalsev3.root",

include("TriggerTest/TriggerTestCommon.py")
                     
print job

#-----------------------------------------------------------------------------

