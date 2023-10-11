cmake_minimum_required(VERSION 3.20)

project(src)

# 设置项目名称
set(CMAKE_CXX_STANDARD 14)

set(SPDK_INCLUDE_DIR "../../spdk/include")
set(SPDK_LIB_DIR "/wzz/spdk/build/lib")
set(DPDK_LIB_DIR "/wzz/spdk/dpdk/build/lib")
set(DPDK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../spdk/dpdk/build/include")
# set(LIB_LIGHTNVM_DIR "/home/liblightnvm/build")

include_directories(${SPDK_INCLUDE_DIR}  ${DPDK_INCLUDE_DIR} )

link_directories(${SPDK_LIB_DIR} ${DPDK_LIB_DIR} "/wzz/spdk/isa-l/.libs" )

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS"${CMAKE_EXE_LINKER_FLAGS}${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_compile_options()

# 添加可执行文件
add_executable(src main.cpp  
                LOCS/locs.h LOCS/locs.cpp LOCS/buffer.h LOCS/buffer.cpp LOCS/level.h LOCS/level.cpp 
                    LOCS/merge.h LOCS/merge.cpp LOCS/run.h LOCS/run.cpp AuxizilaryImplement/SysOutput.cpp
                    LOCS/allocation.h LOCS/allocation.cpp LOCS/global_variables.h LOCS/global_variables.cpp
                    LOCS/io_scheduler.h LOCS/io_scheduler.cpp Debug/debug_micros.h 
                    Auxizilary/pre_definition.h  Auxizilary/pre_definition.cpp 
                    Ti-OCSSD/TiOCS.cpp Ti-OCSSD/TiOCS.h Ti-OCSSD/MemoryTier/Node.h Ti-OCSSD/MemoryTier/Node.cpp
                    Ti-OCSSD/MemoryTier/MemTier.h Ti-OCSSD/MemoryTier/MemTier.cpp Ti-OCSSD/utility/types.h
                    Ti-OCSSD/utility/types.cpp Ti-OCSSD/StoreTier/syncstore.h Ti-OCSSD/StoreTier/syncstore.cpp
                    Ti-OCSSD/utility/types.cpp Ti-OCSSD/StoreTier/syncstore.h Ti-OCSSD/StoreTier/syncstore.cpp
                    Ti-OCSSD/StoreTier/io_manager.h Ti-OCSSD/StoreTier/io_manager.cpp Ti-OCSSD/StoreTier/assistant.h
                    Ti-OCSSD/StoreTier/assistant.cpp
                    LRU/lru.h LRU/lru.cpp)
                    
#                     
#                     
target_link_libraries(src
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
                    rt)





