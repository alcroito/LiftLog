set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)

if(CMAKE_HOST_WIN32)
  set(qt_target_dir "msvc2019_64")
elseif(CMAKE_HOST_APPLE)
  set(qt_target_dir "clang_64")
else()
  set(qt_target_dir "gcc_64")
endif()

# Expects a path to a Qt installed via the Qt installer, which has a subdir for each target platform.
# e.g. /path/to/Qt/5.15.1 (has a subdir /path/to/qt/5.15.1/gcc_64)
if(NOT QT_PREFIX_DIR)
  set(QT_PREFIX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/official_qt")
  message(STATUS "Inferring Qt prefix location to: ${QT_PREFIX_DIR}")
else()
  get_filename_component(QT_PREFIX_DIR "${QT_PREFIX_DIR}" ABSOLUTE)
endif()

if(NOT TARGET_APP_PATH)
    message(FATAL_ERROR "No TARGET_APP_PATH path given: '${TARGET_APP_PATH}'.")
endif()
get_filename_component(TARGET_APP_PATH "${TARGET_APP_PATH}" ABSOLUTE)

set(qt_bin_dir "${QT_PREFIX_DIR}/${qt_target_dir}/bin")

# TODO Implement for all platforms.
if(CMAKE_HOST_APPLE)
  set(deploy_tool "${qt_bin_dir}/macdeployqt")
  message(STATUS "Using '${deploy_tool}'.")

  set(args "")
  list(APPEND args "${TARGET_APP_PATH}")

  if(QML_DIR)
    list(APPEND args "-qmldir=${QML_DIR}")
  endif()

  if(VERBOSE)
    list(APPEND args "-verbose=3")
  endif()

  execute_process(COMMAND 
                  "${deploy_tool}"
                  ${args}
                  RESULT_VARIABLE result)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Failed with error: ${result}")
  endif()
endif()