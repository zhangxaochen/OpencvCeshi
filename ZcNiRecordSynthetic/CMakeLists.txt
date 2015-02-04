# project(ZcNiRecordSynthetic)

include_directories($ENV{OPEN_NI_INCLUDE} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_directories($ENV{OPEN_NI_LIB} ${OpenCV_LIB_DIR} ${Boost_LIBRARY_DIRS})
file(GLOB SRC_LOAD_RAW ZcNiRecordSynthetic.cpp)
add_executable(ZcNiRecordSynthetic ${SRC_LOAD_RAW})
target_link_libraries(ZcNiRecordSynthetic ${OPENNI_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES})



