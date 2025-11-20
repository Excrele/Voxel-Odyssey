#pragma once
#include "Block.h"
#include <unordered_map>
#include <array>

class Inventory {
public:
    Inventory();
    
    // Hotbar operations (0-8 slots)
    void setHotbarSlot(int slot, BlockType type);
    BlockType getHotbarSlot(int slot) const;
    int getSelectedHotbarSlot() const { return m_selectedHotbarSlot; }
    void setSelectedHotbarSlot(int slot);
    BlockType getSelectedBlock() const;
    
    // Inventory operations
    void addBlock(BlockType type, int quantity = 1);
    void removeBlock(BlockType type, int quantity = 1);
    int getBlockCount(BlockType type) const;
    bool hasBlock(BlockType type, int quantity = 1) const;
    
    // Get all blocks in inventory (for UI display)
    const std::unordered_map<BlockType, int>& getAllBlocks() const { return m_blocks; }
    
    // Initialize with default blocks for testing
    void initializeDefault();
    
private:
    static constexpr int HOTBAR_SIZE = 9;
    std::array<BlockType, HOTBAR_SIZE> m_hotbar;
    std::unordered_map<BlockType, int> m_blocks; // Block type -> quantity
    int m_selectedHotbarSlot; // 0-8
};

