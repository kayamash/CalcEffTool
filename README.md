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
$ setupATLAS
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




