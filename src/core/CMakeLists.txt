add_library(core "Transform.cpp" "Camera.h" "Transform.h" "Utility.h" "Engine.h" "Engine.cpp" "Stats.h" "Stats.cpp" "Input.h" "Input.cpp")
add_subdirectory(cvars)
add_subdirectory(spatial)
target_link_libraries(core cvars spatial)