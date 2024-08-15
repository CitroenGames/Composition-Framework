# ECS (Entity Component System) Framework

This repository contains a simple implementation of an Entity Component System (ECS) framework in C++. The ECS architecture is commonly used in game development and simulations to create flexible and efficient systems for managing game objects and their behaviors.

## Features

- Entity management
- Component-based architecture
- System updates
- Serialization and deserialization of world state
- Tickable components for time-based updates

## Project Structure

The project consists of three main files:

1. [`src/ecs.h`](https://github.com/CitroenGames/ECS/blob/master/src/ECS.h): Header file containing the core ECS classes and interfaces.
2. [`src/ecs.cpp`](https://github.com/CitroenGames/ECS/blob/master/src/ECS.cpp): Implementation of the ECS framework.
3. [`example/main.cpp`](https://github.com/CitroenGames/ECS/blob/master/example/main.cpp): Example usage of the ECS framework.

## Dependencies

- [nlohmann/json](https://github.com/nlohmann/json): A modern C++ JSON library.

## Usage

Here's a basic example of how to use the ECS framework:

```cpp
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

int main() {
    ECS::World world;

    // Create entities and add components
    auto entity = world.CreateEntity();
    entity->AddComponent<PositionComponent>(0.0f, 0.0f);
    entity->AddComponent<VelocityComponent>(1.0f, 1.0f);

    // Add systems to the world
    world.AddSystem(std::make_unique<MovementSystem>());

    // Update the world
    world.Update(0.16f);

    // Serialize and deserialize the world state
    ECS::json serialized = world.Serialize();
    // ... (save to file or transmit)
    world.Deserialize(serialized);

    return 0;
}
```

## Building the Project

To build the project, you'll need a C++ compiler that supports C++14 or later. Make sure you have the nlohmann/json library installed or included in your project.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is open-source and available under the [MIT License](LICENSE).
