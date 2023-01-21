#pragma once
#include "glm/gtx/string_cast.hpp"
#include "shaderData.hpp"
#include "tga/tga_math.hpp"

class CameraController {
public:
    CameraController(float _fov, float _aspectRatio, float _nearPlane, float _farPlane,
                     glm::vec3 const& startPosition = glm::vec3{0});
    ~CameraController();
    void update(std::shared_ptr<tga::Interface> const& tgai, tga::Window window, float deltaTime);
    const Camera& getCamera() const;
    tga::Buffer getCameraUB() const;

    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    float speed = 4;
    float boost = 2;
    float turnSpeed = 75;

    glm::vec3 position;

private:
    Camera camera;
    tga::Buffer cameraUB;

    float pitch = 0;
    float yaw = -90;
    static constexpr glm::vec3 baseLookDir{0, 0, -1};
};
