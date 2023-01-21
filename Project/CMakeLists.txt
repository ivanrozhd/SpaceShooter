set(CURRENT_ASSIGNMENT a08)
add_subdirectory(shaders)

add_executable(game_${CURRENT_ASSIGNMENT} game.cpp configuration/config.hpp configuration/config.cpp configuration/config.cfg)
add_dependencies(game_${CURRENT_ASSIGNMENT} shaders_${CURRENT_ASSIGNMENT})
target_link_libraries(game_${CURRENT_ASSIGNMENT} PUBLIC ecg_framework)

if(WIN32)
    # this allows to find the shaders and other assets via relative paths
    set_property(TARGET game_${CURRENT_ASSIGNMENT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif(WIN32)