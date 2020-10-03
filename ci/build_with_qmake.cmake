set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)

if(CMAKE_HOST_WIN32)
  set(qt_target_dir "msvc2019_64")
elseif(CMAKE_HOST_APPLE)
  set(qt_target_dir "clang_64")
else()
  set(qt_target_dir "gcc_64")
endif()

if(NOT SOURCE_DIR)
  set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# Expects a path to a Qt installed via the Qt installer, which has a subdir for each target platform.
# e.g. /path/to/Qt/5.15.1 (has a subdir /path/to/qt/5.15.1/gcc_64)
if(NOT QT_PREFIX_DIR)
  set(QT_PREFIX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/official_qt")
  message(STATUS "Inferring Qt prefix location to: ${QT_PREFIX_DIR}")
else()
  get_filename_component(QT_PREFIX_DIR "${QT_PREFIX_DIR}" ABSOLUTE)
endif()

set(qmake_path "${QT_PREFIX_DIR}/${qt_target_dir}/bin/qmake")
if(CMAKE_HOST_WIN32)
  string(APPEND qmake_path ".exe")
endif()

message(STATUS "Using qmake at ${qmake_path}.")

if(NOT BUILD_DIR)
  set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
endif()
set(build_dir "${BUILD_DIR}")

message(STATUS "Setting build directory to ${build_dir}.")
file(MAKE_DIRECTORY "${build_dir}")


# I'm surprised this works on Windows. CMake docs says the process
# is executed using system API directly (without an intermediate shell).
# And yet it seems to work.
set(command_prefix_args "")
if(ENV_SCRIPT)
  set(command_prefix_args "${ENV_SCRIPT}" "&&")
endif()

execute_process(COMMAND 
                ${command_prefix_args}
                "${qmake_path}" "${SOURCE_DIR}"
                WORKING_DIRECTORY "${build_dir}"
                RESULT_VARIABLE result)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Failed with error: ${result}")
endif()

if(CMAKE_HOST_WIN32)
  execute_process(COMMAND 
                  ${command_prefix_args}
                  "nmake"
                  WORKING_DIRECTORY "${build_dir}"
                  RESULT_VARIABLE result)
else()
  execute_process(
                  ${command_prefix_args}
                  COMMAND "make"
                  WORKING_DIRECTORY "${build_dir}"
                  RESULT_VARIABLE result)
endif()
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Failed with error: ${result}")
endif()
