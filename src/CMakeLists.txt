cmake_minimum_required(VERSION 3.20)

project(src)

# 设置项目名称
set(CMAKE_CXX_STANDARD 14)

# set(SPDK_INCLUDE_DIR "../../spdk/include")
# set(SPDK_LIB_DIR "/home/spdk/build/lib")
# set(DPDK_LIB_DIR "/home/spdk/dpdk/build/lib")
# set(DPDK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../spdk/dpdk/build/include")
set(LIB_LIGHTNVM_DIR "/home/liblightnvm/build")
set(LIB_LIGHTNVM_INCLUDE_DIR "/home/liblightnvm/include")

# include_directories(${SPDK_INCLUDE_DIR}  ${DPDK_INCLUDE_DIR} )
# link_directories(${SPDK_LIB_DIR} ${DPDK_LIB_DIR} "/home/spdk/isa-l/.libs" )

include_directories(${LIB_LIGHTNVM_INCLUDE_DIR})
link_directories(${LIB_LIGHTNVM_DIR})


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
    Debug/Debug.h
    Ti-OCSSD/TiOCS.h Ti-OCSSD/TiOCS.cpp Ti-OCSSD/MemoryTier/MemTier.h
    Ti-OCSSD/MemoryTier/MemTier.cpp Ti-OCSSD/MemoryTier/Node.h Ti-OCSSD/MemoryTier/Node.cpp
    Ti-OCSSD/utility/types.h Ti-OCSSD/utility/types.cpp Ti-OCSSD/StoreTier/asyncstore.h Ti-OCSSD/StoreTier/asyncstore.cpp
    Ti-OCSSD/StoreTier/syncstore.h Ti-OCSSD/StoreTier/syncstore.cpp  
    LRU/lru.h LRU/lru.cpp MultiHash/ExtendibleHash/ExBucket.h MultiHash/ExtendibleHash/ExBucket.h
    Backend/IODisk/WriteDisk.h Backend/BackendMain.h Backend/IODisk/WriteDisk.cpp  
    Backend/BlockManage/BlockInfo.cpp Backend/BlockManage/BlockInfo.h
    Backend/SSDWrite/writer.h Backend/SSDWrite/writer.cpp Backend/SSDRead/reader.h Backend/SSDRead/reader.cpp
    Backend/backend_variables.h Backend/backend_variables.cpp Backend/SSDWrite/parallel_writer.h Backend/SSDWrite/parallel_writer.cpp
    Auxizilary/GlobalVariable.h Auxizilary/pre_definition.h Auxizilary/SysOutput.h AuxizilaryImplement/SysOutput.cpp 
    MultiHash/ExtendibleHash/ExHash.h MultiHash/ExtendibleHash/ExHash.cpp MultiHash/LinearHash/LiHash.h
    MultiHash/ExtendibleHash/ExNode.h MultiHash/ExtendibleHash/ExBucket.h MultiHash/ExtendibleHash/ExBucket.cpp
    MultiHash/LinearHash/LiHash.cpp MultiHash/Bucket.h MultiHash/LazySplitHash/LSHash.h MultiHash/LazySplitHash/LSHash.cpp
    MultiHash/EagerSplitHash/ESHash.h MultiHash/EagerSplitHash/ESHash.cpp
    FIFO/FIFO.h FIFO/FIFO.cpp
    global_variables/global_variables.h global_variables/global_variables.cpp)

target_link_libraries(src
                    lightnvm_cli
                    lightnvm
                    )





