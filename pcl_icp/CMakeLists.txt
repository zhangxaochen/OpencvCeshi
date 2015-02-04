find_package(PCL REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable (pcl_icp pcl_icp.cpp)
target_link_libraries (pcl_icp ${PCL_LIBRARIES})
