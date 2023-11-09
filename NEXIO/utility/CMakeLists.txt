# CMakeLists.txt for a subdirectory

# Gather source files in the current directory
file(GLOB_RECURSE CURRENT_DIR_SRC
    "*.h"
    "*.cpp"
)


# If this subdirectory is a library
add_library(Utility ${CURRENT_DIR_SRC})



