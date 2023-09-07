# Codebase3D  
A simple game engine codebase from scratch

## Convention  
* right-handed coordinates
* radian angle

## Known Issue  
Memory Leak https://github.com/glfw/glfw/issues/2093

## TODO
* Migrate to glm
* DirtyFlag propagation when adopt/abandon
* Use shader instead fixed function pipeline (https://learnopengl.com/Lighting/Basic-Lighting)
* GPU skinning(https://community.khronos.org/t/skinning-on-the-gpu-vs-the-cpu/73169/8)
* Camera z axis rotation issue (adjust quaternion roll axis to zero)
* ECS https://en.wikipedia.org/wiki/Entity_component_system
* Textbox, show FPS
* Improve performance