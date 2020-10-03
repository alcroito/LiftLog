if(WIN32)
  set(installer_file_os "windows-x86")
  set(installer_file_ext "exe")
  set(qt_package_arch "win64_msvc2019_64")
elseif(APPLE)
  set(installer_file_os "mac-x64")
  set(installer_file_ext "dmg")
  set(qt_package_arch "clang_64")
else()
  set(installer_file_os "linux-x64")
  set(installer_file_ext "run")
  set(qt_package_arch "gcc_64")
endif()

set(installer_version "4.0.0_beta")

if(NOT PACKAGE)
  set(PACKAGE "qt.qt5.5151")
endif()
if(NOT QT_PACKAGE_ARCH)
    set(QT_PACKAGE_ARCH "${qt_package_arch}")
endif()

if(NOT QT_VERSION)
  set(QT_VERSION 6.0.0)
endif()

if(NOT INSTALL_ROOT)
  set(INSTALL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/official_qt")
endif()
get_filename_component(install_root_abs "${INSTALL_ROOT}" ABSOLUTE)

if(EXISTS "${install_root_abs}")
  message(STATUS "Found existing directory at ${install_root_abs}.")
  if(REUSE_CACHED_QT)
    message(STATUS "Assuming Qt is already installed at ${install_root_abs}. Exiting.")
    return()
  endif()
endif()

if(PACKAGES)
  set(packages_to_install "${PACKAGES}")
else()
  set(packages_to_install "${PACKAGE}.${QT_PACKAGE_ARCH}")
endif()

set(installer_file_base "qt-unified-${installer_file_os}-${installer_version}-online")

set(qt_base_url "http://download.qt.io/development_releases/online_installers")
set(installer_file "${installer_file_base}.${installer_file_ext}")

if(DEFINED ENV{TEMP})
  set(TMPDIR $ENV{TEMP})
else()
  set(TMPDIR "/tmp")
endif()

set(final_installer_path "${CMAKE_CURRENT_SOURCE_DIR}/${installer_file}")
if(EXISTS "${final_installer_path}")
  message(STATUS "Found existing installer at ${final_installer_path}.")
endif()

if(EXISTS "${final_installer_path}" AND REUSE_INSTALLER)
  message(STATUS "Reusing existing installer found at '${final_installer_path}'.")
else()
  message(STATUS "Downloading ${installer_file} from ${qt_base_url}/${installer_file} to ${TMPDIR}/${installer_file}")
  file(DOWNLOAD "${qt_base_url}/${installer_file}" ${TMPDIR}/${installer_file} SHOW_PROGRESS)
  message(STATUS "Copying ${TMPDIR}/${installer_file} to ${final_installer_path}")
  file(COPY "${TMPDIR}/${installer_file}" DESTINATION . FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
  file(REMOVE "${TMPDIR}/${installer_file}")
endif()

if(APPLE)
  execute_process(COMMAND hdiutil attach "${installer_file}")
  set(installer_file "/Volumes/${installer_file_base}/${installer_file_base}.app/Contents/MacOS/${installer_file_base}")
else()
  set(installer_file "./${installer_file}")
endif()

set(args "${installer_file}" install ${packages_to_install} --root "${install_root_abs}" --accept-licenses --accept-obligations --confirm-command --auto-answer telemetry-question=No,AssociateCommonFiletypes=No,installationErrorWithCancel=Ignore,OverwriteTargetDirectory=No)
string(REPLACE ";" " " args_space_delimited "${args}")
message(STATUS "Running ${args_space_delimited}")
execute_process(COMMAND ${args}
                RESULT_VARIABLE result)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Installation failed. Exit code: ${result}")
endif()

if(APPLE)
  execute_process(COMMAND hdiutil detach "/Volumes/${installer_file_base}")
endif()
