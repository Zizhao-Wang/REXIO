# CMakeLists.txt for a subdirectory

# Gather source files in the current directory
file(GLOB_RECURSE CURRENT_DIR_SRC
    "*.h"
    "*.cpp"
)

# If this subdirectory is a library
add_library(Memlayer ${CURRENT_DIR_SRC})

# If there are other libraries to link

