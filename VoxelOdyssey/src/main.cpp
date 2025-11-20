#include "core/Window.h"
#include "core/Camera.h"
#include "core/Input.h"
#include "core/Frustum.h"
#include "core/Settings.h"
#include "renderer/Shader.h"
#include "renderer/DebugRenderer.h"
#include "renderer/SimpleHUD.h"
#include "renderer/Skybox.h"
#include "world/World.h"
#include "world/Raycast.h"
#include "world/Block.h"
#include "world/Inventory.h"
#include "world/PlayerStats.h"
#include "ui/InventoryUI.h"
#include "ui/PauseMenu.h"
#include "ui/MainMenu.h"
#include "world/WorldManager.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

// Movement direction constants
const int FORWARD = 0;
const int BACKWARD = 1;
const int LEFT = 2;
const int RIGHT = 3;
const int UP = 4;
const int DOWN = 5;

int main() {
    // Window setup
    Window window(1280, 720, "Voxel Odyssey");
    
    // Initialize input system
    Input::initialize(window.getHandle());

    // Texture load (once)
    unsigned int texture = 0;
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);  // GL convention
    unsigned char* data = stbi_load("assets/textures/block_atlas.png", &width, &height, &channels, 0);
    if (data) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }

    World world;
    Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    Shader debugShader("assets/shaders/debug.glsl", "assets/shaders/debug_fragment.glsl");
    Shader skyboxShader("assets/shaders/skybox_vertex.glsl", "assets/shaders/skybox_fragment.glsl");
    
    // Initialize debug renderer, HUD, skybox, and inventory UI
    DebugRenderer::init();
    SimpleHUD::init();
    InventoryUI::init();
    PauseMenu::init();
    MainMenu::init();
    Skybox skybox;
    skybox.init();
    
    // Settings system
    Settings settings;
    settings.loadFromFile(); // Load saved settings
    
    // World manager
    WorldManager& worldManager = WorldManager::getInstance();
    
    // Game state
    bool gameStarted = false;
    bool inventoryOpen = false;
    
    // Game objects (initialized when world is loaded)
    World* world = nullptr;
    Inventory* inventory = nullptr;
    PlayerStats* playerStats = nullptr;
    Camera* camera = nullptr;
    
    // Set clear color
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);  // Sky blue
    
    // Projection matrix
    glm::vec2 windowSize = window.getSize();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getZoom()), 
                                     windowSize.x / windowSize.y, 0.1f, 1000.0f);

    // Timing
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    while (!window.shouldClose()) {
        // Calculate delta time
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Update main menu
        MainMenu::update(deltaTime);
        
        // Handle main menu navigation
        if (MainMenu::isOpen()) {
            // Check for menu actions
            if (MainMenu::getState() == MainMenuState::NEW_WORLD && MainMenu::isCreatingWorld()) {
                std::string worldName = MainMenu::getNewWorldName();
                if (!worldName.empty()) {
                    std::string worldPath;
                    if (worldManager.createNewWorld(worldName, worldPath)) {
                        // Initialize game with new world
                        world = new World();
                        world->setWorldName(worldManager.getCurrentWorld());
                        worldManager.setCurrentWorld(worldManager.getCurrentWorld());
                        
                        inventory = new Inventory();
                        playerStats = new PlayerStats();
                        camera = new Camera(glm::vec3(0.0f, 50.0f, 0.0f));
                        settings.applySettings(*camera, *world);
                        
                        gameStarted = true;
                        MainMenu::setOpen(false);
                        MainMenu::setCreatingWorld(false);
                        Input::setMouseLocked(true);
                    }
                }
            }
            else if (MainMenu::getState() == MainMenuState::SELECT_WORLD) {
                auto worlds = worldManager.getAvailableWorlds();
                if (!worlds.empty() && Input::isKeyJustPressed(GLFW_KEY_ENTER)) {
                    int selected = MainMenu::getSelectedWorldIndex();
                    if (selected >= 0 && selected < (int)worlds.size()) {
                        // Load selected world
                        worldManager.setCurrentWorld(worlds[selected].folderName);
                        
                        world = new World();
                        world->setWorldName(worlds[selected].folderName);
                        
                        inventory = new Inventory();
                        playerStats = new PlayerStats();
                        camera = new Camera(worlds[selected].spawnPosition);
                        settings.applySettings(*camera, *world);
                        
                        gameStarted = true;
                        MainMenu::setOpen(false);
                        Input::setMouseLocked(true);
                    }
                }
            }
        }
        
        // Update world manager (auto-save)
        if (gameStarted && world) {
            static float saveTimer = 0.0f;
            saveTimer += deltaTime;
            
            // Auto-save every 1 minute
            if (saveTimer >= 60.0f && worldManager.isAutoSaveEnabled() && worldManager.getCurrentWorld() != "") {
                world->saveAllChunks();
                saveTimer = 0.0f;
            }
        }
        
        // Update pause menu (only if game is started)
        if (gameStarted) {
            PauseMenu::update(settings, deltaTime);
            
            // Toggle pause menu
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::TOGGLE_PAUSE))) {
                if (!PauseMenu::isOpen()) {
                    PauseMenu::setOpen(true);
                    Input::setMouseLocked(false);
                } else if (PauseMenu::getState() == PauseMenuState::MAIN) {
                    PauseMenu::setOpen(false);
                    if (!inventoryOpen) {
                        Input::setMouseLocked(true);
                    }
                } else {
                    // Go back to main menu
                    PauseMenu::setState(PauseMenuState::MAIN);
                }
            }
        }
        
        // Only process game input if game is started and not paused
        if (gameStarted && !PauseMenu::isOpen() && world && camera && inventory) {
            // Process input using settings keybinds
            // Movement
            if (Input::isKeyPressed(settings.getKeybind(KeybindAction::MOVE_FORWARD)))
                camera->processKeyboard(FORWARD, deltaTime);
            if (Input::isKeyPressed(settings.getKeybind(KeybindAction::MOVE_BACKWARD)))
                camera->processKeyboard(BACKWARD, deltaTime);
            if (Input::isKeyPressed(settings.getKeybind(KeybindAction::MOVE_LEFT)))
                camera->processKeyboard(LEFT, deltaTime);
            if (Input::isKeyPressed(settings.getKeybind(KeybindAction::MOVE_RIGHT)))
                camera->processKeyboard(RIGHT, deltaTime);
            if (Input::isKeyPressed(settings.getKeybind(KeybindAction::MOVE_UP)))
                camera->processKeyboard(UP, deltaTime);
            if (Input::isKeyPressed(settings.getKeybind(KeybindAction::MOVE_DOWN)))
                camera->processKeyboard(DOWN, deltaTime);
            
            // Mouse look
            if (Input::isMouseLocked()) {
                glm::vec2 mouseDelta = Input::getMouseDelta();
                if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
                    camera->processMouseMovement(mouseDelta.x, mouseDelta.y);
                }
            }
            
            // Toggle mouse lock
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::TOGGLE_MOUSE_LOCK))) {
                Input::setMouseLocked(!Input::isMouseLocked());
            }
            
            // Hotbar selection with number keys (1-9)
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::HOTBAR_1))) inventory->setSelectedHotbarSlot(0);
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::HOTBAR_2))) inventory->setSelectedHotbarSlot(1);
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::HOTBAR_3))) inventory->setSelectedHotbarSlot(2);
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::HOTBAR_4))) inventory->setSelectedHotbarSlot(3);
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::HOTBAR_5))) inventory->setSelectedHotbarSlot(4);
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::HOTBAR_6))) inventory->setSelectedHotbarSlot(5);
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::HOTBAR_7))) inventory->setSelectedHotbarSlot(6);
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::HOTBAR_8))) inventory->setSelectedHotbarSlot(7);
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::HOTBAR_9))) inventory->setSelectedHotbarSlot(8);
            
            // Toggle inventory
            if (Input::isKeyJustPressed(settings.getKeybind(KeybindAction::TOGGLE_INVENTORY))) {
                inventoryOpen = !inventoryOpen;
                Input::setMouseLocked(!inventoryOpen);
            }
            
            // Block interaction (only when mouse is locked)
            RaycastResult hit;
            if (Input::isMouseLocked()) {
                // Cast ray from camera
                glm::vec3 camPos = camera->getPosition();
                glm::vec3 camFront = camera->getFront();
                
                auto blockQuery = [world](int x, int y, int z) {
                    return world->getBlock(x, y, z);
                };
                
                hit = Raycast::cast(camPos, camFront, 10.0f, blockQuery);
                
                // Left click to destroy block
                if (Input::isMouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT) && hit.hit) {
                    BlockType destroyedBlock = world->getBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z);
                    world->setBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z, BlockType::AIR);
                    
                    // Add destroyed block to inventory
                    if (destroyedBlock != BlockType::AIR) {
                        inventory->addBlock(destroyedBlock, 1);
                    }
                }
                
                // Right click to place block
                if (Input::isMouseButtonJustPressed(GLFW_MOUSE_BUTTON_RIGHT) && hit.hit) {
                    BlockType selectedBlock = inventory->getSelectedBlock();
                    
                    if (selectedBlock != BlockType::AIR && inventory->hasBlock(selectedBlock, 1)) {
                        glm::ivec3 placePos = Raycast::getAdjacentBlock(hit.blockPos, hit.faceNormal);
                        // Only place if the target position is air
                        if (world->getBlock(placePos.x, placePos.y, placePos.z) == BlockType::AIR) {
                            world->setBlock(placePos.x, placePos.y, placePos.z, selectedBlock);
                            inventory->removeBlock(selectedBlock, 1);
                        }
                    }
                }
            }
        }
        
        // Update input system
        Input::update();
        
        // Update player stats
        if (gameStarted && playerStats) {
            playerStats->update(deltaTime);
        }
        
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render main menu if open
        if (MainMenu::isOpen()) {
            glm::vec2 windowSize = window.getSize();
            MainMenu::render(debugShader, (int)windowSize.x, (int)windowSize.y);
        }
        // Render game if started
        else if (gameStarted && world && camera && playerStats && inventory) {
            // Update world
            glm::vec3 camPos = camera->getPosition();
            world->update(camPos);
            
            // Render skybox first (before everything else)
            glm::mat4 view = camera->getViewMatrix();
            skybox.render(skyboxShader, view, proj);

            // Update projection if window size changed
            windowSize = window.getSize();
            proj = glm::perspective(glm::radians(camera->getZoom()), 
                                   windowSize.x / windowSize.y, 0.1f, 1000.0f);

            // Calculate frustum for culling
            glm::mat4 mvp = proj * view;
            Frustum frustum;
            frustum.extractFromMatrix(mvp);

            // Render world
            shader.use();
            shader.setMat4("view", view);
            shader.setMat4("projection", proj);
            shader.setVec3("viewPos", camPos);
            shader.setVec3("lightPos", camPos + glm::vec3(10,10,10));  // Sun-ish
            shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

            world->render(shader, texture, frustum);
            
            // Render debug outline for selected block (need to get hit from earlier)
            static RaycastResult lastHit;
            if (Input::isMouseLocked() && !PauseMenu::isOpen()) {
                glm::vec3 camPos2 = camera->getPosition();
                glm::vec3 camFront = camera->getFront();
                auto blockQuery = [world](int x, int y, int z) { return world->getBlock(x, y, z); };
                lastHit = Raycast::cast(camPos2, camFront, 10.0f, blockQuery);
                
                if (lastHit.hit) {
                    DebugRenderer::renderBlockOutline(debugShader, lastHit.blockPos, 
                                                     camera->getViewMatrix(), proj);
                }
            }
            
            // Render HUD (crosshair, hotbar, and stats)
            if (Input::isMouseLocked() && !inventoryOpen && !PauseMenu::isOpen()) {
                glm::vec2 windowSize2 = window.getSize();
                SimpleHUD::renderCrosshair(debugShader, (int)windowSize2.x, (int)windowSize2.y);
                
                // Render health and hunger bars
                SimpleHUD::renderHealthBar(debugShader, playerStats->getHealth(), (int)windowSize2.x, (int)windowSize2.y);
                SimpleHUD::renderHungerBar(debugShader, playerStats->getHunger(), (int)windowSize2.x, (int)windowSize2.y);
                
                // Render hotbar
                InventoryUI::renderHotbar(debugShader, *inventory, (int)windowSize2.x, (int)windowSize2.y);
            }
            
            // Render full inventory if open
            if (inventoryOpen) {
                glm::vec2 windowSize2 = window.getSize();
                InventoryUI::renderInventory(debugShader, *inventory, (int)windowSize2.x, (int)windowSize2.y, true);
            }
            
            // Render pause menu
            if (PauseMenu::isOpen()) {
                glm::vec2 windowSize2 = window.getSize();
                PauseMenu::render(debugShader, settings, (int)windowSize2.x, (int)windowSize2.y);
            }
        }

        window.swapBuffers();
        window.pollEvents();
    }

    // Save world before exit
    if (gameStarted && world) {
        world->saveAllChunks();
    }
    
    // Save settings before exit
    settings.saveToFile();
    
    // Cleanup game objects
    if (world) delete world;
    if (inventory) delete inventory;
    if (playerStats) delete playerStats;
    if (camera) delete camera;
    
    // Cleanup UI
    MainMenu::cleanup();
    PauseMenu::cleanup();
    SimpleHUD::cleanup();
    DebugRenderer::cleanup();
    glDeleteTextures(1, &texture);
    return 0;
}