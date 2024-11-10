#pragma once

#include "composable.h"

namespace Composable {

class Transform : public Component {
public:
    struct Vec3 {
        float x, y, z;
        Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
        json Serialize() const;
        void Deserialize(const json& j);
    };

    Transform();
    
    // Local transform properties
    INLINE void SetLocalPosition(const Vec3& position);
    INLINE void SetLocalRotation(const Vec3& rotation);
    INLINE void SetLocalScale(const Vec3& scale);

    INLINE Vec3 GetLocalPosition() const { return localPosition; }
    INLINE Vec3 GetLocalRotation() const { return localRotation; }
    INLINE Vec3 GetLocalScale() const { return localScale; }

    // World transform properties
    INLINE Vec3 GetWorldPosition() const;
    INLINE Vec3 GetWorldRotation() const;
    INLINE Vec3 GetWorldScale() const;
    
    // Transform operations
    void TranslateLocal(const Vec3& delta);
    void RotateLocal(const Vec3& delta);
    
    // Component interface
    void OnAttach() override;
    void OnDetach() override;
    json Serialize() const override;
    void Deserialize(const json& j) override;
    std::string GetTypeName() const override { return "Transform"; }

private:
    Vec3 localPosition;
    Vec3 localRotation;
    Vec3 localScale;
    
    mutable bool dirty;
    mutable Vec3 worldPosition;
    mutable Vec3 worldRotation;
    mutable Vec3 worldScale;
    
    void UpdateWorldTransform() const;
};

} // namespace Composable