/*GAME*/

#include "framework.hpp"
using namespace std;

/*Heightmap Function Implementations*/
float& Heightmap::at(int x, int y)
{
    // Wrap around for the coordinates
    x = (x + width) % width;
    y = (y + height) % height;
    return data[x + y * width];
}

void Heightmap::normalize()
{
    float minF = 4e20;  // Pretty high
    float maxF = -6e9;  // Immature Joke
    for (auto f : data) {
        minF = glm::min(minF, f);
        maxF = glm::max(maxF, f);
    }
    for (auto& f : data) f = glm::remap(f, minF, maxF, 0.f, 1.f);
}

/*RNG Function Implementations*/
RNG::RNG() : seed(rd()), generator(seed) {}

RNG::RNG(uint32_t seed) : seed(seed), generator(seed) {}

uint32_t RNG::getSeed() { return seed; }

void RNG::reSeed(uint32_t seed) { generator.seed(seed); }

float RNG::getFloat()
{
    // With a Sigma of .5, the values range roughly from -1.5 to 1.5
    auto dist = std::normal_distribution<float>{0, .5};
    float val = dist(generator);
    return val;
}

/*Diamond Square Function Implementations*/
DiamondSquare::DiamondSquare(int n, bool wrap, int featureSizeRatio, uint32_t seed)
{
    if (seed)
        rng.reSeed(seed);
    else {
        std::cout << "[Resource Generator] Diamond Square" << (wrap ? " Wrapped" : "")
                  << " using seed: " << rng.getSeed() << "\n";
    }
    if (wrap)
        generateWrapped(n, featureSizeRatio);
    else
        generate(n);
}

Heightmap& DiamondSquare::getHeightmap() { return heightmap; }

// Regular diamond step with no wrapping
void DiamondSquare::diamondStep(int x, int y, int step, double scale)
{
    // Could I just pass halfstep instead of step as parameter?
    int hs = step / 2;
    // Yes, but hs is shorter to write
    // and nobody would now what "hs" means as a paramter

    int total = 0;
    float value = 0;

    bool left = x - hs >= 0;
    bool right = x + hs < heightmap.width;
    bool up = y - hs >= 0;
    bool down = y + hs < heightmap.height;

    if (up && left) {
        total++;
        value += heightmap.at(x - hs, y - hs);
    }
    if (up && right) {
        total++;
        value += heightmap.at(x + hs, y - hs);
    }
    if (down && left) {
        total++;
        value += heightmap.at(x - hs, y + hs);
    }
    if (down && right) {
        total++;
        value += heightmap.at(x + hs, y + hs);
    }

    heightmap.at(x, y) = value / total + rng.getFloat() * scale;
}

// Regular square step with no wrapping
void DiamondSquare::squareStep(int x, int y, int step, double scale)
{
    int hs = step / 2;

    int total = 0;
    float value = 0;
    if (x - hs >= 0) {
        value += heightmap.at(x - hs, y);
        total++;
    }
    if (x + hs < heightmap.width) {
        value += heightmap.at(x + hs, y);
        total++;
    }
    if (y - hs >= 0) {
        value += heightmap.at(x, y - hs);
        total++;
    }
    if (y + hs < heightmap.height) {
        value += heightmap.at(x, y + hs);
        total++;
    }
    heightmap.at(x, y) = value / total + rng.getFloat() * scale;
}


// Regular diamond square algorithm with no wrapping
void DiamondSquare::generate(int n)
{

    int res = (1 << n) + 1;
    heightmap = {res, res, std::vector<float>(res * res)};

    // Init corners
    heightmap.at(0, 0) = rng.getFloat();
    heightmap.at(0, res - 1) = rng.getFloat();
    heightmap.at(res - 1, 0) = rng.getFloat();
    heightmap.at(res - 1, res - 1) = rng.getFloat();

    double scale = 1;
    for (int step = res - 1; step > 1; step /= 2, scale /= 2) {
        int halfstep = step / 2;

        for (int y = halfstep; y < res; y += step) {
            for (int x = halfstep; x < res; x += step) diamondStep(x, y, step, scale);
        }

        // Two for loops since the diamond centers are offset every second row
        for (int y = 0; y < res; y += step) {
            for (int x = halfstep; x < res; x += step) squareStep(x, y, step, scale);
        }
        for (int y = halfstep; y < res; y += step) {
            for (int x = 0; x < res; x += step) squareStep(x, y, step, scale);
        }
    }
}

// Diamond step with wrapping
// Would this qualify for the bonus task "Other Algorithm"?  Maybe, but probably not
void DiamondSquare::diamondStepWrapped(int x, int y, int step, double scale)
{
    int hs = step / 2;
    heightmap.at(x, y) = (heightmap.at(x - hs, y - hs) + heightmap.at(x + hs, y - hs) + heightmap.at(x - hs, y + hs) +
                          heightmap.at(x + hs, y + hs)) /
                         4 +
                         rng.getFloat() * scale;
}

// Square step with wrapping
void DiamondSquare::squareStepWrapped(int x, int y, int step, double scale)
{
    int hs = step / 2;
    heightmap.at(x, y) = (heightmap.at(x - hs, y + 0) + heightmap.at(x + hs, y + 0) + heightmap.at(x + 0, y - hs) +
                          heightmap.at(x + 0, y + hs)) /
                         4 +
                         rng.getFloat() * scale;
}

// Diamond Square Algorithm using wrapping behavior of heightmap access
void DiamondSquare::generateWrapped(int n, int featureSizeRatio)
{
    int res = (1 << n);
    heightmap = {res, res, std::vector<float>(res * res)};

    // Wrapping behavior allows for variance in the feature size of the algorithm
    // Default feature size of 2 makes more interesting heightmaps (personal opinion)
    int featureSize = res / featureSizeRatio;
    heightmap.at(0, 0) = rng.getFloat();
    heightmap.at(featureSize, 0) = rng.getFloat();
    heightmap.at(0, featureSize) = rng.getFloat();
    heightmap.at(featureSize, featureSize) = rng.getFloat();

    double scale = 1;
    for (int step = featureSize; step > 1; step /= 2, scale /= 2) {
        int halfstep = step / 2;

        for (int y = halfstep; y < heightmap.height + halfstep; y += step)
            for (int x = halfstep; x < heightmap.width + halfstep; x += step) diamondStepWrapped(x, y, step, scale);

        for (int y = 0; y < res; y += step)
            for (int x = halfstep; x < res; x += step) squareStepWrapped(x, y, step, scale);

        for (int y = halfstep; y < res; y += step)
            for (int x = 0; x < res; x += step) squareStepWrapped(x, y, step, scale);
    }
}

HDRImage createMipMap(HDRImage image, int mipLevel)
{
    HDRImage result = image;
    for (int currentLevel = 0; currentLevel < mipLevel; currentLevel++) {
        if (result.width == result.height == 1) {
            return result;
        }
        result = generateMipMap(result);
    }
    return result;
}

HDRImage generateMipMap(HDRImage image)
{
    vector<float> data = image.data;
    vector<float> newData;
    for (int y = 0; y < image.height - 1; y++) {
        for (int x = 0; x < image.width - 1; x++) {
            float value;
            value = data[x + y * image.width] + data[x + 1 + y * image.width] + data[x + (y + 1) * image.width] +
                    data[x + 1 + (y + 1) * image.width] / 4.0f;
            newData.push_back(value);
            x++;
        }
        y++;
    }
    HDRImage result = {image.width / 2, image.height / 2, image.components, newData};
    return result;
}

vector<float> createHeightmap(int size)
{
    // int size = 1025;
    int maxLen = std::max(size, size);
    int n = -1;
    while (maxLen) {  // Count how many times you can divide by 2
        n++;
        maxLen /= 2;
    }
    int powerTwo = 1 << n;  // A shift to the left is equivalent to multiplying by 2 and we shift n time
    if (powerTwo < std::max(size, size)) n++;

    DiamondSquare ds{n, false, false, false};
    auto& tempHeightmap = ds.getHeightmap();

    // Trim the heightmap to the desired dimensions
    Heightmap heightmap{size, size, std::vector<float>(size * size)};
    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++) heightmap.at(x, y) = tempHeightmap.at(x, y);

    // Remap to a valid range, we can't have less than 0 in hdr
    heightmap.normalize();
    return heightmap.data;
}

vector<vec3> generateNormalMap(vector<float> source, int length)
{
    vector<float> data = source;
    vector<vec3> result;
    for (int y = 0; y < length; y++) {
        for (int x = 0; x < length; x++) {
            float fx0, fx1;
            if (x > 0)
                fx0 = data[x - 1 + y * length];
            else
                fx0 = data[x + y * length];

            if (x < length - 1)
                fx1 = data[x + 1 + y * length];
            else
                fx1 = data[x + y * length];

            float fy0, fy1;
            if (y > 0)
                fy0 = data[x + (y - 1) * length];
            else
                fy0 = data[x + y * length];

            if (y < length - 1)
                fy1 = data[x + (y + 1) * length];
            else
                fy1 = data[x + y * length];

            float scaleX = remap((float)x, 1.0f, (float)length, 0.0f, 1.0f);
            float scaleY = remap((float)y, 1.0f, (float)length, 0.0f, 1.0f);

            float dx = (fx1 - fx0) / (2 * scaleX), dy = (fy1 - fy0) / (2 * scaleY);

            vec3 n = vec3(-1 * dx, -1 * dy, (scaleX + scaleY) / length);
            n = normalize(n);
            result.push_back(n);
        }
    }
    return result;
}

vector<float> convertToFloats(vector<vec3> source)
{
    vector<float> result;
    for (vec3 current : source) {
        result.push_back(current.z);
    }
    return result;
}

vector<uint8_t> convertNormalMap(vector<vec3> source)
{
    vector<uint8_t> result;
    for (vec3 current : source) {
        uint8_t valueX = remap(current.x, -1.0f, 1.0f, 0.0f, 255.0f);
        uint8_t valueY = remap(current.y, -1.0f, 1.0f, 0.0f, 255.0f);
        uint8_t valueZ = remap(current.z, -1.0f, 1.0f, 0.0f, 255.0f);
        result.push_back(valueX);
        result.push_back(valueY);
        result.push_back(valueZ);
    }
    return result;
}

class Game : public Framework {
    vector<Vertex> createVertexBuffer(vector<float> heightmap, vector<vec3> normalmap)
    {
        this->heightmap = heightmap;
        this->normalmap = normalmap;
        vector<Vertex> vertices;
        vertices.resize(heightmap.size());

        for (float z = 0; z < sqrt(heightmap.size()); z++) {
            for (float x = 0; x < sqrt(heightmap.size()); x++) {
                vertices[z * sqrt(heightmap.size()) + x].position = {(2.0f * x) / (sqrt(heightmap.size()) - 1) - 1.0f,
                                                                     heightmap[z * sqrt(heightmap.size()) + x],
                                                                     (2.0f * z) / (sqrt(heightmap.size()) - 1) - 1.0};
                vertices[z * sqrt(heightmap.size()) + x].texcoord = {
                    static_cast<float>(x) / (sqrt(heightmap.size()) - 1),
                    static_cast<float>(z) / (sqrt(heightmap.size()) - 1)};
            }
        }

        for (int i = 0; i < normalmap.size(); i++) {
            vertices[i].normal.x = normalmap[i].x;
            vertices[i].normal.y = normalmap[i].y;
            vertices[i].normal.z = normalmap[i].z;
        }

        this->vertices = vertices;
        return vertices;
    }

    vector<int> createIndexBuffer(vector<Vertex> vertices)
    {
        vector<int> index;
        for (float z = 0; z < sqrt(heightmap.size())-1; z++) {
            for (float x = 0; x < sqrt(heightmap.size()) - 1; x++) {
                index.push_back(x + (z * sqrt(vertices.size())));
                index.push_back((x + (z * sqrt(vertices.size())) + 1));
                index.push_back(x + ((z + 1) * sqrt(vertices.size())));
                index.push_back(x + ((z + 1) * sqrt(vertices.size())));
                index.push_back(x + 1 + (z * sqrt(vertices.size())));
                index.push_back(x + 1 + ((z + 1) * sqrt(vertices.size())));
            }
        }

        this->index = index;
        return index;
    }

    TerrainData createTerrainUniformBuffer(vector<float> heightmap)
    {
        TerrainData uniformTerrain;
        uniformTerrain.dimensions = vec3(10.0f, 10.0f, 10.0f);
        uniformTerrain.height = 10.0f;
        this->uniformTerrain = uniformTerrain;
        return uniformTerrain;
    }

    tga::Texture createTextureUniformBuffer(string pathToTexture)
    {
        return tga::loadTexture(pathToTexture, tga::Format::r8g8b8a8_srgb, tga::SamplerMode::linear, tgai);
    }

    void createBackgroundResources()
    {
        auto bgVS = tga::loadShader("shaders/background_vert.spv", tga::ShaderType::vertex, tgai);
        auto bgFS = tga::loadShader("shaders/background_frag.spv", tga::ShaderType::fragment, tgai);
        backgroundPass = tgai->createRenderPass(
            {{bgVS, bgFS},
             frameworkWindow,
             tga::ClearOperation::none,
             {},
             {},
             {{/* Three uniform Buffers for the System Input*/
                  {tga::BindingType::uniformBuffer, tga::BindingType::uniformBuffer, tga::BindingType::uniformBuffer}}}});
        systemInputSet = makeSystemInputSet(backgroundPass);

        // Shaders get backed into the renderpass, we don't need the modules anymore, so free them
        tgai->free(bgVS);
        tgai->free(bgFS);
    }

    void createTerrainResources()
    {
        auto terrainVS = tga::loadShader("shaders/terrain_proxy_vert.spv", tga::ShaderType::vertex, tgai);
        auto terrainFS = tga::loadShader("shaders/terrain_proxy_frag.spv", tga::ShaderType::fragment, tgai);

        /*TODO: adjust to your own Vertex Buffer Layout*/
        tga::VertexLayout terrainVertexLayout{
            /*Size of a single Vertex, vec4 position, vec3 normal, vec2 textureCoordinates*/
            (sizeof(Vertex)),
            {
                /*{offset in bytes from start of a vector, Format of the component}*/
                {offsetof(Vertex, position), tga::Format::r32g32b32_sfloat}, /* Position x,y,z */
                {offsetof(Vertex, normal), tga::Format::r32g32b32_sfloat},   /* Normal x,y,z*/
                {offsetof(Vertex, texcoord), tga::Format::r32g32_sfloat}     /* Texture Coordinates x,y*/
            }};

        terrainPass = tgai->createRenderPass(
            {{terrainVS, terrainFS},
             frameworkWindow,
             tga::ClearOperation::depth,
             {tga::FrontFace::clockwise,tga::CullMode::none},
             {tga::CompareOperation::less},
             {{/*Set 0: Three uniform Buffers for the System Input*/
                  {tga::BindingType::uniformBuffer, tga::BindingType::uniformBuffer, tga::BindingType::uniformBuffer},

                  /*Set 1: Terrain Data*/
                  {tga::BindingType::uniformBuffer,
                   tga::BindingType::sampler /*TODO: Declare more Bindings for your Shader here*/}

              }},
             terrainVertexLayout});

        terrainCenter = vec3(5.0f, 5.0f, 5.0f);
        terrainRadius = 10.0f;
        vector<float> heightHuiMap = createHeightmap(1025);

        vector<vec3> normalHuiMap = generateNormalMap(heightHuiMap, 1025);

        vector<Vertex> vertices = createVertexBuffer(heightHuiMap, normalHuiMap);

        this->vertexBuffer = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::vertex, tga::memoryAccess(vertices), vertices.size() * sizeof(Vertex)});

        vector<int> indices = createIndexBuffer(this->vertices);

        this->indexBuffer = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::index, tga::memoryAccess(indices), indices.size() * sizeof(int)});

        TerrainData uniformTerrainData = createTerrainUniformBuffer(heightHuiMap);
        this->uniformBuffer = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(uniformTerrainData), sizeof(TerrainData)});

        terrainInputSet = tgai->createInputSet({terrainPass,1,{{uniformBuffer, 0},}});
        tga::Texture texture =
            loadTexture(this->pathToTheTexture, tga::Format::r32g32b32a32_sfloat, tga::SamplerMode::linear, tgai);
        terrainInputSet = tgai->createInputSet({terrainPass, 1, {{uniformBuffer, 0}, {texture, 1}}});

        // Shaders get backed into the renderpass, we don't need the modules anymore, so free them
        tgai->free(terrainVS);
        tgai->free(terrainFS);

        /*TODO: Terrain Buffer Creation*/  //
    }



    BoundingSphere createBoundingSphere(vector<tga::Vertex> vertices) {
        BoundingSphere boundingSphere{};

        vec3 sum;
        vec4 maxRad = vec4(0,0,0, 0);
        for(tga::Vertex vertex: vertices) {
            sum += vertex.position;
        }

        boundingSphere.center = vec4(sum.x / vertices.size(), sum.y / vertices.size(), sum.z / vertices.size(), 1);
        //vec3 cntr = vec3(sum.x/vertices.size(), sum.y/vertices.size(), sum.x/vertices.size(), sum.x/vertices.size())

        for(tga::Vertex vertex: vertices) {
            float dotProduct = abs(glm::dot(vec4(vertex.position, 1), boundingSphere.center));
            if(dotProduct > abs(glm::dot(maxRad, boundingSphere.center))) {
                maxRad = vec4(vertex.position, 1);
            }
        }
        boundingSphere.radius = abs(glm::dot(maxRad, boundingSphere.center));
        return boundingSphere;
    }



    void createEnemyResources()
    {
        auto enemyVS = tga::loadShader("shaders/instances_vert.spv", tga::ShaderType::vertex, tgai);
        auto enemyFS = tga::loadShader("shaders/phong_frag.spv", tga::ShaderType::fragment, tgai);
        this->enemy = tga::loadObj("resources/Enemies/amy/amy.obj");
        VertexLayout enemyVertexLayout = tga::Vertex::layout();
        this->enemyPass = tgai->createRenderPass(
            {{enemyVS, enemyFS},
             frameworkWindow,
             tga::ClearOperation::depth,
             {tga::FrontFace::counterclockwise, tga::CullMode::none},
             {tga::CompareOperation::less},
             {{
                  /*Set 0: Three uniform Buffers for the System Input*/
                  {tga::BindingType::uniformBuffer, tga::BindingType::uniformBuffer, tga::BindingType::uniformBuffer},

                  /*Set 1: Terrain Data*/
                  {tga::BindingType::sampler, tga::BindingType::sampler, tga::BindingType::sampler,
                   tga::BindingType::uniformBuffer, tga::BindingType::storageBuffer},
              }},
             enemyVertexLayout});

        this->enemyVertex =
            tgai->createBuffer(tga::BufferInfo{tga::BufferUsage::vertex, tga::memoryAccess(this->enemy.vertexBuffer),
                                               this->enemy.vertexBuffer.size() * sizeof(tga::Vertex)});


        this->enemyIndex = tgai->createBuffer(tga::BufferInfo{tga::BufferUsage::index, tga::memoryAccess(this->enemy.indexBuffer),
                                                              this->enemy.indexBuffer.size() * sizeof(glm::uint32_t)});





        transformations[0] = glm::translate(glm::mat4(1.0f), glm::vec3(5, 2, 8)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.002, 0.002, 0.002)) *
                             glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(1, 0, 0)) *
                             glm::rotate(glm::mat4(1.0f), -1.14f, glm::vec3(0, 1, 0));



        transformations[0] = glm::translate(glm::mat4(1.0f), generateTranslationVector(0)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.002, 0.002, 0.002)) *
                             glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(1, 0, 0)) *
                             glm::rotate(glm::mat4(1.0f), -1.14f, glm::vec3(0, 1, 0));

        transformations[1] = glm::translate(glm::mat4(1.0f), generateTranslationVector(27)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.002, 0.002, 0.002)) *
                             glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(1, 0, 0));
        transformations[2] = glm::translate(glm::mat4(1.0f), generateTranslationVector(5.5)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.002, 0.002, 0.002)) *
                             glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(1, 0, 0)) *
                             glm::rotate(glm::mat4(1.0f), 1.14f, glm::vec3(0, 1, 0));
        transformations[3] = glm::translate(glm::mat4(1.0f), generateTranslationVector(3)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.002, 0.002, 0.002)) *
                             glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(1, 0, 0)) *
                             glm::rotate(glm::mat4(1.0f), 12.44f, glm::vec3(0, 1, 0));
        transformations[4] = glm::translate(glm::mat4(1.0f), generateTranslationVector(150)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.002, 0.002, 0.002)) *
                             glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(1, 0, 0)) *
                             glm::rotate(glm::mat4(1.0f), 2.04f, glm::vec3(0, 1, 0));
        transformations[5] = glm::translate(glm::mat4(1.0f), generateTranslationVector(6)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(0.002, 0.002, 0.002)) *
                             glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(1, 0, 0)) *
                             glm::rotate(glm::mat4(1.0f), 2.04f, glm::vec3(0, 1, 0));




        for(int i = 0; i < 6; i++){
            boundingSpheres[i] = createBoundingSphere(enemy.vertexBuffer);
            boundingSpheres[i].center = transformations[i] * boundingSpheres[i].center;
        }


        enemyStorage = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::storage, tga::memoryAccess(transformations), 6 * sizeof(mat4x4)});

        tga::Texture texture_dif = loadTexture("resources/Enemies/amy/amy_diffuse.png",
                                               tga::Format::r32g32b32a32_sfloat, tga::SamplerMode::linear, tgai);
        tga::Texture texture_em = loadTexture("resources/Enemies/amy/amy_emission.png",
                                              tga::Format::r32g32b32a32_sfloat, tga::SamplerMode::linear, tgai);
        tga::Texture texture_spec = loadTexture("resources/Enemies/amy/amy_specular.png",
                                                tga::Format::r32g32b32a32_sfloat, tga::SamplerMode::linear, tgai);

        this->meshCockpitUniformBuffer = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(camController->position), sizeof(vec3)});
        enemyInputSet = tgai->createInputSet(
            {enemyPass,
             1,
             { {texture_dif, 0}, {texture_em, 1}, {texture_spec, 2}, {meshCockpitUniformBuffer, 3},{enemyStorage, 4}}});

        tgai->free(enemyVS);
        tgai->free(enemyFS);
    }






    void createMeshResources() {
        auto meshVS = tga::loadShader("shaders/phong_vert.spv", tga::ShaderType::vertex, tgai);
        auto meshFS = tga::loadShader("shaders/phong_frag.spv", tga::ShaderType::fragment, tgai);

        this->mesh = tga::loadObj("resources/Cockpit/cockpit/cockpit.obj");
        this->meshGunGatling = tga::loadObj("resources/Cockpit/gatling_gun/gatling_gun_barrel.obj");
        this->meshGunGatlingBase = tga::loadObj("resources/Cockpit/gatling_gun/gatling_gun_base.obj");
        this->meshGunPlasma = tga::loadObj("resources/Cockpit/plasma_gun/plasma_gun_barrel.obj");
        this->meshGunPlasmaBase = tga::loadObj("resources/Cockpit/plasma_gun/plasma_gun_base.obj");
        tga::VertexLayout meshVertexLayout{
            /*Size of a single Vertex, vec4 position, vec3 normal, vec2 textureCoordinates*/
            (sizeof(tga::Vertex)),
            {
                /*{offset in bytes from start of a vector, Format of the component}*/
                {offsetof(tga::Vertex, position), tga::Format::r32g32b32_sfloat}, /* Position x,y,z */
                {offsetof(tga::Vertex, uv), tga::Format::r32g32_sfloat},     /* Texture Coordinates x,y*/
                {offsetof(tga::Vertex, normal), tga::Format::r32g32b32_sfloat},   /* Normal x,y,z*/
                {offsetof(tga::Vertex, tangent), tga::Format::r32g32b32_sfloat} /*Tangent x, y, x */
            }};
        this->meshPass = tgai->createRenderPass(
            {{meshVS, meshFS},
             frameworkWindow,
             tga::ClearOperation::depth,
             {tga::FrontFace::counterclockwise,tga::CullMode::none},
             {tga::CompareOperation::less},
             {{/*Set 0: Three uniform Buffers for the System Input*/
                  {tga::BindingType::uniformBuffer, tga::BindingType::uniformBuffer, tga::BindingType::uniformBuffer},

                  /*Set 1: Terrain Data*/
                  {tga::BindingType::sampler, tga::BindingType::sampler, tga::BindingType::sampler, tga::BindingType::uniformBuffer, tga::BindingType::uniformBuffer /*TODO: Declare more Bindings for your Shader here*/},
              }},
             meshVertexLayout});


        // mesh for cockpit
        this->meshVertexBuffer = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::vertex, tga::memoryAccess(this->mesh.vertexBuffer), this->mesh.vertexBuffer.size() * sizeof(tga::Vertex)});

        this->meshIndexBuffer = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::index, tga::memoryAccess(mesh.indexBuffer), mesh.indexBuffer.size() * sizeof(glm::uint32_t)});

        this->meshUniformBuffer = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(camController->position), sizeof(vec3)});

        mat4 mat = mat4(1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, -0.8, -1.6, 1);
        this->matrixBuffer = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(mat), sizeof(mat4)});
        // mesh for gatling

        //Gatling
        for(tga::Vertex vertex: this->meshGunGatling.vertexBuffer) {
            vertex.position += vec3(3.0, 0.0, 0.0);
        }

        //Base Gatling
        for(tga::Vertex vertex: this->meshGunGatlingBase.vertexBuffer) {
            vertex.position += vec3(3.0, 0.0, 0.0);
        }

        for(tga::Vertex vertex: this->meshGunPlasma.vertexBuffer) {
            vertex.position += vec3(-3.0, 0.0, 0.0);
        }

        //Plasma Base
        for(tga::Vertex vertex: this->meshGunPlasmaBase.vertexBuffer) {
            vertex.position += vec3(-3.0, 0.0, 0.0);
        }



        this->meshVertexBufferGatling = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::vertex, tga::memoryAccess(this->meshGunGatling.vertexBuffer), this->meshGunGatling.vertexBuffer.size() * sizeof(tga::Vertex)});

        this->meshIndexBufferGatling = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::index, tga::memoryAccess(meshGunGatling.indexBuffer), meshGunGatling.indexBuffer.size() * sizeof(glm::uint32_t)});

        this->meshUniformBufferGatling = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(camController->position), sizeof(vec3)});

        mat4 mat2 = mat4(1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, 0,
                         2.6, -0.2, -2, 1);
        this->matrixBufferGatling = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(mat2), sizeof(mat4)});



        this->meshVertexBufferGatlingBase = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::vertex, tga::memoryAccess(this->meshGunGatlingBase.vertexBuffer), this->meshGunGatlingBase.vertexBuffer.size() * sizeof(tga::Vertex)});

        this->meshIndexBufferGatlingBase = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::index, tga::memoryAccess(meshGunGatlingBase.indexBuffer), meshGunGatlingBase.indexBuffer.size() * sizeof(glm::uint32_t)});

        this->meshUniformBufferGatlingBase = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(camController->position), sizeof(vec3)});





        this->meshVertexBufferPlasma = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::vertex, tga::memoryAccess(this->meshGunPlasma.vertexBuffer), this->meshGunPlasma.vertexBuffer.size() * sizeof(tga::Vertex)});

        this->meshIndexBufferPlasma = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::index, tga::memoryAccess(meshGunPlasma.indexBuffer), meshGunPlasma.indexBuffer.size() * sizeof(glm::uint32_t)});

        this->meshUniformBufferPlasma = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(camController->position), sizeof(vec3)});

        mat4 mat3 = mat4(1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, 0,
                         -2.6, -0.2, -2, 1);
        this->matrixBufferPlasma = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(mat3), sizeof(mat4)});


        this->meshVertexBufferPlasmaBase = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::vertex, tga::memoryAccess(this->meshGunPlasmaBase.vertexBuffer), this->meshGunPlasmaBase.vertexBuffer.size() * sizeof(tga::Vertex)});

        this->meshIndexBufferPlasmaBase = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::index, tga::memoryAccess(meshGunPlasmaBase.indexBuffer), meshGunPlasmaBase.indexBuffer.size() * sizeof(glm::uint32_t)});

        this->meshUniformBufferPlasmaBase = tgai->createBuffer(
            tga::BufferInfo{tga::BufferUsage::uniform, tga::memoryAccess(camController->position), sizeof(vec3)});


        // textures for cockpit
        tga::Texture texture_dif =
            loadTexture(
                "resources/Cockpit/cockpit/cockpit_diffuse.png", tga::Format::r32g32b32a32_sfloat, tga::SamplerMode::linear, tgai);
        tga::Texture texture_em =
            loadTexture(
                "resources/Cockpit/cockpit/cockpit_emission.png", tga::Format::r32g32b32a32_sfloat,
                tga::SamplerMode::linear, tgai);
        tga::Texture texture_spec =
            loadTexture(
                "resources/Cockpit/cockpit/cockpit_specular.png", tga::Format::r32g32b32a32_sfloat,
                tga::SamplerMode::linear, tgai);
        meshInputSet = tgai->createInputSet({meshPass, 1, {{texture_dif, 0}, {texture_em, 1}, {texture_spec, 2} ,{meshUniformBuffer, 3}, {matrixBuffer, 4}}});

        // textures for gatling
        tga::Texture texture_dif_gatling =
            loadTexture(
                "resources/Cockpit/gatling_gun/gatling_gun_diffuse.png", tga::Format::r32g32b32a32_sfloat, tga::SamplerMode::linear, tgai);
        tga::Texture texture_em_gatling =
            loadTexture(
                "resources/Cockpit/gatling_gun/gatling_gun_emission.png", tga::Format::r32g32b32a32_sfloat,
                tga::SamplerMode::linear, tgai);
        tga::Texture texture_spec_gatling =
            loadTexture(
                "resources/Cockpit/gatling_gun/gatling_gun_specular.png", tga::Format::r32g32b32a32_sfloat,
                tga::SamplerMode::linear, tgai);
        meshInputSetGatling = tgai->createInputSet({meshPass, 1, {{texture_dif_gatling, 0}, {texture_em_gatling, 1}, {texture_spec_gatling, 2} ,{meshUniformBufferGatling, 3}, {matrixBufferGatling, 4}}});


        //textures for plasma
        tga::Texture texture_dif_plasma =
            loadTexture(
                "resources/Cockpit/plasma_gun/plasma_gun_diffuse.png", tga::Format::r32g32b32a32_sfloat, tga::SamplerMode::linear, tgai);
        tga::Texture texture_em_plasma =
            loadTexture(
                "resources/Cockpit/plasma_gun/plasma_gun_emission.png", tga::Format::r32g32b32a32_sfloat,
                tga::SamplerMode::linear, tgai);
        tga::Texture texture_spec_plasma =
            loadTexture(
                "resources/Cockpit/plasma_gun/plasma_gun_specular.png", tga::Format::r32g32b32a32_sfloat,
                tga::SamplerMode::linear, tgai);
        meshInputSetPlasma = tgai->createInputSet({meshPass, 1, {{texture_dif_plasma, 0}, {texture_em_plasma, 1}, {texture_spec_plasma, 2} ,{meshUniformBufferPlasma, 3}, {matrixBufferPlasma, 4}}});



        // Shaders get backed into the renderpass, we don't need the modules anymore, so free them
        tgai->free(meshVS);
        tgai->free(meshFS);
    }

    void OnCreate() override
    {
        createBackgroundResources();
        createTerrainResources();
        createEnemyResources();
        createMeshResources();
        this->camController->position = glm::vec3(0.0, 14.0f, 28.0f);
//        this->camController->position = glm::vec3(0.0, 5.0f, 5.0f);
    }

    void OnFixedUpdate() override {}

    glm::vec3 generateTranslationVector(float alpha) {
        return terrainCenter + terrainRadius * vec3(sin(alpha), 1, cos(alpha));
    }

    void OnUpdate(uint32_t backbufferIndex) override
    {

        for(int i = 0; i < 6; i++) {
            float distance = length(vec3(transformations[i][3][0], transformations[i][3][1], transformations[i][3][2]) - terrainCenter);
            if(distance > terrainRadius + 5){
                transformations[i] = glm::translate(glm::mat4(1.0f), generateTranslationVector(i)) *
                                     glm::scale(glm::mat4(1.0f), glm::vec3(0.002, 0.002, 0.002)) *
                                     glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(1, 0, 0)) *
                                     glm::rotate(glm::mat4(1.0f), -1.14f, glm::vec3(0, 1, 0));

            }
            else{
                speeds[i] = vec4(camController->position, 0) - transformations[i][3];

                mat4 translation = glm::translate(glm::mat4(1.0f), vec3(speeds[i].x * (deltaTime/10), speeds[i].y * (deltaTime/10), speeds[i].z * (deltaTime/10)));
                transformations[i] = translation *  transformations[i];
            }
        }




        vector<mat4x4> newTransformations;

        vec4 normal = normalize(this->camController->getCamera().view[2]);
        int counter = 6;
        int lastIndex = 5;
        for(int i = 0; i < 6; i++) {
            mat4 translation = glm::translate(glm::mat4(1.0f), vec3(speeds[i].x * (deltaTime/10), speeds[i].y * (deltaTime/10), speeds[i].z * (deltaTime/10)));
            boundingSpheres[i].center = translation * boundingSpheres[i].center;
            vec4 pp = boundingSpheres[i].center - vec4(this->camController.get()->position, 0);
            float distance = dot(normal, pp);
            if(distance < 0 && boundingSpheres[i].radius < distance){
                counter--;
                mat4 temp = transformations[lastIndex];
                transformations[lastIndex] = transformations[i];
                transformations[i] = temp;
                lastIndex--;
            }
        }
        /*TODO: Update Data here*/
        tgai->beginCommandBuffer(cmdBuffer);


        tgai->setRenderPass(backgroundPass, backbufferIndex);
        tgai->bindInputSet(systemInputSet);
        tgai->draw(3, 0);
        tgai->setRenderPass(enemyPass, backbufferIndex);
        tgai->bindVertexBuffer(this->enemyVertex);
        tgai->bindIndexBuffer(this->enemyIndex);
        tgai->bindInputSet(enemyInputSet);
        tgai->drawIndexed(this->enemy.indexBuffer.size(), this->enemy.indexBuffer[0], 0, counter, 0);

        tgai->updateBuffer(enemyStorage, memoryAccess(transformations), 6 * sizeof(mat4), 0);

        /*TODO: Record you commands here*/
        tgai->setRenderPass(terrainPass, backbufferIndex);
        tgai->bindVertexBuffer(this->vertexBuffer);
        tgai->bindIndexBuffer(this->indexBuffer);
        tgai->bindInputSet(terrainInputSet);
        tgai->drawIndexed(this->index.size(),this->index[0] , 0);

        tgai->setRenderPass(meshPass, backbufferIndex);

        //gatling
        tgai->setRenderPass(meshPass, backbufferIndex);
        tgai->bindVertexBuffer(this->meshVertexBufferGatling);
        tgai->bindIndexBuffer(this->meshIndexBufferGatling);
        tgai->bindInputSet(meshInputSetGatling);
        tgai->drawIndexed(this->meshGunGatling.indexBuffer.size(),this->meshGunGatling.indexBuffer[0], 0);

        tgai->setRenderPass(meshPass, backbufferIndex);
        tgai->bindVertexBuffer(this->meshVertexBufferGatlingBase);
        tgai->bindIndexBuffer(this->meshIndexBufferGatlingBase);
        tgai->bindInputSet(meshInputSetGatling);
        tgai->drawIndexed(this->meshGunGatlingBase.indexBuffer.size(),this->meshGunGatlingBase.indexBuffer[0], 0);


        //plasma
        tgai->setRenderPass(meshPass, backbufferIndex);
        tgai->bindVertexBuffer(this->meshVertexBufferPlasma);
        tgai->bindIndexBuffer(this->meshIndexBufferPlasma);
        tgai->bindInputSet(meshInputSetPlasma);
        tgai->drawIndexed(this->meshGunPlasma.indexBuffer.size(),this->meshGunPlasma.indexBuffer[0], 0);

        tgai->setRenderPass(meshPass, backbufferIndex);
        tgai->bindVertexBuffer(this->meshVertexBufferPlasmaBase);
        tgai->bindIndexBuffer(this->meshIndexBufferPlasmaBase);
        tgai->bindInputSet(meshInputSetPlasma);
        tgai->drawIndexed(this->meshGunPlasmaBase.indexBuffer.size(),this->meshGunPlasmaBase.indexBuffer[0], 0);


        //cockpit
        tgai->bindVertexBuffer(this->meshVertexBuffer);
        tgai->bindIndexBuffer(this->meshIndexBuffer);
        tgai->bindInputSet(meshInputSet);
        tgai->drawIndexed(this->mesh.indexBuffer.size(),this->mesh.indexBuffer[0], 0);



        cmdBuffer = tgai->endCommandBuffer();
        tgai->execute(cmdBuffer);
    }
    void OnDestroy() override {}


    glm::vec4 speeds[6];
    tga::CommandBuffer cmdBuffer;
    tga::InputSet systemInputSet;
    tga::RenderPass backgroundPass;

    tga::RenderPass terrainPass;
    tga::RenderPass meshPass;
    tga::InputSet terrainInputSet;

    tga::RenderPass enemyPass;
    tga::InputSet enemyInputSet;
    tga::Buffer enemyStorage;
    glm::mat4x4 transformations[6];
    BoundingSphere boundingSpheres[6];

};

int main()
{
    try {
        Game game;
        game.run({0, 0}, tga::PresentMode::immediate);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}