find_package(PCL REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable (pcl_iterative_closest_point pcl_iterative_closest_point.cpp)
target_link_libraries (pcl_iterative_closest_point ${PCL_LIBRARIES})
