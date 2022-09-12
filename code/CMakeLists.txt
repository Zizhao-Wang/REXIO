cmake_minimum_required(VERSION 3.20)
project(code)

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

add_executable(code main.cpp HashMethods.h FrontEnd/HashTable/HashTable.h FrontEnd/MemoryAllocate/HashBlock.h Backend/IODisk/WriteDisk.h FrontEnd/FrontMain.h Backend/BackendMain.h Auxizilary/GlobalVariable.h AuxizilaryImplement/GlobalVariable.cpp Auxizilary/VariablesTypes.h FrontEnd/FrontMain.cpp FrontEnd/HashTable/HashTable.cpp FrontEnd/GlobalVar/FrontGlobalVar.h FrontEnd/GlobalVar/FrontGlobalVar.cpp FrontEnd/MemoryAllocate/HashBlock.cpp Backend/IODisk/WriteDisk.cpp Backend/DiskRead/ReadDisk.cpp Backend/DiskRead/ReadDisk.h Exhashing/ExtendibleHash.cpp Backend/BlockManage/BlockInfo.cpp Backend/BlockManage/BlockInfo.h Linearhashing/LHash.h Linearhashing/LHash.cpp LSM-Tree/LsmTree.h LSM-Tree/LsmTree.cpp LSM-Tree/UtilityDefine/buffer.h LSM-Tree/UtilityImplement/buffer.cpp LSM-Tree/UtilityDefine/level.h LSM-Tree/UtilityImplement/level.cpp)
