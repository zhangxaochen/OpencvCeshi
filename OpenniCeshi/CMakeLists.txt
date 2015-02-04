cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# ---------------proj 1
# project(OpenniCeshi)
include_directories($ENV{OPEN_NI_INCLUDE} ${OpenCV_INCLUDE_DIRS})
link_directories($ENV{OPEN_NI_LIB} ${OpenCV_LIB_DIR})
file(GLOB SRC_OPENNI_CESHI openniCeshi.cpp)
add_executable(OpenniCeshi ${SRC_OPENNI_CESHI})
target_link_libraries(OpenniCeshi ${OPENNI_LIBRARIES} ${OpenCV_LIBS})

# ---------------proj 2
# project(Openni_load_raw_file)
# include_directories($ENV{OPEN_NI_INCLUDE})
# link_directories($ENV{OPEN_NI_LIB})
# file(GLOB SOURCES_LOAD_RAW openni_load_raw_file.cpp)
# add_executable(Openni_load_raw_file ${SOURCES_LOAD_RAW})
# target_link_libraries(Openni_load_raw_file ${OPENNI_LIBRARIES})



