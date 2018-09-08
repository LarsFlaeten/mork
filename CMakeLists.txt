set(LIBNAME "mork")

# Sources
message(STATUS "****    MORK    ****")
message(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})

include_directories(${SIMULATOR_SOURCES} ${PROJECT_SOURCE_DIR}/src)

file(GLOB SOURCE_FILES *.cpp */*.cpp */*.c)

# Libraries
set(LIBS "")
if(UNIX)
	set(LIBS ${LIBS} rt )
endif(UNIX)

# Static or shared?
set(LIBTYPE STATIC)
if(BUILD_SHARED)
	set(LIBTYPE SHARED)
endif(BUILD_SHARED)

# Create a static library
add_library(${LIBNAME} ${LIBTYPE} ${SOURCE_FILES})
target_link_libraries(${LIBNAME} ${LIBS} glfw3 GL X11 pthread Xrandr Xi dl)

# Adds SO Version and subversion. To be added to ensure ABI/API compatibility.
#SET_TARGET_PROPERTIES(${LIBNAME} PROPERTIES SOVERSION ${MORK_VERSION_MAJOR} VERSION ${MORK_VERSION})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
message(STATUS "Output path for " ${LIBNAME} " is " ${LIBRARY_OUTPUT_PATH})
link_directories(${PROJECT_SOURCE_DIR}/lib)


