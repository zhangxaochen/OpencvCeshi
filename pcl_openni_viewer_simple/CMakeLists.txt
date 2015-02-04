#cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#project(concatenate_clouds)

#find_package(PCL 1.2 REQUIRED)
find_package(PCL REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable (pcl_openni_viewer_simple pcl_openni_viewer_simple.cpp)
target_link_libraries (pcl_openni_viewer_simple ${PCL_LIBRARIES})

