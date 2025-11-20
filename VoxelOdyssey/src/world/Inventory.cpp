#include "Inventory.h"

Inventory::Inventory() : m_selectedHotbarSlot(0) {
    // Initialize hotbar with AIR
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        m_hotbar[i] = BlockType::AIR;
    }
    
    // Initialize with some default blocks for testing
    initializeDefault();
}

void Inventory::initializeDefault() {
    // Give player some starting blocks
    addBlock(BlockType::GRASS, 64);
    addBlock(BlockType::DIRT, 64);
    addBlock(BlockType::STONE, 64);
    addBlock(BlockType::WOOD, 32);
    addBlock(BlockType::SAND, 32);
    
    // Set up hotbar
    setHotbarSlot(0, BlockType::GRASS);
    setHotbarSlot(1, BlockType::DIRT);
    setHotbarSlot(2, BlockType::STONE);
    setHotbarSlot(3, BlockType::SAND);
    setHotbarSlot(4, BlockType::WOOD);
    setHotbarSlot(5, BlockType::LEAVES);
}

void Inventory::setHotbarSlot(int slot, BlockType type) {
    if (slot >= 0 && slot < HOTBAR_SIZE) {
        m_hotbar[slot] = type;
    }
}

BlockType Inventory::getHotbarSlot(int slot) const {
    if (slot >= 0 && slot < HOTBAR_SIZE) {
        return m_hotbar[slot];
    }
    return BlockType::AIR;
}

void Inventory::setSelectedHotbarSlot(int slot) {
    if (slot >= 0 && slot < HOTBAR_SIZE) {
        m_selectedHotbarSlot = slot;
    }
}

BlockType Inventory::getSelectedBlock() const {
    return getHotbarSlot(m_selectedHotbarSlot);
}

void Inventory::addBlock(BlockType type, int quantity) {
    if (type == BlockType::AIR) return; // Can't add air
    
    m_blocks[type] += quantity;
    
    // If this block is in hotbar and wasn't there before, update it
    for (int i = 0; i < HOTBAR_SIZE; i++) {
        if (m_hotbar[i] == BlockType::AIR) {
            m_hotbar[i] = type;
            break;
        }
    }
}

void Inventory::removeBlock(BlockType type, int quantity) {
    if (type == BlockType::AIR) return;
    
    auto it = m_blocks.find(type);
    if (it != m_blocks.end()) {
        it->second -= quantity;
        if (it->second <= 0) {
            m_blocks.erase(it);
            
            // Remove from hotbar if quantity is 0
            for (int i = 0; i < HOTBAR_SIZE; i++) {
                if (m_hotbar[i] == type) {
                    m_hotbar[i] = BlockType::AIR;
                }
            }
        }
    }
}

int Inventory::getBlockCount(BlockType type) const {
    auto it = m_blocks.find(type);
    if (it != m_blocks.end()) {
        return it->second;
    }
    return 0;
}

bool Inventory::hasBlock(BlockType type, int quantity) const {
    return getBlockCount(type) >= quantity;
}

