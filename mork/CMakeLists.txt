set(LIBNAME "mork")

# Sources
message(STATUS "****    MORK    ****")
message(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})

file(GLOB SOURCE_FILES *.cpp */*.cpp */*.c */*.cc)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/libraries ${PROJECT_SOURCE_DIR}/libraries/freetype/include ${PROJECT_SOURCE_DIR}/libraries/nlohmann)


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

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
link_directories(${PROJECT_SOURCE_DIR}/libraries/freetype/lib)
add_library(${LIBNAME} ${LIBTYPE} ${SOURCE_FILES})
set_property(TARGET ${LIBNAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${LIBNAME} ${LIBS} json-schema-validator assimp freetype glfw3 GL X11 pthread Xrandr Xi dl)

# Adds SO Version and subversion. To be added to ensure ABI/API compatibility.
#SET_TARGET_PROPERTIES(${LIBNAME} PROPERTIES SOVERSION ${MORK_VERSION_MAJOR} VERSION ${MORK_VERSION})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
message(STATUS "Output path for " ${LIBNAME} " is " ${LIBRARY_OUTPUT_PATH})
link_directories(${PROJECT_SOURCE_DIR}/lib)


# Install headers
FOREACH(subdir core glad atmosphere imgui math render scene resource ui util)
    FILE(GLOB include-files ${subdir}/*.h)
    INSTALL(FILES ${include-files} DESTINATION include/mork/${subdir})
ENDFOREACH(subdir)

# Install library
INSTALL(TARGETS ${LIBNAME} LIBRARY DESTINATION ${LIB_INSTALL_DIR} ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
