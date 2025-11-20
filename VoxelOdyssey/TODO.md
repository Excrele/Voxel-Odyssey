# Voxel Odyssey - TODO List

## Project Status: Core Systems Complete ✅

**Last Updated:** Current session  
**Status:** Fully playable voxel game with core features implemented

---

## ✅ Completed Features

### Core Systems
- [x] Fix compilation errors
- [x] Implement Mesh class (VAO/VBO management)
- [x] Implement Chunk class (mesh generation, block storage)
- [x] Implement World class (chunk management, loading/unloading)
- [x] Basic rendering pipeline

### Camera & Input
- [x] Camera System (FPS-style with mouse look and WASD movement)
- [x] Input Handling (keyboard and mouse input)
- [x] Delta time calculation for smooth movement
- [x] Mouse lock/unlock (ESC key)

### World Systems
- [x] Fix Chunk Border Face Culling Bug
  - Chunks now check neighbors across chunk boundaries
  - World provides cross-chunk block queries
  - Proper face culling on chunk borders
- [x] Block Placement/Destruction
  - Ray casting system implemented (DDA algorithm)
  - Left click to destroy blocks
  - Right click to place blocks
  - Visual feedback (yellow wireframe outline on selected block)

### Block System
- [x] Enhanced Block Types (DIRT, WOOD, LEAVES, SAND, GRAVEL)
- [x] Different Textures Per Face (grass top/sides/bottom, wood rings/bark)
- [x] Block Selection System (number keys 1-7 to choose block type)

### Terrain Generation
- [x] Enhanced Terrain Generation
  - Biomes (grassland, desert)
  - Caves using 3D noise
  - Trees with procedural generation
  - Better height variation with noise
  - Layer system (stone → dirt → grass/sand)

---

## 📋 Next Priority Features

### ✅ Recently Completed (This Session)
- [x] Frustum Culling (don't render off-screen chunks)
  - Implemented frustum extraction from view-projection matrix
  - AABB testing for chunks
  - Significant performance improvement for large render distances
- [x] Simple UI/HUD System
  - Crosshair in center of screen
  - Block selection indicator (colored box showing selected block)
  - Visual feedback for current block type
- [x] Skybox Rendering
  - Procedural gradient skybox (light blue to darker blue)
  - Cube map texture
  - Renders before world geometry

### Performance Optimizations
- [ ] Greedy Meshing (combine adjacent faces for better performance) - *Future optimization*
- [ ] Chunk mesh generation on background thread
- [x] Optimize Renderer::drawCube() (currently creates/destroys VAO per frame)
  - Created static cube VAO that's initialized once and reused
  - Fixed incomplete cube vertices array
  - Removed hardcoded view/projection matrices

### ✅ Recently Completed (This Session)
- [x] Inventory System
  - Full inventory with block quantities
  - 9-slot hotbar for quick access
  - Hotbar selection with number keys (1-9)
  - Block consumption when placing blocks
  - Block pickup when destroying blocks
  - Hotbar UI with visual selection indicator
  - Inventory UI (toggle with E key)

### Enhanced Features
- [x] More Biomes
  - [x] Snow biome (high altitude/cold regions with snow blocks)
  - [x] Forest biome (increased tree density, ~17% chance)
  - [x] Ocean biome (low-lying areas with sand)
  - Biome system uses noise-based generation for natural distribution
- [x] Ambient Occlusion
  - [x] AO calculation for vertex corners based on adjacent blocks
  - [x] AO values passed to shader and applied to lighting
  - [x] Corners and edges darken naturally where blocks meet
  - Improves visual depth and quality significantly
- [x] Better Cave Systems
  - [x] Improved cave generation with multi-octave noise for varied shapes
  - [x] Ore generation (Coal Ore, Iron Ore) in stone layers
  - [x] Stalactites and stalagmites in caves

### Gameplay Features
- [x] Save/Load system for chunks
  - Chunk serialization/deserialization implemented
  - Chunks saved to disk when unloaded
  - Chunks loaded from disk when available (falls back to generation)
  - File format: binary format in saves/worldName/chunks/ directory
- [x] World Management System
  - [x] WorldManager class for managing multiple worlds
  - [x] World metadata (name, creation date, last played, spawn position)
  - [x] Automatic save every 1 minute
  - [x] World creation with custom names
  - [x] World selection/loading system
- [x] Main Menu System
  - [x] Main menu on game startup
  - [x] Singleplayer menu
  - [x] New World creation screen with name input
  - [x] World selection screen showing all saved worlds
  - [x] Menu navigation (similar to Minecraft)
- [x] Inventory system ✅
- [ ] Physics system (gravity, collisions)
- [ ] Entity system (mobs, items)
- [ ] Crafting system
- [x] UI/HUD system ✅
  - [x] Block selection display (hotbar)
  - [x] Health/hunger bars
    - [x] PlayerStats class for health and hunger tracking
    - [x] Health bar with color coding (green/yellow/red)
    - [x] Hunger bar with color coding
    - [x] Hunger depletes over time
    - [x] Health regenerates when well-fed, damages when starving
  - [ ] Minimap

### Technical Improvements
- [x] Pause Menu with Settings
  - [x] Settings system with keybind configuration
  - [x] Visual quality settings (render distance, FOV, mouse sensitivity, movement speed)
  - [x] Pause menu UI with multiple submenus
  - [x] Settings save/load functionality
  - [x] Keybind remapping system
  - [x] Integration with game systems
- [ ] Better memory management for chunk unloading
- [ ] Handle edge cases in world coordinate conversion
- [ ] Multi-threading for chunk generation
- [ ] Sound system
- [ ] Multiplayer support

---

## 🎮 Current Controls

- **W/A/S/D**: Move forward/left/backward/right
- **Space**: Move up
- **Left Shift**: Move down
- **Mouse**: Look around (when locked)
- **ESC**: Toggle mouse lock/unlock
- **E**: Toggle inventory (opens/closes inventory, unlocks mouse)
- **Left Click**: Destroy block (adds to inventory)
- **Right Click**: Place block from hotbar (consumes from inventory)
- **1-9**: Select hotbar slot
  - Hotbar shows 9 slots at bottom of screen
  - Selected slot highlighted with yellow border
  - Only blocks in inventory can be placed

---

## 📊 Project Statistics

- **Block Types**: 11 (AIR, GRASS, DIRT, STONE, SAND, WOOD, LEAVES, GRAVEL, SNOW, COAL_ORE, IRON_ORE)
- **Chunk Size**: 16x64x16 blocks
- **Render Distance**: 4 chunks
- **Terrain Features**: 5 Biomes (Grassland, Desert, Snow, Forest, Ocean), caves, trees, height variation
- **Performance**: Naive meshing (greedy meshing deferred)
