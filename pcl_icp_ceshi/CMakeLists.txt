find_package(PCL REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable (pcl_icp_ceshi pcl_icp_ceshi.cpp)
target_link_libraries (pcl_icp_ceshi ${PCL_LIBRARIES})
