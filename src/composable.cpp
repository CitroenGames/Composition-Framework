#include "composable.h"
#include "transform.h"
#include <algorithm>
#include <queue>

namespace Composable {

// Node implementation
Node::Node(const std::string& name) : name(name) {
    // Add Transform component by default
    AddComponent<Transform>();
}

void Node::SetParent(NodePtr newParent) {
    if (auto oldParent = parent.lock()) {
        oldParent->RemoveChild(shared_from_this());
    }
    
    if (newParent) {
        parent = newParent;
        newParent->AddChild(shared_from_this());
        
        // Update transform when parent changes
        if (auto transform = GetTransform()) {
            transform->SetLocalPosition(transform->GetWorldPosition());
            transform->SetLocalRotation(transform->GetWorldRotation());
            transform->SetLocalScale(transform->GetWorldScale());
        }
    }
}

void Node::RemoveParent() {
    if (auto p = parent.lock()) {
        p->RemoveChild(shared_from_this());
        parent.reset();
    }
}

Node::NodePtr Node::GetParent() const {
    return parent.lock();
}

const std::vector<Node::NodePtr>& Node::GetChildren() const {
    return children;
}

void Node::AddChild(NodePtr child) {
    if (std::find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
    }
}

void Node::RemoveChild(NodePtr child) {
    children.erase(
        std::remove(children.begin(), children.end(), child),
        children.end()
    );
}

void Node::SetActive(bool value) {
    if (active != value) {
        active = value;
        // Propagate to children
        for (auto& child : children) {
            child->SetActive(value);
        }
    }
}

json Node::Serialize() const {
    json j;
    j["name"] = name;
    j["active"] = active;
    
    j["components"] = json::array();
    for (const auto& [type, component] : components) {
        json componentJson;
        componentJson["type"] = component->GetTypeName();
        componentJson["data"] = component->Serialize();
        j["components"].push_back(componentJson);
    }
    
    j["children"] = json::array();
    for (const auto& child : children) {
        j["children"].push_back(child->Serialize());
    }
    
    return j;
}

void Node::Deserialize(const json& j) {
    name = j["name"];
    active = j["active"];
    
    // Clear existing components except Transform
    auto transformComponent = GetTransform();
    components.clear();
    if (transformComponent) {
        components[std::type_index(typeid(Transform))] = transformComponent;
    }
    
    // Deserialize components
    for (const auto& componentJson : j["components"]) {
        std::string typeName = componentJson["type"];
        if (typeName == "Transform") {
            if (auto transform = GetTransform()) {
                transform->Deserialize(componentJson["data"]);
            }
        }
        // Add other component deserialization here
    }
    
    // Deserialize children
    children.clear();
    for (const auto& childJson : j["children"]) {
        auto child = std::make_shared<Node>();
        child->Deserialize(childJson);
        AddChild(child);
        child->parent = weak_from_this();
    }
}

// Scene implementation
Scene::Scene() {}

Scene::NodePtr Scene::CreateNode(const std::string& name) {
    auto node = std::make_shared<Node>(name);
    rootNodes.push_back(node);
    return node;
}

Scene::NodePtr Scene::CreateNode(NodePtr parent, const std::string& name) {
    auto node = std::make_shared<Node>(name);
    if (parent) {
        node->SetParent(parent);
    } else {
        rootNodes.push_back(node);
    }
    return node;
}

void Scene::RemoveNode(NodePtr node) {
    // Remove from root nodes if it's a root
    rootNodes.erase(
        std::remove(rootNodes.begin(), rootNodes.end(), node),
        rootNodes.end()
    );
    
    // Remove from parent if it has one
    if (auto parent = node->GetParent()) {
        parent->RemoveChild(node);
    }
    
    // Recursively remove all children
    auto children = node->GetChildren();
    for (auto& child : children) {
        RemoveNode(child);
    }
}

void Scene::ForEachNode(const std::function<void(NodePtr)>& fn) {
    std::vector<NodePtr> allNodes;
    CollectAllNodes(allNodes);
    for (auto& node : allNodes) {
        fn(node);
    }
}

Scene::NodePtr Scene::FindNodeByName(const std::string& name) {
    NodePtr result;
    ForEachNode([&](NodePtr node) {
        if (node->GetName() == name) {
            result = node;
        }
    });
    return result;
}

void Scene::Update(double deltaTime) {
    ForEachNode([deltaTime](NodePtr node) {
        if (node->IsActive()) {
            for (const auto& [type, component] : node->components) {
                component->Update(deltaTime);
            }
        }
    });
}

void Scene::CollectAllNodes(std::vector<NodePtr>& result) const {
    std::function<void(const NodePtr&)> collect;
    collect = [&](const NodePtr& node) {
        result.push_back(node);
        for (const auto& child : node->GetChildren()) {
            collect(child);
        }
    };
    
    for (const auto& node : rootNodes) {
        collect(node);
    }
}

json Scene::Serialize() const {
    json j;
    j["rootNodes"] = json::array();
    for (const auto& node : rootNodes) {
        j["rootNodes"].push_back(node->Serialize());
    }
    return j;
}

void Scene::Deserialize(const json& j) {
    rootNodes.clear();
    for (const auto& nodeJson : j["rootNodes"]) {
        auto node = std::make_shared<Node>();
        node->Deserialize(nodeJson);
        rootNodes.push_back(node);
    }
}

} // namespace Composable