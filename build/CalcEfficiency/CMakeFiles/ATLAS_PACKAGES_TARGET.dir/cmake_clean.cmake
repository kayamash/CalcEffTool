file(REMOVE_RECURSE
  "../x86_64-centos7-gcc62-opt/python/CalcEfficiency/__init__.py"
  "../x86_64-centos7-gcc62-opt/include/CalcEfficiency"
  "../x86_64-centos7-gcc62-opt/include/CalcEfficiency"
  "../x86_64-centos7-gcc62-opt/jobOptions/CalcEfficiency/CalcEffAlg_options.py"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ATLAS_PACKAGES_TARGET.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
