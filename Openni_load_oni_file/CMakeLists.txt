# ---------------proj 2
# project(openni_load_oni_file)

include_directories($ENV{OPEN_NI_INCLUDE} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_directories($ENV{OPEN_NI_LIB} ${OpenCV_LIB_DIR} ${Boost_LIBRARY_DIRS})
file(GLOB SRC_LOAD_RAW openni_load_oni_file.cpp)
add_executable(Openni_load_oni_file ${SRC_LOAD_RAW})
target_link_libraries(Openni_load_oni_file ${OPENNI_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES})



