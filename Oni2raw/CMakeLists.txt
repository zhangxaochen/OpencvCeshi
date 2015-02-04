# project(Oni2raw)

include_directories($ENV{OPEN_NI_INCLUDE} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_directories($ENV{OPEN_NI_LIB} ${OpenCV_LIB_DIR} ${Boost_LIBRARY_DIRS})
file(GLOB SRC_ONI_TO_RAW oni2raw.cpp)
add_executable(oni2raw ${SRC_ONI_TO_RAW})
target_link_libraries(oni2raw ${OPENNI_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES})



