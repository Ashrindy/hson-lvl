set(HEDGELIB_BUILD_HEDGERENDER OFF)
set(HEDGELIB_BUILD_HEDGETOOLS OFF)

set(HEDGELIB_DIR ${HE_LEVEL_EXTERNAL_DIR}/hedgelib)
set(MHEDGELIB_DIR ${HEDGELIB_DIR}/HedgeLib)

if(NOT EXISTS ${MHEDGELIB_DIR}/Dependencies/x64)
    execute_process(
        COMMAND powershell "-File" "${MHEDGELIB_DIR}/Get-Dependencies.ps1" "-A" "x64"
        WORKING_DIRECTORY "${MHEDGELIB_DIR}"
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

set(HEDGELIB_LOCAL_DEPENDENCIES_DIR "${MHEDGELIB_DIR}/Dependencies/x64")

add_subdirectory(${MHEDGELIB_DIR})
