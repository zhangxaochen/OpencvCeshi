cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# ---------------proj OpencvCalib:
# project(OpencvCalib)

include_directories($ENV{OPEN_NI_INCLUDE} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_directories($ENV{OPEN_NI_LIB} ${OpenCV_LIB_DIR} ${Boost_LIBRARY_DIRS})
file(GLOB SRC_OPENCV_CALIB OpencvCalib.cpp)
add_executable(OpencvCalib ${SRC_OPENCV_CALIB})
target_link_libraries(OpencvCalib ${OPENNI_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES})

