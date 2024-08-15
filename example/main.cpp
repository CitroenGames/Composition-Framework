#include "ecs.h"
#include <iostream>
#include <fstream>

struct PositionComponent : ECS::Component {
    float x, y;
    PositionComponent(float x = 0, float y = 0) : x(x), y(y) {}

    ECS::json Serialize() const override {
        return {{"x", x}, {"y", y}};
    }

    void Deserialize(const ECS::json& j) override {
        x = j["x"];
        y = j["y"];
    }

    std::string GetTypeName() const override {
        return "PositionComponent";
    }
};

struct VelocityComponent : ECS::Component {
    float dx, dy;
    VelocityComponent(float dx = 0, float dy = 0) : dx(dx), dy(dy) {}

    ECS::json Serialize() const override {
        return {{"dx", dx}, {"dy", dy}};
    }

    void Deserialize(const ECS::json& j) override {
        dx = j["dx"];
        dy = j["dy"];
    }

    std::string GetTypeName() const override {
        return "VelocityComponent";
    }
};

class MovementSystem : public ECS::System {
public:
    void update(std::vector<std::shared_ptr<ECS::Entity>>& entities) override {
        for (auto& entity : entities) {
            if (entity->HasComponent<PositionComponent>() && entity->HasComponent<VelocityComponent>()) {
                auto position = entity->GetComponent<PositionComponent>();
                auto velocity = entity->GetComponent<VelocityComponent>();
                position->x += velocity->dx;
                position->y += velocity->dy;
                std::cout << "Entity " << entity->GetId() << " moved to (" 
                          << position->x << ", " << position->y << ")" << std::endl;
            }
        }
    }
};

// Factory function to create components based on type name
std::shared_ptr<ECS::Component> createComponentFromTypeName(const std::string& typeName) {
    if (typeName == "PositionComponent") return std::make_shared<PositionComponent>();
    if (typeName == "VelocityComponent") return std::make_shared<VelocityComponent>();
    return nullptr;
}

int main() {
    ECS::World world;

    auto entity1 = world.CreateEntity();
    entity1->AddComponent<PositionComponent>(0.0f, 0.0f);
    entity1->AddComponent<VelocityComponent>(1.0f, 1.0f);

    auto entity2 = world.CreateEntity();
    entity2->AddComponent<PositionComponent>(5.0f, 5.0f);
    entity2->AddComponent<VelocityComponent>(-1.0f, 0.5f);

    world.AddSystem(std::make_unique<MovementSystem>());

    // Serialize the world
    ECS::json serialized = world.Serialize();
    std::ofstream out("world_state.json");
    out << serialized.dump(4);
    out.close();

    // Clear the world
    world = ECS::World();

    // Deserialize the world
    std::ifstream in("world_state.json");
    ECS::json deserialized;
    in >> deserialized;
    in.close();
    world.Deserialize(deserialized);

    // Run the simulation
    for (int i = 0; i < 5; ++i) {
        std::cout << "Update " << i + 1 << std::endl;
        // emulating time passing
        world.Update(0.16f);
        std::cout << std::endl;
    }

    return 0;
}