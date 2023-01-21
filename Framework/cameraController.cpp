#include "cameraController.hpp"

CameraController::CameraController(float _fov, float _aspectRatio, float _nearPlane, float _farPlane,
                                   glm::vec3 const& startPosition)
    : fov(_fov), aspectRatio(_aspectRatio), nearPlane(_nearPlane), farPlane(_farPlane), position(startPosition)
{}

CameraController::~CameraController() {}

void CameraController::update(std::shared_ptr<tga::Interface> const& tgai, tga::Window window, float deltaTime)
{
    float moveSpeed = speed;

    if (tgai->keyDown(window, tga::Key::R)) moveSpeed *= boost;

    // Since the camera is pointing in the opposite direction of z, it inverts left and right rotation
    if (tgai->keyDown(window, tga::Key::Left)) yaw -= deltaTime * turnSpeed;
    if (tgai->keyDown(window, tga::Key::Right)) yaw += deltaTime * turnSpeed;
    if (tgai->keyDown(window, tga::Key::Up)) pitch += deltaTime * turnSpeed;
    if (tgai->keyDown(window, tga::Key::Down)) pitch -= deltaTime * turnSpeed;

    pitch = std::clamp(pitch, -89.f, 89.f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    auto lookDir = glm::normalize(direction);
    auto r = glm::normalize(glm::cross(lookDir, up));

    /*auto rotation = glm::quat(glm::vec3(-glm::radians(pitch),glm::radians(yaw),0.f));
    auto lookDir = glm::mat3_cast(rotation)*baseLookDir;
    auto r = rotation*glm::cross(up,lookDir);*/

    if (tgai->keyDown(window, tga::Key::W)) position += lookDir * deltaTime * moveSpeed;
    if (tgai->keyDown(window, tga::Key::S)) position -= lookDir * deltaTime * moveSpeed;
    if (tgai->keyDown(window, tga::Key::A)) position -= r * deltaTime * moveSpeed;
    if (tgai->keyDown(window, tga::Key::D)) position += r * deltaTime * moveSpeed;

    if (tgai->keyDown(window, tga::Key::Space)) position += up * deltaTime * moveSpeed;
    if (tgai->keyDown(window, tga::Key::Shift_Left)) position -= up * deltaTime * moveSpeed;

    camera.projection = glm::perspective_vk(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    camera.view = glm::lookAt(position, position + lookDir, up);
    camera.toWorld = glm::inverse(camera.view);  // glm::translate(glm::mat4_cast(rotation),position);

    if (cameraUB)
        tgai->updateBuffer(cameraUB, (uint8_t*)&camera, sizeof(camera), 0);
    else
        cameraUB = tgai->createBuffer({tga::BufferUsage::uniform, (uint8_t*)&camera, sizeof(camera)});
}

tga::Buffer CameraController::getCameraUB() const { return cameraUB; }

const Camera& CameraController::getCamera() const { return camera; }