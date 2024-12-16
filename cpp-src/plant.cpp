#include "plant.hpp"

#include <algorithm>

#include "global.hpp"
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/shape2d.hpp>

namespace godot {
Plant::Plant() {
    mMaxHealth   = 10;
    mPlantHealth = 10;
    mPlantType   = GlobalType::NormalHPlant | GlobalType::NormalGPlant | GlobalType::Soil;
}

Plant::~Plant() {}

void Plant::_bind_methods() {
    ClassDB::bind_method(D_METHOD("setPlantTypes", "types"), &Plant::setPlantTypes);
    ClassDB::bind_method(D_METHOD("getPlantTypes"), &Plant::getPlantTypes);
    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "plantType", PROPERTY_HINT_FLAGS, make_property_hint_flags<GlobalType>().c_str()),
        "setPlantTypes", "getPlantTypes");

    ClassDB::bind_method(D_METHOD("setPlantHealth", "health"), &Plant::setPlantHealth);
    ClassDB::bind_method(D_METHOD("getPlantHealth"), &Plant::getPlantHealth);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "plantHealth", PROPERTY_HINT_RANGE,
                              "0," + String::num_uint64(std::numeric_limits<uint32_t>::max()) + ",1"),
                 "setPlantHealth", "getPlantHealth");

    ClassDB::bind_method(D_METHOD("setMaxHealth", "health"), &Plant::setMaxHealth);
    ClassDB::bind_method(D_METHOD("getMaxHealth"), &Plant::getMaxHealth);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "maxHealth", PROPERTY_HINT_RANGE,
                              "0," + String::num_uint64(std::numeric_limits<uint32_t>::max()) + ",1"),
                 "setMaxHealth", "getMaxHealth");
}

void Plant::setPlantTypes(uint32_t types) { mPlantType = types; }

uint32_t Plant::getPlantTypes() const { return mPlantType; }

bool Plant::checkType(GlobalType type) const { return (type & mPlantType) != 0; }

void Plant::setPlantType(GlobalType type, bool enable) {
    if (enable) {
        mPlantType = mPlantType | type;
    } else {
        mPlantType = mPlantType & ~type;
    }
}

void Plant::setPlantHealth(int health) {
    health       = std::max(health, 0);
    health       = std::min(static_cast<uint32_t>(health), mMaxHealth);
    mPlantHealth = health;
}

int Plant::getPlantHealth() const { return mPlantHealth; }

void Plant::setMaxHealth(int health) { mMaxHealth = health; }

int Plant::getMaxHealth() const { return mMaxHealth; }

void Plant::_process(double delta) {
    mPlantHealth = std::max<uint32_t>(mPlantHealth, 0);

    if (mPlantHealth == 0) {
        emit_signal("dead", this);
    }
}

void Plant::_enter_tree() {}

void Plant::_ready() {
    // AnimatedSprite2D* sprite = memnew(AnimatedSprite2D);
    // sprite->set_name("PlantAnimated");
    // add_child(sprite);
    // sprite->set_owner(this);

    // CollisionShape2D* shape = memnew(CollisionShape2D);
    // shape->set_name("PlantCollision");
    // add_child(shape);
    // shape->set_owner(this);
}

void Plant::_physics_process(double pDelta) {}
} // namespace godot