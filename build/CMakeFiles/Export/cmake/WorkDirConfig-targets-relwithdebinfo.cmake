#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "WorkDir::CalcEfficiency" for configuration "RelWithDebInfo"
set_property(TARGET WorkDir::CalcEfficiency APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(WorkDir::CalcEfficiency PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libCalcEfficiency.so"
  IMPORTED_NO_SONAME_RELWITHDEBINFO "TRUE"
  )

list(APPEND _IMPORT_CHECK_TARGETS WorkDir::CalcEfficiency )
list(APPEND _IMPORT_CHECK_FILES_FOR_WorkDir::CalcEfficiency "${_IMPORT_PREFIX}/lib/libCalcEfficiency.so" )

# Import target "WorkDir::CalcEfficiencyLib" for configuration "RelWithDebInfo"
set_property(TARGET WorkDir::CalcEfficiencyLib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(WorkDir::CalcEfficiencyLib PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libCalcEfficiencyLib.so"
  IMPORTED_SONAME_RELWITHDEBINFO "libCalcEfficiencyLib.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS WorkDir::CalcEfficiencyLib )
list(APPEND _IMPORT_CHECK_FILES_FOR_WorkDir::CalcEfficiencyLib "${_IMPORT_PREFIX}/lib/libCalcEfficiencyLib.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
