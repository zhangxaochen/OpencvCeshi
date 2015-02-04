cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# ---------------proj CvImg2Video:
# project(CvImg2Video)

include_directories(${OpenCV_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR} ${Boost_LIBRARY_DIRS})
file(GLOB SRC_OPENCV_CALIB cvImg2Video.cpp)
add_executable(CvImg2Video ${SRC_OPENCV_CALIB})
target_link_libraries(CvImg2Video ${OpenCV_LIBS} ${Boost_LIBRARIES})

