#pragma once
#include <chrono>
#include <filesystem>
#include <random>

#include "cameraController.hpp"
#include "glm/gtx/string_cast.hpp"
#include "shaderData.hpp"
#include "tga/tga.hpp"
#include "tga/tga_math.hpp"
#include "tga/tga_utils.hpp"
#include "tga/tga_vulkan/tga_vulkan.hpp"
using namespace glm;
using namespace std;
using namespace tga;

struct Heightmap {
    /**
     * @brief Save access with wrapping behavior
     *
     * @param x x-cooordinate
     * @param y y-coordinate
     * @return float&, reference to the value, allowing read and write with one function
     */
    float& at(int x, int y);

    /**
     * @brief Remaps the values in the heightmap to the rango zero to one
     *
     */
    void normalize();

    int width, height;
    std::vector<float> data;
};

class RNG {
public:
    RNG();
    RNG(uint32_t seed);

    /**
     * @brief receive a random float between -1 and 1
     *
     */
    float getFloat();

    /**
     * @brief Set the seed of the generator
     *
     */
    void reSeed(uint32_t seed);
    /**
     * @brief Returns the currently used seed
     *
     */
    uint32_t getSeed();

private:
    std::random_device rd;
    uint32_t seed;
    std::mt19937 generator;
};

class DiamondSquare {
public:
    /**
     * @brief Uses the diamond square algorithm to construct a heightmap
     *
     * @param n power of two as in 2^n
     * @param wrap Whether the heightmap can be tiled
     * @param featureSizeRatio Decreases the feature size for wrapped implementation
     * @param seed Seed for the random number generator
     */
    DiamondSquare(int n, bool wrap = false, int featureSizeRatio = 1, uint32_t seed = 0);

    /**
     * @brief Returns the reference to the generated heightmap
     *
     */
    Heightmap& getHeightmap();

private:
    RNG rng;
    Heightmap heightmap;

    void diamondStep(int x, int y, int stepsize, double scale);
    void squareStep(int x, int y, int stepsize, double scale);
    void generate(int n);

    // Alternative Implementation
    void diamondStepWrapped(int x, int y, int stepsize, double scale);
    void squareStepWrapped(int x, int y, int stepsize, double scale);
    void generateWrapped(int n, int featureSizeRatio);
};

HDRImage generateMipMap(HDRImage image);
HDRImage createMipMap(HDRImage image, int mipLevel);
vector<float> createHeightmap(int size);

vector<vec3> generateNormalMap(vector<float> source, int length);
vector<uint8_t> convertNormalMap(vector<vec3> source);
vector<float> convertToFloats(vector<vec3> source);

class Framework {
public:
    // Override these
    virtual void OnCreate() {}       // Create Stuff here
    virtual void OnFixedUpdate() {}  // Gets called every fixedTimestep (default 60 times a seconds)
    virtual void OnUpdate(uint32_t backbufferIndex) { (void)backbufferIndex; }  // Gets called every frame
    virtual void OnDestroy() {}  // Gets called when it's time to delete stuff

protected:
    // Access those
    double deltaTime;          // Time since the last frame
    double fixedTimestep;      // Time between two calls to OnFixedUpdate()
    double totalElapsedTime;   // Total Time since the call to run()
    uint64_t totalFrameCount;  // Number of frames rendered so far

    std::shared_ptr<tga::Interface> tgai;  // Interface to TGA
    glm::uvec2 frameworkWindowResolution;  // The resolution of the framework window
    tga::Window frameworkWindow;           // The window managed by the framework

    std::unique_ptr<CameraController> camController;  // A simple first person camera controller

    bool sunMovement = true;  // Deactivating this will stop the update and movement of the sun

    // Use this to get standard data to your shader
    tga::InputSet makeSystemInputSet(tga::RenderPass renderPass)
    {
        return tgai->createInputSet({
                                        renderPass,  // The RenderPass needs to match the layout on the first set
                                        0,           // Index of the Set
                                        {{camController->getCameraUB(), 0}, {lightUB, 1}, {systemUB, 2}}  // What we want to bind
                                    });
    }
    struct Vertex {
        vec3 position;
        vec3 normal;
        vec2 texcoord;
    };

    struct BoundingSphere {
        float radius;
        vec4 center;
    };

    glm::vec3 terrainCenter;
    float terrainRadius;

    struct TerrainData {
        alignas(16) vec3 dimensions;
        alignas(4) float height;
    };
    Buffer vertexBuffer;
    Buffer indexBuffer;
    Buffer uniformBuffer;
    Buffer textureBuffer;
    //object
    Buffer meshVertexBuffer;
    Buffer meshIndexBuffer;
    Buffer meshUniformBuffer;
    // gatling
    Buffer meshVertexBufferGatling;
    Buffer meshIndexBufferGatling;
    Buffer meshUniformBufferGatling;
    Buffer meshVertexBufferGatlingBase;
    Buffer meshIndexBufferGatlingBase;
    Buffer meshUniformBufferGatlingBase;
    //plasma
    Buffer meshVertexBufferPlasma;
    Buffer meshIndexBufferPlasma;
    Buffer meshUniformBufferPlasma;
    Buffer meshVertexBufferPlasmaBase;
    Buffer meshIndexBufferPlasmaBase;
    Buffer meshUniformBufferPlasmaBase;

    tga::Obj enemy;
    Buffer enemyVertex;
    Buffer enemyIndex;

    vector<float> heightmap;
    vector<vec3> normalmap;
    vector<Vertex> vertices;
    vector<int> index;
    TerrainData uniformTerrain;
    string pathToTheTexture = "resources/Surface_Textures/gras15.png";
    InputSet actualInputSet;
    InputSet textureInputSet;
    InputSet meshInputSet;
    InputSet meshInputSetGatling;
    InputSet meshInputSetPlasma;
    Buffer meshCockpitUniformBuffer;

    tga::Obj mesh;
    tga::Obj meshGunGatling;
    tga::Obj meshGunGatlingBase;
    tga::Obj meshGunPlasma;
    tga::Obj meshGunPlasmaBase;

    Buffer vertexBufferGuns;
    Buffer indexBufferGuns;

    Buffer matrixBuffer;
    Buffer matrixBufferGatling;
    Buffer matrixBufferPlasma;

    vector<Vertex> createVertexBuffer(vector<float> heightmap, vector<vec3> normalmap);
    vector<int> createIndexBuffer(vector<Vertex> vertices);
    TerrainData createTerrainUniformBuffer(vector<float> heightmap);

public:
    Framework()
        : deltaTime(1. / 60.), fixedTimestep(1. / 60.), totalElapsedTime(0), totalFrameCount(0),
          tgai(std::make_shared<tga::TGAVulkan>())
    {}

    // Call this to start the framework, ignore everything further down
    void run(glm::uvec2 resolution = glm::uvec2(0), tga::PresentMode presentMode = tga::PresentMode::vsync)
    {
        using namespace std::chrono_literals;

        if (!resolution.x || !resolution.y) {
            auto [wx, wy] = tgai->screenResolution();
            resolution = {wx, wy};
        }
        frameworkWindowResolution = resolution;
        frameworkWindow = tgai->createWindow({frameworkWindowResolution.x, frameworkWindowResolution.y, presentMode});

        camController = std::make_unique<CameraController>(
            90., frameworkWindowResolution.x / float(frameworkWindowResolution.y), 0.1, 5000.);
        camController->update(tgai, frameworkWindow, 0);
        updateSystemBuffers();
        OnCreate();

        static double accumulator = 0;
        auto beginTime = std::chrono::steady_clock::now();

        while (!tgai->windowShouldClose(frameworkWindow)) {
            accumulator += deltaTime;
            while (accumulator >= fixedTimestep) {
                tgai->pollEvents(frameworkWindow);
                OnFixedUpdate();
                accumulator -= fixedTimestep;
            }
            auto nextFrame = tgai->nextFrame(frameworkWindow);
            camController->update(tgai, frameworkWindow, deltaTime);
            updateSystemBuffers();
            OnUpdate(nextFrame);

            tgai->present(frameworkWindow);
            auto endTime = std::chrono::steady_clock::now();
            deltaTime = std::chrono::duration<double>(endTime - beginTime).count();
            beginTime = endTime;
            totalElapsedTime += deltaTime;
            totalFrameCount++;
        }

        OnDestroy();
        tgai->free(camController->getCameraUB());
        tgai->free(systemUB);
        tgai->free(lightUB);
        tgai->free(frameworkWindow);
    }

    /* Ignore this part unless you want to do some fancy stuff*/

protected:
    void updateSystemBuffers()
    {
        // Light Data
        light.color = glm::vec4(1., 0.894, 0.518, 1.);
        if (sunMovement || !lightUB) {                   // Way to complicated light movement
            constexpr double realDaysToIngameDays = 72;  // 1 day ~ 20 minutes
            constexpr double secondsToHours = 1. / (60 * 60 * 60);
            auto localTimeHours = glm::mod(totalElapsedTime * secondsToHours * realDaysToIngameDays, 24.);
            float hourAngle = glm::radians(15 * (localTimeHours - 8));
            glm::vec3 rotation = {hourAngle, 0, 0};
            light.direction = glm::normalize(glm::mat3(glm::quat(rotation)) * -up);
            light.position = light.direction * 1.49e11f;
        }

        if (lightUB)
            tgai->updateBuffer(lightUB, tga::memoryAccess(light), sizeof(light), 0);
        else
            lightUB = tgai->createBuffer({tga::BufferUsage::uniform, tga::memoryAccess(light), sizeof(light)});

        // System Data
        systemData.resolution = frameworkWindowResolution;
        systemData.time = totalElapsedTime;

        if (systemUB)
            tgai->updateBuffer(systemUB, tga::memoryAccess(systemData), sizeof(systemData), 0);
        else
            systemUB =
                tgai->createBuffer({tga::BufferUsage::uniform, tga::memoryAccess(systemData), sizeof(systemData)});
    }

    // Those are managed for you
    SystemData systemData;
    Light light;
    tga::Buffer systemUB, lightUB;
};