# Sources
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/libraries
    ${CMAKE_CURRENT_SOURCE_DIR})




#link_directories(${PROJECT_SOURCE_DIR}/libraries ${PROJECT_SOURCE_DIR}/lib)

set(EXENAME ex00-framework)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

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

set(EXENAME ex04-fonts1)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex05-camera_and_scene)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex06-lights01)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex07-lights02)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex08-lights03)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex09-model01)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex10-model02)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex11-normal_mapping)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex12-resources)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)

set(EXENAME ex13-atmosphere)
add_executable(${EXENAME} ${EXENAME}.cpp)
set_property(TARGET ${EXENAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${EXENAME} mork)


