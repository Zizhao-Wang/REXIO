cmake_minimum_required(VERSION 3.20)


project(src)


# set the project name
set(CMAKE_CXX_STANDARD 14)


include_directories("../../../liblightnvm/include/")


link_libraries(-llightnvm_cli -llightnvm)


FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS"${CMAKE_EXE_LINKER_FLAGS}${OpenMP_EXE_LINKER_FLAGS}")
endif()


# add the executable
add_executable(src main.cpp 
				TNCTree/TNCtree.h TNCTree/TNCtree.cpp TNCTree/MemoryTier/MemTier.h
				TNCTree/MemoryTier/MemTier.cpp TNCTree/MemoryTier/Node.h TNCTree/MemoryTier/Node.cpp
				TNCTree/utility/types.h TNCTree/utility/types.cpp TNCTree/StoreTier/asyncstore.h TNCTree/StoreTier/asyncstore.cpp
				TNCTree/StoreTier/syncstore.h TNCTree/StoreTier/syncstore.cpp
				Backend/IODisk/WriteDisk.h Backend/BackendMain.h Backend/IODisk/WriteDisk.cpp  
				Backend/BlockManage/BlockInfo.cpp Backend/BlockManage/BlockInfo.h
				Backend/SSDWrite/writer.h Backend/SSDWrite/writer.cpp Backend/SSDRead/reader.h Backend/SSDRead/reader.cpp
				Auxizilary/GlobalVariable.h Auxizilary/VariablesTypes.h Auxizilary/SysOutput.h AuxizilaryImplement/SysOutput.cpp 
				MultiHash/ExtendibleHash/ExHash.h MultiHash/ExtendibleHash/ExHash.cpp MultiHash/LinearHash/LiHash.h 
				MultiHash/LinearHash/LiHash.cpp MultiHash/Bucket.h MultiHash/LazySplitHash/LSHash.h MultiHash/LazySplitHash/LSHash.cpp
				MultiHash/EagerSplitHash/ESHash.h MultiHash/EagerSplitHash/ESHash.cpp
				LSM-Tree/LsmTree.h LSM-Tree/LsmTree.cpp LSM-Tree/UtilityDefine/buffer.h LSM-Tree/UtilityImplement/buffer.cpp 
				LSM-Tree/UtilityDefine/ResAllocate.h LSM-Tree/UtilityImplement/ResAllocate.cpp LSM-Tree/UtilityDefine/level.h 
				LSM-Tree/UtilityImplement/level.cpp LSM-Tree/UtilityDefine/merge.h LSM-Tree/UtilityImplement/merge.cpp 
				LSM-Tree/UtilityDefine/run.h LSM-Tree/UtilityImplement/run.cpp)
