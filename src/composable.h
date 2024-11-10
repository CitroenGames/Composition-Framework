#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include <Helper.h>

namespace Composable {

using json = nlohmann::json;

// Forward declarations
class Transform;
class Node;
class Component;
class Scene;

// Base component class that can be attached to nodes
class Component {
public:
    virtual ~Component() = default;
    virtual void OnAttach() {}      // Called when component is attached to a node
    virtual void OnDetach() {}      // Called when component is detached from a node
    virtual void Update(double dt) {} // Optional update method
    
    virtual json Serialize() const = 0;
    virtual void Deserialize(const json& j) = 0;
    virtual std::string GetTypeName() const = 0;
    
    void SetOwner(std::weak_ptr<Node> owner) { this->owner = owner; }
    std::weak_ptr<Node> GetOwner() const { return owner; }

protected:
    std::weak_ptr<Node> owner;
};

// Main compositable node class
class Node : public std::enable_shared_from_this<Node> {
public:
    using NodePtr = std::shared_ptr<Node>;
    using WeakNodePtr = std::weak_ptr<Node>;
    using ComponentPtr = std::shared_ptr<Component>;

private:
    std::string name;
    WeakNodePtr parent;
    std::vector<NodePtr> children;
    std::unordered_map<std::type_index, ComponentPtr> components;
    ComponentPtr transform;  // Quick access to transform component
    bool active = true;

public:
    explicit Node(const std::string& name = "Node");
    virtual ~Node() = default;

    // Hierarchy management
    void SetParent(NodePtr parent);
    void RemoveParent();
    NodePtr GetParent() const;
    const std::vector<NodePtr>& GetChildren() const;
    void AddChild(NodePtr child);
    void RemoveChild(NodePtr child);
    
    // Component management
    template<typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args) {
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        component->SetOwner(weak_from_this());
        components[std::type_index(typeid(T))] = component;
        
        // Special handling for Transform component
        if constexpr (std::is_same_v<T, Transform>) {
            transform = component;
        }
        
        component->OnAttach();
        return component;
    }

    template<typename T>
    void RemoveComponent() {
        auto typeIndex = std::type_index(typeid(T));
        auto it = components.find(typeIndex);
        if (it != components.end()) {
            it->second->OnDetach();
            if constexpr (std::is_same_v<T, Transform>) {
                transform.reset();
            }
            components.erase(typeIndex);
        }
    }

    template<typename T>
    bool HasComponent() const {
        return components.find(std::type_index(typeid(T))) != components.end();
    }

    template<typename T>
    std::shared_ptr<T> GetComponent() const {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    // Node properties
    inline const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }
    inline bool IsActive() const { return active; }
    void SetActive(bool value);
    inline std::unordered_map<std::type_index, ComponentPtr> GetComponents() const { return components; }
    
    // Transform helper
    std::shared_ptr<Transform> GetTransform() const { return std::static_pointer_cast<Transform>(transform); }

    // Serialization
    json Serialize() const;
    void Deserialize(const json& j);
};

// Scene management class
class Scene {
public:
    using NodePtr = std::shared_ptr<Node>;

    Scene();
    virtual ~Scene() = default;

    // Node management
    NodePtr CreateNode(const std::string& name = "Node");
    NodePtr CreateNode(NodePtr parent, const std::string& name = "Node");
    void RemoveNode(NodePtr node);
    
    // Scene traversal
    void ForEachNode(const std::function<void(NodePtr)>& fn);
    NodePtr FindNodeByName(const std::string& name);
    
    // Scene lifecycle
    virtual void OnLoad() {}
    virtual void OnUnload() {}
    void Update(double deltaTime);
    
    // Serialization
    json Serialize() const;
    void Deserialize(const json& j);

	const std::vector<NodePtr>& GetRootNodes() const { return rootNodes; }

private:
    std::vector<NodePtr> rootNodes;
    void CollectAllNodes(std::vector<NodePtr>& result) const;
};

} // namespace Composable