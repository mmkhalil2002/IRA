cmake_minimum_required(VERSION 2.8)
project( video_client )
find_package( OpenCV REQUIRED )
add_executable( video_client video_client.cpp )
target_link_libraries( video_client ${OpenCV_LIBS} pthread )
