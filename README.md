# CalcEffTool
------------------------------------
Author: Yukitaka yazawa
Email: yukitaka.yazawa[at]cern.ch

This is a package for calculating muon trigger efficiency.
This package is created by dkobayas(https://gitlab.cern.ch/dkobayas/CalcEffTool).
Compiling with cmake is available.

## Install
------------------------------------
On lxplus:

```sh
$ git clone ssh://git@gitlab.cern.ch:7999/yyazawa/CalcEffTool.git
$ cd CalcEffTool/
#export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
#alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
$ source setup_grid.sh
$ ./compile.sh
```


## How To Use Local
------------------------------------
```sh
$ cd run
$ vim your-aod-files.list
$ cat your-aod-files.list
/you-path-to-data/AOD.pool.root.1
/you-path-to-data/DAOD_MUON0.pool.root.1
$ vim CalcEffAlg_options.py
# edit joboption file to set the list
$ head -n 1 CalcEffAlg_options.py
LocalInputFileList="your-aod-files.list"
$ ./run.sh
```

## Notice about data17
------------------------------------
Option data17 was added in job option file.
It changes trigger from "HLT_mu20_2mu0noL1_JpsimumuFS" to "HLT_mu20_2mu2noL1_JpsimumuFS", which is the only running trigger in data17.

If you want to measure L1 MU20 and MU21 triggers in data17, you must write it as MU15 and MU20 in addMesChain function respectively(not MU20 and MU21), because current L1 trigger state was not reflected yet.

Different method to check the active state of trigger element was used in data17, so this option should be made empty for data16.

## How To Create List
------------------------------------
$./yy_newmakelist.sh -d AOD -o (list_name) -g (GRL)

This requires lsetup rucio

## How To Use Grid
------------------------------------
$./sub_pathena.sh (list_name) (output_name)

## Branch names
------------------------------------
| branch name | description |
|:------------:|:------------:|
| mes_n | number of trigger chains to measure |
| mes_name | names of trigger chains |
| tag_proc | id of procedure to tag muons <br> ( 1:J/&psi; tag & probe passing HLT_mu20_2mu2noL1_JpsimumuFS(intend to calculate efficiency L1 and L2, <br> 2:J/&psi; tap pass HLT_mu20_2mu4_JpsimumuL2(intend to calculate efficiency of EF, <br> 3:Z tap ) |
| sumReqdRL1 | sum of dR threshold between Level1 and offline (tag + probe) |
| sumReqdREF | sum of dR threshold between Event Filter and offline (tag + probe) |
| tag_ReqdRL1 | dR threshold between Level1 and offline of tag muon |
| tag_ReqdREF | dR threshold between EventFilter and offline of tag muon |
| tag_dRL1 | dR between Level1 and offline of tag muon |
| tag_dREF | dR between Level1 and offline of tag muon |
| tag_pt | offline p<sub>T</sub> of tag muon |
| tag_eta | offline &eta; of tag muon |
| tag_exteta | offline &eta; of tag muon ( extrapolated to Muon Spectrometer ) |
| tag_phi | offline &phi; of tag muon |
| tag_extphi | offline &phi; of tag muon ( extrapolated to Muon Spectrometer ) |
| tag_d0 | offline d<sub>0</sub> of tag muon |
| tag_z0 | offline z<sub>0</sub> of tag muon |
| tag_charge | offline charge of tag muon |
| probe_pt | offline p<sub>T</sub> of probe muon |
| probe_eta | offline &eta; of probe muon |
| probe_exteta | offline &eta; of probe muon ( extrapolated to Muon Spectrometer ) |
| probe_phi | offline &phi; of probe muon |
| probe_extphi | offline &phi; of probe muon ( extrapolated to Muon Spectrometer ) |
| probe_d0 | offline d<sub>0</sub> of probe muon |
| probe_z0 | offline z<sub>0</sub> of probe muon |
| probe_charge | offline charge of probe muon |
| tp_dR | offline dR between tag and probe muons |
| tp_deta | offline &eta; between tag and probe muons |
| tp_dphi | offline &phi; between tag and probe muons |
| tp_extdR | offline dR between tag and probe muons( extrapolated to Muon Spectrometer ) |
| tp_extdeta | offline &eta; between tag and probe muons( extrapolated to Muon Spectrometer ) |
| tp_extdphi | offline &phi; between tag and probe muons( extrapolated to Muon Spectrometer ) |
| tp_mass | reconstructed mass of tag and probe muons |

Branch names beginning with probe_mesL1, probe_mesSA, probe_mesCB, probe_mesEF and probe_mesFTF are variables of probe muon measured in Level1, muonSA, muComb, EventFilter and FTF respectively.  
For example,

| branch name | description |
|:------------:|:------------:|
| probe_mesSA_pass | whether pass muonSA or not ( -1 if non-pass ) |
| probe_mesSA_dR | dR between muonSA and offline muons |
| probe_mesSA_pt | p<sub>T</sub> measured in muonSA |
| probe_mesSA_eta | &eta; measured in muonSA ( in case of muonSA, etaIP ) |
| probe_mesSA_phi | &phi; measured in muonSA ( in case of muonSA, etaIP ) |
| probe_mesSA_etams | &eta; measured in muonSA ( etaMS ) |
| probe_mesSA_phims | &phi; measured in muonSA ( phiMS ) |
| probe_mesSA_etabe | &eta; measured in muonSA ( &eta; back-extrapolated to IP with same method of muComb ) |
| probe_mesSA_phibe | &phi; measured in muonSA ( &phi; back-extrapolated to IP with same method of muComb ) |


