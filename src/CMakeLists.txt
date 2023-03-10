cmake_minimum_required(VERSION 3.20)


project(src)


# set the project name
set(CMAKE_CXX_STANDARD 14)


include_directories("../../liblightnvm/include/")


link_libraries("/home/liblightnvm/build/liblightnvm_cli.a"  "/home/liblightnvm/build/liblightnvm.a")


FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS"${CMAKE_EXE_LINKER_FLAGS}${OpenMP_EXE_LINKER_FLAGS}")
endif()


# add the executable
add_executable(src main.cpp 
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
				LSM-Tree/LsmTree.h LSM-Tree/LsmTree.cpp LSM-Tree/UtilityDefine/buffer.h LSM-Tree/UtilityImplement/buffer.cpp 
				LSM-Tree/UtilityDefine/ResAllocate.h LSM-Tree/UtilityImplement/ResAllocate.cpp LSM-Tree/UtilityDefine/level.h 
				LSM-Tree/UtilityImplement/level.cpp LSM-Tree/UtilityDefine/merge.h LSM-Tree/UtilityImplement/merge.cpp 
				LSM-Tree/UtilityDefine/run.h LSM-Tree/UtilityImplement/run.cpp LSM-Tree/UtilityDefine/node2.h LSM-Tree/UtilityDefine/node2.cpp
				LSMTree-NoFTL/StorageManager.h  LSMTree-NoFTL/StorageManager.cpp LSMTree-NoFTL/run.h LSMTree-NoFTL/run.cpp 
				LSMTree-NoFTL/level.h LSMTree-NoFTL/level.cpp LSMTree-NoFTL/buffer.h LSMTree-NoFTL/buffer.cpp
				LSMTree-NoFTL/merge.h LSMTree-NoFTL/merge.cpp LSMTree-NoFTL/region.h LSMTree-NoFTL/region.cpp
				LSMTree-NoFTL/allocation.h  LSMTree-NoFTL/allocation.cpp
				FIFO/FIFO.h FIFO/FIFO.cpp)
