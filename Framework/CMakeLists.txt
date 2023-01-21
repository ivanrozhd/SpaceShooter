# Library ecg_framework

find_package(Threads)
add_library(ecg_framework shaderData.hpp framework.hpp cameraController.hpp cameraController.cpp)
target_link_libraries(ecg_framework PUBLIC tga_vulkan tga_utils ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(ecg_framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})