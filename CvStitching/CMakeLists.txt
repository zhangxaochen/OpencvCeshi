cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# ---------------proj CvStitching:
# project(CvStitching)

include_directories(${OpenCV_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR} ${Boost_LIBRARY_DIRS})
file(GLOB SRC_OPENCV_CALIB cvStitching.cpp)
add_executable(CvStitching ${SRC_OPENCV_CALIB})
target_link_libraries(CvStitching ${OpenCV_LIBS} ${Boost_LIBRARIES})

