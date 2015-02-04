#cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#project(statistical_removal)

#find_package(PCL 1.2 REQUIRED)
find_package(PCL REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable (pcl_statistical_removal pcl_statistical_removal.cpp pcl_statistical_removal_kb.cpp)
target_link_libraries (pcl_statistical_removal ${PCL_LIBRARIES})
