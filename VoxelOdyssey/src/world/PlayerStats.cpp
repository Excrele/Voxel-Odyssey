#include "PlayerStats.h"

PlayerStats::PlayerStats() 
    : m_health(MAX_HEALTH), m_hunger(MAX_HUNGER), m_hungerTimer(0.0f) {
}

void PlayerStats::setHealth(float health) {
    m_health = health;
    if (m_health > MAX_HEALTH) m_health = MAX_HEALTH;
    if (m_health < 0.0f) m_health = 0.0f;
}

void PlayerStats::takeDamage(float damage) {
    m_health -= damage;
    if (m_health < 0.0f) m_health = 0.0f;
}

void PlayerStats::heal(float amount) {
    m_health += amount;
    if (m_health > MAX_HEALTH) m_health = MAX_HEALTH;
}

void PlayerStats::setHunger(float hunger) {
    m_hunger = hunger;
    if (m_hunger > MAX_HUNGER) m_hunger = MAX_HUNGER;
    if (m_hunger < 0.0f) m_hunger = 0.0f;
}

void PlayerStats::consumeHunger(float amount) {
    m_hunger -= amount;
    if (m_hunger < 0.0f) m_hunger = 0.0f;
}

void PlayerStats::restoreHunger(float amount) {
    m_hunger += amount;
    if (m_hunger > MAX_HUNGER) m_hunger = MAX_HUNGER;
}

void PlayerStats::update(float deltaTime) {
    m_hungerTimer += deltaTime;
    
    // Deplete hunger over time (every second)
    if (m_hungerTimer >= 1.0f) {
        consumeHunger(HUNGER_DEPLETION_RATE);
        m_hungerTimer = 0.0f;
    }
    
    // Health regeneration when well-fed
    if (m_hunger > 80.0f && m_health < MAX_HEALTH) {
        heal(HEALTH_REGEN_RATE * deltaTime);
    }
    
    // Take damage when starving
    if (m_hunger <= 0.0f && m_health > 0.0f) {
        takeDamage(HEALTH_DAMAGE_RATE * deltaTime);
    }
}

