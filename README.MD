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

1. `ecs.h`: Header file containing the core ECS classes and interfaces.
2. `ecs.cpp`: Implementation of the ECS framework.
3. `main.cpp`: Example usage of the ECS framework.

## Dependencies

- [nlohmann/json](https://github.com/nlohmann/json): A modern C++ JSON library.

## Usage

Here's a basic example of how to use the ECS framework:

```cpp
#include "ecs.h"

// Define your components
struct PositionComponent : ECS::Component {
    float x, y;
    // ... (implementation details)
};

struct VelocityComponent : ECS::Component {
    float dx, dy;
    // ... (implementation details)
};

// Define your systems
class MovementSystem : public ECS::System {
    void update(std::vector<std::shared_ptr<ECS::Entity>>& entities) override {
        // ... (implementation details)
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
