# Sources
include_directories("${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")




#link_directories(${PROJECT_SOURCE_DIR}/libraries ${PROJECT_SOURCE_DIR}/lib)

set(EXENAME ex01-rendering1)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex02-matrices1)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex03-matrices2)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)


