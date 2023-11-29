cmake_minimum_required(VERSION 3.20)
project(TestNEXIO)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)



set(SPDK_ROOT_DIR "/home")
# Set default paths for SPDK and DPDK. Users can modify these using -DSPDK_INCLUDE_DIR=<path> etc.
set(SPDK_INCLUDE_DIR "${SPDK_ROOT_DIR}/spdk/include" CACHE PATH "Path to SPDK include directory")
set(SPDK_LIB_DIR "${SPDK_ROOT_DIR}/spdk/build/lib" CACHE PATH "Path to SPDK library directory")
set(DPDK_LIB_DIR "${SPDK_ROOT_DIR}/spdk/dpdk/build/lib" CACHE PATH "Path to DPDK library directory")
set(DPDK_INCLUDE_DIR "${SPDK_ROOT_DIR}/spdk/dpdk/build/include" CACHE PATH "Path to DPDK include directory")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${SPDK_INCLUDE_DIR} ${DPDK_INCLUDE_DIR})
link_directories(${SPDK_LIB_DIR} ${DPDK_LIB_DIR} "${SPDK_ROOT_DIR}/spdk/isa-l/.libs")


# 添加OpenMP支持
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


find_package(gflags REQUIRED)


# Add the source files
file(GLOB TEST_SOURCES "*.cpp")

# Add the header files (optional, for clarity)
file(GLOB TEST_HEADERS "*.h")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# 创建测试程序的可执行文件
add_executable(TestNEXIO ${TEST_SOURCES} ${TEST_HEADERS})


# 链接测试程序需要的库
target_link_libraries(TestNEXIO
    Memlayer
    Disklayer 
    Utility
    gflags
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


