# Archura Engine - Release 1.0.20 Update Notes

This release brings significant improvements to the Level Editor workflow and performance.

### 🌟 New Features

*   **Advanced Model Spawner**: The `+ Entity` menu now recursively scans the `assets/models` folder. You can easily navigate your organized model libraries (e.g., `URBAN/Buildings/...`) through sub-menus. Supports `.fbx` and `.obj` files natively.
*   **Entity Renaming**: You can now rename any object directly from the Inspector panel. The Scene Hierarchy updates instantly to reflect the new name.
*   **Entity Deletion**: Added a "Delete Entity" button to the Inspector for quick object removal.
*   **GLB Support (Preview)**: Added UI recognition for `.glb` and `.gltf` files. Currently spawns a placeholder, paving the way for full support in future updates.

### ⚡ Optimizations

*   **Editor Performance**: Completely rewrote the model menu system to use nested directory rendering. This eliminates the FPS drop previously caused by listing thousands of asset files at once.

### 🔧 Fixes

*   Fixed an issue where model files in subdirectories were not visible in the entity menu.
