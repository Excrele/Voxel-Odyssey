#pragma once

class PlayerStats {
public:
    PlayerStats();
    
    // Health (0-100)
    float getHealth() const { return m_health; }
    void setHealth(float health);
    void takeDamage(float damage);
    void heal(float amount);
    bool isAlive() const { return m_health > 0.0f; }
    
    // Hunger (0-100)
    float getHunger() const { return m_hunger; }
    void setHunger(float hunger);
    void consumeHunger(float amount);
    void restoreHunger(float amount);
    
    // Update stats over time
    void update(float deltaTime);
    
    // Constants
    static constexpr float MAX_HEALTH = 100.0f;
    static constexpr float MAX_HUNGER = 100.0f;
    static constexpr float HUNGER_DEPLETION_RATE = 0.5f; // Per second
    static constexpr float HEALTH_REGEN_RATE = 1.0f; // Per second when hunger > 80
    static constexpr float HEALTH_DAMAGE_RATE = 0.5f; // Per second when hunger = 0
    
private:
    float m_health;
    float m_hunger;
    float m_hungerTimer; // Track time for hunger depletion
};

