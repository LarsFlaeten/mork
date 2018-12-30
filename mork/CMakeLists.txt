set(LIBNAME "mork")

# Sources
message(STATUS "****    MORK    ****")
message(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})

file(GLOB SOURCE_FILES *.cpp */*.cpp */*.c)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/libraries ${PROJECT_SOURCE_DIR}/libraries/freetype/include)


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
link_directories(${PROJECT_SOURCE_DIR}/libraries/freetype/lib)
add_library(${LIBNAME} ${LIBTYPE} ${SOURCE_FILES})
set_property(TARGET ${LIBNAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${LIBNAME} ${LIBS} assimp freetype glfw3 GL X11 pthread Xrandr Xi dl)

# Adds SO Version and subversion. To be added to ensure ABI/API compatibility.
#SET_TARGET_PROPERTIES(${LIBNAME} PROPERTIES SOVERSION ${MORK_VERSION_MAJOR} VERSION ${MORK_VERSION})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
message(STATUS "Output path for " ${LIBNAME} " is " ${LIBRARY_OUTPUT_PATH})
link_directories(${PROJECT_SOURCE_DIR}/lib)


