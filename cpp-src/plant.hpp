#pragma once

#include "global.hpp"
#include <godot_cpp/classes/static_body2d.hpp>

namespace godot {

class Plant : public StaticBody2D {
    // NOLINTBEGIN
    GDCLASS(Plant, StaticBody2D)
    friend class Node;
    // NOLINTEND

public:
    Plant();
    ~Plant();

    void setPlantTypes(uint32_t types);
    uint32_t getPlantTypes() const;
    bool checkType(GlobalType type) const;
    void setPlantType(GlobalType type, bool enable = true);

    void setPlantHealth(int health);
    int getPlantHealth() const;

    void setMaxHealth(int health);
    int getMaxHealth() const;

protected:
    static void _bind_methods();
    void _enter_tree() override;
    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double pDelta) override;

private:
    uint32_t mPlantType;
    uint32_t mPlantHealth;
    uint32_t mMaxHealth;
};
} // namespace godot
