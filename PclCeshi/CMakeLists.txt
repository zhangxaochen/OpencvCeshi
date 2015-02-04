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


#include_directories(${EIGEN_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})
#link_directories(${OpenCV_LIB_DIR})
#file(GLOB SRC_PCL_CESHI *.cpp *.h)
#add_executable(PclCeshi ${SRC_PCL_CESHI})
#target_link_libraries(PclCeshi ${OpenCV_LIBS})

find_package(PCL  REQUIRED COMPONENTS common io)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})
#add_executable(pcd_write_test pcd_write.cpp)
#target_link_libraries(pcd_write_test ${PCL_COMMON_LIBRARIES} ${PCL_IO_LIBRARIES})
file(GLOB SRC_PCL_CESHI *.cpp *.h)
add_executable(PclCeshi ${SRC_PCL_CESHI})
target_link_libraries(PclCeshi ${PCL_COMMON_LIBRARIES} ${PCL_IO_LIBRARIES})


message(STATUS "PCL_INCLUDE_DIRS: ${PCL_INCLUDE_DIRS}")
message(STATUS "PCL_LIBRARY_DIRS: ${PCL_LIBRARY_DIRS}")
message(STATUS "PCL_DEFINITIONS: ${PCL_DEFINITIONS}")
message(STATUS "PCL_COMMON_LIBRARIES: ${PCL_COMMON_LIBRARIES}")
message(STATUS "PCL_IO_LIBRARIES: ${PCL_IO_LIBRARIES}")

