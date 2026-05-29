add_subdirectory(Component)
add_subdirectory(Interfaces)
add_subdirectory(Layer)
add_subdirectory(Object)
add_subdirectory(PollEvent)
add_subdirectory(Render)
add_subdirectory(Utils)

target_sources(Project_AK PRIVATE Engine.cpp)
