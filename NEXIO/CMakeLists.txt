# Root CMakeLists.txt

cmake_minimum_required(VERSION 3.20)
project(NEXIO-DB)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)

# Set default paths for SPDK and DPDK. Users can modify these using -DSPDK_INCLUDE_DIR=<path> etc.
set(SPDK_INCLUDE_DIR "/home/spdk/include" CACHE PATH "Path to SPDK include directory")
set(SPDK_LIB_DIR "/home/spdk/build/lib" CACHE PATH "Path to SPDK library directory")
set(DPDK_LIB_DIR "/home/spdk/dpdk/build/lib" CACHE PATH "Path to DPDK library directory")
set(DPDK_INCLUDE_DIR "/home/spdk/dpdk/build/include" CACHE PATH "Path to DPDK include directory")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${SPDK_INCLUDE_DIR} ${DPDK_INCLUDE_DIR})
link_directories(${SPDK_LIB_DIR} ${DPDK_LIB_DIR} "/home/spdk/isa-l/.libs")

# Check and set OpenMP
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


# Check and set dependencies
message(STATUS "Checking and installing dependencies...")
execute_process(COMMAND ./install_dependencies.sh RESULT_VARIABLE DEP_INSTALL_RESULT)


execute_process(COMMAND /home/NEXIO/detect_device.sh OUTPUT_VARIABLE device_type)
string(STRIP ${device_type} device_type_cleaned) 

if("${device_type_cleaned}" STREQUAL "0")
    add_definitions(-D__OCSSD__CONTROLLER__)
    message("OCSSD CONTROLLER")
elseif("${device_type_cleaned}" STREQUAL "1")
    add_definitions(-DNVME_SSD_CONTROLLER)
    message("NVME SSD CONTROLLER")
else()
    message(FATAL_ERROR "Unknown device type!")
endif()


# Include subdirectories
add_subdirectory(memlayer)
add_subdirectory(disklayer)
add_subdirectory(utility)
add_subdirectory(tests)

# Define the main executable (if present)
# Note: this might be moved to a CMakeLists.txt in a subdirectory
add_executable(NEXIO main.cpp)
target_link_libraries(NEXIO
    Memlayer
    Disklayer 
    Utility
    -Wl,--whole-archive
    spdk_log.a 
    spdk_env_dpdk.a 
    spdk_nvme.a 
    spdk_util.a 
    spdk_event.a
    spdk_sock.a
    spdk_rpc.a
    spdk_json.a
    spdk_vfio_user.a 
    spdk_jsonrpc.a
    spdk_nvmf.a 
    spdk_trace.a
    spdk_dma.a
    spdk_bdev.a
    spdk_accel.a
    spdk_notify.a
    spdk_init.a
    spdk_thread.a
    -Wl,--no-whole-archive
    rte_eal.a 
    rte_mempool.a 
    rte_telemetry.a 
    rte_ring.a 
    rte_kvargs.a 
    rte_bus_pci.a
    rte_pci.a 
    rte_vhost.a 
    rte_power.a
    isal.a
    uuid 
    numa 
    dl 
    rt
)
