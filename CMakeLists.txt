cmake_minimum_required (VERSION 3.1)

project (clearpath_sc_torque_control)

add_executable(clearpath_sc_torque_control clearpath_sc_torque_control.cpp)

target_include_directories(clearpath_sc_torque_control PRIVATE lib/inc/inc-private lib/inc/inc-private/linux lib/inc/inc-private/sFound lib/inc/inc-pub lib/LibINI/inc lib/LibLinuxOS/inc lib/LibXML/inc)


target_sources(clearpath_sc_torque_control PRIVATE lib/LibINI/src/iniparser.cpp lib/LibINI/src/dictionary.cpp)
target_sources(clearpath_sc_torque_control PRIVATE lib/LibLinuxOS/src/tekEventsLinux.cpp lib/LibLinuxOS/src/tekThreadsLinux.cpp lib/LibLinuxOS/src/version.cpp)
target_sources(clearpath_sc_torque_control PRIVATE lib/LibXML/src/ErrCodeStr.cpp lib/LibXML/src/pugixml.cpp)
target_sources(clearpath_sc_torque_control PRIVATE lib/src-linux/lnkAccessLinux.cpp lib/src-linux/SerialLinux.cpp)
target_sources(clearpath_sc_torque_control PRIVATE lib/src/converterLib.cpp lib/src/cpmAPI.cpp lib/src/cpmClassImpl.cpp lib/src/iscAPI.cpp lib/src/lnkAccessCommon.cpp lib/src/meridianNet.cpp lib/src/netCmdAPI.cpp lib/src/netCoreFmt.cpp lib/src/SerialEx.cpp lib/src/sysClassImpl.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(clearpath_sc_torque_control PRIVATE Threads::Threads dl rt)
