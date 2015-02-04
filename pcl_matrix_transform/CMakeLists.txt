find_package(PCL REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable (pcl_matrix_transform pcl_matrix_transform.cpp)
target_link_libraries (pcl_matrix_transform ${PCL_LIBRARIES})
