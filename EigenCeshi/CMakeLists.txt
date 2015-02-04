#PclCeshi global cmake
#cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#---------------top level:
#project(PclCeshi)
#message(STATUS "--PCL_ROOT is: $ENV{PCL_ROOT}")

#---------------Eigen
#set(Eigen_DIR "$ENV{PCL_ROOT}/3rdParty/Eigen")
#set(EIGEN_INCLUDE_DIR "${Eigen_DIR}/include")
#message(STATUS "--Eigen_DIR is: ${Eigen_DIR}")
#find_package(Eigen REQUIRED)	#找不到， 而且 eigen 只有 .h

#---------------opencv
#find_package(OpenCV REQUIRED)
#if (OPENCV_FOUND)
#	add_definitions(-DHAVE_OPENCV)
#	message(STATUS "--DHAVE_OPENCV is: ${HAVE_OPENCV}")
#endif()


include_directories(${EIGEN_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})
file(GLOB SRC_PCL_CESHI *.cpp *.h)
add_executable(eigenCeshi ${SRC_PCL_CESHI})
target_link_libraries(eigenCeshi ${OpenCV_LIBS})


