add_library(heightmap_viewer HeightmapViewer.hpp HeightmapViewer.cpp CameraController.hpp CameraController.cpp shaders.hpp)
target_link_libraries(heightmap_viewer PUBLIC tga_vulkan tga_utils ${CMAKE_THREAD_LIBS_INIT})

