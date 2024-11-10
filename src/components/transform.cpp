#include "transform.h"
#include <cmath>

namespace Composable {

// Vec3 serialization implementations
json Transform::Vec3::Serialize() const {
    json j;
    j["x"] = x;
    j["y"] = y;
    j["z"] = z;
    return j;
}

void Transform::Vec3::Deserialize(const json& j) {
    x = j["x"];
    y = j["y"];
    z = j["z"];
}

Transform::Transform()
    : localPosition(0, 0, 0)
    , localRotation(0, 0, 0)
    , localScale(1, 1, 1)
    , dirty(true)
    , worldPosition(0, 0, 0)
    , worldRotation(0, 0, 0)
    , worldScale(1, 1, 1)
{}

void Transform::OnAttach() {
    dirty = true;
    UpdateWorldTransform();
}

void Transform::OnDetach() {
    // Reset to local values
    worldPosition = localPosition;
    worldRotation = localRotation;
    worldScale = localScale;
}

void Transform::SetLocalPosition(const Vec3& position) {
    localPosition = position;
    dirty = true;
    
    // Mark children as dirty
    if (auto node = owner.lock()) {
        for (const auto& child : node->GetChildren()) {
            if (auto childTransform = child->GetComponent<Transform>()) {
                childTransform->dirty = true;
            }
        }
    }
}

void Transform::SetLocalRotation(const Vec3& rotation) {
    localRotation = rotation;
    dirty = true;
    
    // Mark children as dirty
    if (auto node = owner.lock()) {
        for (const auto& child : node->GetChildren()) {
            if (auto childTransform = child->GetComponent<Transform>()) {
                childTransform->dirty = true;
            }
        }
    }
}

void Transform::SetLocalScale(const Vec3& scale) {
    localScale = scale;
    dirty = true;
    
    // Mark children as dirty
    if (auto node = owner.lock()) {
        for (const auto& child : node->GetChildren()) {
            if (auto childTransform = child->GetComponent<Transform>()) {
                childTransform->dirty = true;
            }
        }
    }
}

Transform::Vec3 Transform::GetWorldPosition() const {
    if (dirty) {
        UpdateWorldTransform();
    }
    return worldPosition;
}

Transform::Vec3 Transform::GetWorldRotation() const {
    if (dirty) {
        UpdateWorldTransform();
    }
    return worldRotation;
}

Transform::Vec3 Transform::GetWorldScale() const {
    if (dirty) {
        UpdateWorldTransform();
    }
    return worldScale;
}

void Transform::TranslateLocal(const Vec3& delta) {
    SetLocalPosition(Vec3(
        localPosition.x + delta.x,
        localPosition.y + delta.y,
        localPosition.z + delta.z
    ));
}

void Transform::RotateLocal(const Vec3& delta) {
    SetLocalRotation(Vec3(
        localRotation.x + delta.x,
        localRotation.y + delta.y,
        localRotation.z + delta.z
    ));
}

void Transform::UpdateWorldTransform() const {
    if (auto node = owner.lock()) {
        if (auto parent = node->GetParent()) {
            if (auto parentTransform = parent->GetComponent<Transform>()) {
                // Get parent's world transform
                auto parentWorldPos = parentTransform->GetWorldPosition();
                auto parentWorldRot = parentTransform->GetWorldRotation();
                auto parentWorldScale = parentTransform->GetWorldScale();
                
                // Calculate world position
                worldPosition = Vec3(
                    parentWorldPos.x + (localPosition.x * parentWorldScale.x),
                    parentWorldPos.y + (localPosition.y * parentWorldScale.y),
                    parentWorldPos.z + (localPosition.z * parentWorldScale.z)
                );
                
                // Calculate world rotation (simple additive - in a real engine you'd use quaternions)
                worldRotation = Vec3(
                    parentWorldRot.x + localRotation.x,
                    parentWorldRot.y + localRotation.y,
                    parentWorldRot.z + localRotation.z
                );
                
                // Calculate world scale (multiplicative)
                worldScale = Vec3(
                    parentWorldScale.x * localScale.x,
                    parentWorldScale.y * localScale.y,
                    parentWorldScale.z * localScale.z
                );
            } else {
                // Parent has no transform, use local values
                worldPosition = localPosition;
                worldRotation = localRotation;
                worldScale = localScale;
            }
        } else {
            // No parent, world transform equals local transform
            worldPosition = localPosition;
            worldRotation = localRotation;
            worldScale = localScale;
        }
    }
    
    dirty = false;
}

json Transform::Serialize() const {
    json j;
    j["position"] = localPosition.Serialize();
    j["rotation"] = localRotation.Serialize();
    j["scale"] = localScale.Serialize();
    return j;
}

void Transform::Deserialize(const json& j) {
    localPosition.Deserialize(j["position"]);
    localRotation.Deserialize(j["rotation"]);
    localScale.Deserialize(j["scale"]);
    dirty = true;
}

} // namespace Composable