cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#---------------openni 
set(OPENNI_LIBRARIES openNI.lib)

#---------------opencv
#http://docs.opencv.org/trunk/doc/tutorials/introduction/linux_gcc_cmake/linux_gcc_cmake.html
#set(OpenCV_DIR "D:\sdk\opencv249")
#set(OpenCV_LIB_DIR "D:/opencv249/build/x86/vc10/staticlib")
#set(OpenCV_SHARED OFF)
#set(OpenCV_STATIC ON) #因为同时有 opencv, boost, 所以不要这样

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV_LIB_DIR := ${OpenCV_LIB_DIR}")

#---------------Eigen
set(Eigen_DIR "$ENV{PCL_ROOT}/3rdParty/Eigen")
set(EIGEN_INCLUDE_DIR "${Eigen_DIR}/include")

#---------------boost
#set(BOOST_ROOT "D:/sdk/boost_1_56_0-vc10-x86")
#find_package(Boost 1.56.0 REQUIRED)
#message(STATUS "--BOOST_ROOT is: ${BOOST_ROOT}")
find_package(Boost REQUIRED)


# ---------------top level:
project(OpencvCeshi)
#set(CMAKE_CXX_FLAGS_RELEASE "/MT")
#set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

#set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1") 
message(STATUS "--CMAKE_CXX_FLAGS_DEBUG is: ${CMAKE_CXX_FLAGS_DEBUG}")

#set(CompilerFlags
#        CMAKE_CXX_FLAGS
#        CMAKE_CXX_FLAGS_DEBUG
#        CMAKE_CXX_FLAGS_RELEASE
#        CMAKE_C_FLAGS
#        CMAKE_C_FLAGS_DEBUG
#        CMAKE_C_FLAGS_RELEASE
#        )
#foreach(CompilerFlag ${CompilerFlags})
#  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
#endforeach()

add_subdirectory(OpencvCeshi)
add_subdirectory(OpencvCalib)
add_subdirectory(EigenCeshi)

add_subdirectory(OpenniCeshi)
add_subdirectory(Openni_load_oni_file)
add_subdirectory(Openni_load_raw_file)

add_subdirectory(ZcNiRecordSynthetic)
add_subdirectory(Oni2raw)

# ---------------pcl
add_subdirectory(PclCeshi)
add_subdirectory(pcl_matrix_transform)
add_subdirectory(pcl_passthrough)
add_subdirectory(pcl_voxel_grid)
add_subdirectory(pcl_concatenate_clouds)
add_subdirectory(pcl_openni_viewer_simple)
add_subdirectory(pcl_statistical_removal)
add_subdirectory(pcl_project_inliers)
add_subdirectory(pcl_icp)
add_subdirectory(pcl_iterative_closest_point)
add_subdirectory(pcl_icp_ceshi)


#---------------计算机视觉作业
add_subdirectory(CvImg2Video)
add_subdirectory(CvStitching)

message(STATUS "CMAKE_VERSION is: ${CMAKE_VERSION}")
message(STATUS "ENV{OPEN_NI_INCLUDE} is:$ENV{OPEN_NI_INCLUDE}")
message(STATUS "CMAKE_SOURCE_DIR := ${CMAKE_SOURCE_DIR}")

