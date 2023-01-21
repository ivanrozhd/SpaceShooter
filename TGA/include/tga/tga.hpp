#pragma once
#include <algorithm>
#include <any>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "tga_core.h"
#include "tga_input.hpp"

namespace tga
{
    // Type Safe Handles

    /** \brief A Shader represents code to be executed on the GPU.
     */
    struct Shader {
        Shader() : handle(TGA_NULL_HANDLE) {}
        Shader(std::nullptr_t) : handle(TGA_NULL_HANDLE) {}
        Shader(TgaShader tgaShader) : handle(tgaShader) {}
        Shader &operator=(TgaShader tgaShader)
        {
            handle = tgaShader;
            return *this;
        }
        operator TgaShader() const { return handle; }
        explicit operator bool() const { return handle != TGA_NULL_HANDLE; }
        bool operator!() const { return handle == TGA_NULL_HANDLE; }

    private:
        TgaShader handle;
    };

    /** \brief A Buffer represents a chunk of memory on the GPU.
     */
    struct Buffer {
        Buffer() : handle(TGA_NULL_HANDLE) {}
        Buffer(std::nullptr_t) : handle(TGA_NULL_HANDLE) {}
        Buffer(TgaBuffer tgaBuffer) : handle(tgaBuffer) {}
        Buffer &operator=(TgaBuffer tgaBuffer)
        {
            handle = tgaBuffer;
            return *this;
        }
        operator TgaBuffer() const { return handle; }
        explicit operator bool() const { return handle != TGA_NULL_HANDLE; }
        bool operator!() const { return handle == TGA_NULL_HANDLE; }

    private:
        TgaBuffer handle;
    };

    /** \brief A Texture represents an image that is stored on and used by the GPU.
     */
    struct Texture {
        Texture() : handle(TGA_NULL_HANDLE) {}
        Texture(std::nullptr_t) : handle(TGA_NULL_HANDLE) {}
        Texture(TgaTexture tgaTexture) : handle(tgaTexture) {}
        Texture &operator=(TgaTexture tgaTexture)
        {
            handle = tgaTexture;
            return *this;
        }
        operator TgaTexture() const { return handle; }
        explicit operator bool() const { return handle != TGA_NULL_HANDLE; }
        bool operator!() const { return handle == TGA_NULL_HANDLE; }

    private:
        TgaTexture handle;
    };

    /** \brief A Window is used to present the result of a fragment shader to the screen.
     */
    struct Window {
        Window() : handle(TGA_NULL_HANDLE) {}
        Window(std::nullptr_t) : handle(TGA_NULL_HANDLE) {}
        Window(TgaWindow tgaWindow) : handle(tgaWindow) {}
        Window &operator=(TgaWindow tgaWindow)
        {
            handle = tgaWindow;
            return *this;
        }
        operator TgaWindow() const { return handle; }
        explicit operator bool() const { return handle != TGA_NULL_HANDLE; }
        bool operator!() const { return handle == TGA_NULL_HANDLE; }

    private:
        TgaWindow handle;
    };

    /** \brief An InputSet is a collection of Bindings and a Binding is a resource used in a Shader.
     */
    struct InputSet {
        InputSet() : handle(TGA_NULL_HANDLE) {}
        InputSet(std::nullptr_t) : handle(TGA_NULL_HANDLE) {}
        InputSet(TgaInputSet tgaInputSet) : handle(tgaInputSet) {}
        InputSet &operator=(TgaInputSet tgaInputSet)
        {
            handle = tgaInputSet;
            return *this;
        }
        operator TgaInputSet() const { return handle; }
        explicit operator bool() const { return handle != TGA_NULL_HANDLE; }
        bool operator!() const { return handle == TGA_NULL_HANDLE; }

    private:
        TgaInputSet handle;
    };

    /** \brief A RenderPass describes a configuration of the graphics-pipeline.
     */
    struct RenderPass {
        RenderPass() : handle(TGA_NULL_HANDLE) {}
        RenderPass(std::nullptr_t) : handle(TGA_NULL_HANDLE) {}
        RenderPass(TgaRenderPass tgaRenderPass) : handle(tgaRenderPass) {}
        RenderPass &operator=(TgaRenderPass tgaRenderPass)
        {
            handle = tgaRenderPass;
            return *this;
        }
        operator TgaRenderPass() const { return handle; }
        explicit operator bool() const { return handle != TGA_NULL_HANDLE; }
        bool operator!() const { return handle == TGA_NULL_HANDLE; }

    private:
        TgaRenderPass handle;
    };

    /** \brief A CommandBuffer is a list of instructions to be executed by the GPU.
     */
    struct CommandBuffer {
        CommandBuffer() : handle(TGA_NULL_HANDLE) {}
        CommandBuffer(std::nullptr_t) : handle(TGA_NULL_HANDLE) {}
        CommandBuffer(TgaCommandBuffer tgaCommandBuffer) : handle(tgaCommandBuffer) {}
        CommandBuffer &operator=(TgaCommandBuffer tgaCommandBuffer)
        {
            handle = tgaCommandBuffer;
            return *this;
        }
        operator TgaCommandBuffer() const { return handle; }
        explicit operator bool() const { return handle != TGA_NULL_HANDLE; }
        bool operator!() const { return handle == TGA_NULL_HANDLE; }

    private:
        TgaCommandBuffer handle;
    };

    // enum classes
    enum class ShaderType { vertex, fragment, compute };

    enum class BufferUsage : uint32_t { undefined = 0x0, uniform = 0x1, vertex = 0x2, index = 0x4, storage = 0x8 };
    inline BufferUsage operator|(BufferUsage a, BufferUsage b)
    {
        return static_cast<BufferUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline bool operator&(BufferUsage a, BufferUsage b)
    {
        return bool(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    // _ is needed because enum types aren't allowed to start with a number
    enum class TextureType { _2D, _2DArray, _3D, _Cube };

    enum class SamplerMode { nearest, linear };

    enum class AddressMode { clampBorder, clampEdge, repeat, repeatMirrow };

    enum class PresentMode {
        immediate,
        vsync,
    };

    enum class Format {
        undefined,
        r8_uint,
        r8_sint,
        r8_srgb,
        r8_unorm,
        r8_snorm,
        r8g8_uint,
        r8g8_sint,
        r8g8_srgb,
        r8g8_unorm,
        r8g8_snorm,
        r8g8b8_uint,
        r8g8b8_sint,
        r8g8b8_srgb,
        r8g8b8_unorm,
        r8g8b8_snorm,
        r8g8b8a8_uint,
        r8g8b8a8_sint,
        r8g8b8a8_srgb,
        r8g8b8a8_unorm,
        r8g8b8a8_snorm,
        r32_uint,
        r32_sint,
        r32_sfloat,
        r32g32_uint,
        r32g32_sint,
        r32g32_sfloat,
        r32g32b32_uint,
        r32g32b32_sint,
        r32g32b32_sfloat,
        r32g32b32a32_uint,
        r32g32b32a32_sint,
        r32g32b32a32_sfloat
    };

    enum class CompareOperation { ignore, equal, greater, greaterEqual, less, lessEqual };

    enum class BlendFactor { zero, one, srcAlpha, oneMinusSrcAlpha, dstAlpha, oneMinusDstAlpha };

    enum class ClearOperation { none, color, depth, all };

    enum class BindingType { uniformBuffer, sampler, storageBuffer };

    enum class CullMode { none, front, back, all };

    enum class FrontFace { clockwise, counterclockwise };

    enum class PolygonMode { solid, wireframe };

    // Special Input Structs
    struct VertexAttribute {
        size_t offset;
        Format format;
        VertexAttribute(size_t _offset = 0, Format _format = Format::undefined) : offset(_offset), format(_format) {}
    };

    struct VertexLayout {
        size_t vertexSize;
        std::vector<VertexAttribute> vertexAttributes;
        VertexLayout(size_t _vertexSize = 0,
                     std::vector<VertexAttribute> const &_vertexAttributes = std::vector<VertexAttribute>())
            : vertexSize(_vertexSize), vertexAttributes(_vertexAttributes)
        {}
    };

    struct RasterizerConfig {
        FrontFace frontFace;
        CullMode cullMode;
        PolygonMode polygonMode;
        RasterizerConfig(FrontFace _frontFace = FrontFace::clockwise, CullMode _cullMode = CullMode::none,
                         PolygonMode _polygonMode = PolygonMode::solid)
            : frontFace(_frontFace), cullMode(_cullMode), polygonMode(_polygonMode)
        {}
    };

    struct PerPixelOperations {
        CompareOperation depthCompareOp;
        bool blendEnabled;
        BlendFactor srcBlend;
        BlendFactor dstBlend;
        BlendFactor srcAlphaBlend;
        BlendFactor dstAlphaBlend;
        PerPixelOperations(CompareOperation _depthCompareOp = CompareOperation::ignore, bool _blendEnabled = false,
                           BlendFactor _srcBlend = BlendFactor::srcAlpha,
                           BlendFactor _dstBlend = BlendFactor::oneMinusSrcAlpha,
                           BlendFactor _srcAlphaBlend = BlendFactor::one,
                           BlendFactor _dstAlphaBlend = BlendFactor::oneMinusSrcAlpha)
            : depthCompareOp(_depthCompareOp), blendEnabled(_blendEnabled), srcBlend(_srcBlend), dstBlend(_dstBlend),
              srcAlphaBlend(_srcAlphaBlend), dstAlphaBlend(_dstAlphaBlend)
        {}
    };

    struct BindingLayout {
        BindingType type;
        uint32_t count;
        BindingLayout(BindingType _type, uint32_t _count = 1) : type(_type), count(_count) {}
    };

    struct SetLayout {
        std::vector<BindingLayout> bindingLayouts;
        SetLayout(const std::vector<BindingLayout> &_bindingLayouts = {}) : bindingLayouts(_bindingLayouts) {}
        SetLayout(std::initializer_list<BindingLayout> _bindingLayouts) : bindingLayouts(_bindingLayouts){};
    };

    struct InputLayout {
        std::vector<SetLayout> setLayouts;
        InputLayout(const std::vector<SetLayout> &_setLayouts = {}) : setLayouts(_setLayouts) {}
    };

    struct Binding {
        std::variant<Buffer, Texture> resource;
        uint32_t slot;
        uint32_t arrayElement;
        Binding(std::variant<Buffer, Texture> _resource = Buffer(), uint32_t _slot = 0, uint32_t _arrayElement = 0)
            : resource(_resource), slot(_slot), arrayElement(_arrayElement)
        {}
    };

    // CreationInfos
    struct ShaderInfo {
        ShaderType type; /**<Type of Shader. Valid Types are ShaderType::vertex and ShaderType::fragment*/
        uint8_t const
            *src; /**<Pointer to the shader code. Dependant on the underlying API. For Vulkan this would be SPIR-V*/
        size_t srcSize; /**<Size of the shader code in bytes*/
        ShaderInfo(ShaderType _type, uint8_t const *_src, size_t _srcSize) : type(_type), src(_src), srcSize(_srcSize)
        {}
        ShaderInfo(ShaderType _type, std::vector<uint8_t> const &_src)
            : type(_type), src(_src.data()), srcSize(_src.size())
        {}
    };
    struct BufferInfo {
        BufferUsage usage; /**<Usage flags of the Buffer. Valid Usage flags are BufferUsage::uniform,
                              BufferUsage::vertex and BufferUsage::index. Others are work in progress*/
        uint8_t const
            *data;       /**<Data of the Buffer to be uploaded. Alignment requirements are the users responsibility*/
        size_t dataSize; /**<Size of the buffer data in bytes*/
        BufferInfo(BufferUsage _usage = BufferUsage::undefined, uint8_t const *_data = nullptr, size_t _dataSize = 0)
            : usage(_usage), data(_data), dataSize(_dataSize)
        {}
        BufferInfo(BufferUsage _usage = BufferUsage::undefined,
                   std::vector<uint8_t> const &_data = std::vector<uint8_t>())
            : usage(_usage), data(_data.data()), dataSize(_data.size())
        {}
    };
    struct TextureInfo {
        uint32_t width;      /**<Width of the Texture in pixels*/
        uint32_t height;     /**<Height of the Texture in pixels*/
        Format format;       /**<Format of the pixels. Example: For 8 bit per Pixel with red, green and blue channel use
                                Format::r8g8b8_unorm. For a list of all formats refer to tga::Format*/
        uint8_t const *data; /**<Data of the Texture. Pass a nullptr to create a texture with undefined content*/
        size_t dataSize;     /**<Size of the texture data in bytes*/
        SamplerMode samplerMode; /**<How the Texture is sampled. Valid SamplerModes are SamplerMode::nearest (default)
                                    and   SamplerMode::linear*/
        AddressMode addressMode; /**<How textures reads with uv-coordinates outside of [0:1] are handled. For a list of
                                    all repeat modes refer to tga::AddressMode*/
        TextureType textureType; /**<Type of the texture, by default 2D*/
        uint32_t depthLayers; /**<If texture type is not 2D, this describes the third dimension of the image. Must be 6
                                 for Cube */
        TextureInfo(uint32_t _width, uint32_t _height, Format _format, uint8_t const *_data, size_t _dataSize,
                    SamplerMode _samplerMode = SamplerMode::nearest,
                    AddressMode _repeateMode = AddressMode::clampBorder, TextureType _textureType = TextureType::_2D,
                    uint32_t _depthLayers = 1)
            : width(_width), height(_height), format(_format), data(_data), dataSize(_dataSize),
              samplerMode(_samplerMode), addressMode(_repeateMode), textureType(_textureType), depthLayers(_depthLayers)
        {}

        TextureInfo(uint32_t _width, uint32_t _height, Format _format,
                    std::vector<uint8_t> const &_data = std::vector<uint8_t>(),
                    SamplerMode _samplerMode = SamplerMode::nearest,
                    AddressMode _repeateMode = AddressMode::clampBorder, TextureType _textureType = TextureType::_2D,
                    uint32_t _depthLayers = 1)
            : width(_width), height(_height), format(_format), data(_data.data()), dataSize(_data.size()),
              samplerMode(_samplerMode), addressMode(_repeateMode), textureType(_textureType), depthLayers(_depthLayers)
        {}
    };
    struct WindowInfo {
        uint32_t width;            /**<Width of the Window in pixels*/
        uint32_t height;           /**<Height of the Window in pixels*/
        PresentMode presentMode;   /**<How syncronization to the monitor is handled. Valid PresentModes are
                                      PresentMode::immediate (show frame as fast as possible, default) and
                                      PresentMode::vsync (sync to the monitor refresh rate)*/
        uint32_t framebufferCount; /**<How many backbuffers the window has to manage. Due to minimum and maximum
                                      contraints this value may not be the actual resulting number of backbuffers and
                                      needs to be polled later*/
        WindowInfo(uint32_t _width, uint32_t _height, PresentMode _presentMode = PresentMode::immediate,
                   uint32_t _framebufferCount = 0)
            : width(_width), height(_height), presentMode(_presentMode), framebufferCount(_framebufferCount)
        {}
    };

    struct InputSetInfo {
        RenderPass targetRenderPass;   /**<The RenderPass this InputSet should be used with*/
        uint32_t setIndex;             /**<The Index of this InputSet as defined in RenderPass.inputLayout*/
        std::vector<Binding> bindings; /**<The collection of Bindings in this InputSet*/
        InputSetInfo(RenderPass _targetRenderPass, uint32_t _setIndex, std::vector<Binding> const &_bindings)
            : targetRenderPass(_targetRenderPass), setIndex(_setIndex), bindings(_bindings)
        {}
    };

    struct RenderPassInfo {
        std::vector<Shader>
            shaderStages; /**<The Shaders to be executed in this RenderPass. Must be ordererd in accordance with the
                             shader stages of the graphics pipeline (i.e vertex before fragment, no duplicate stages,
                             etc.). If using a compute shader it has to be the only stader stage*/
        std::variant<Texture, Window>
            renderTarget; /**<Where the result of the fragment shader stage will be saved. Keep in mind that a Window
                             can have several framebuffers and only one is written at a time*/
        ClearOperation clearOperations; /**<Determines if the renderTarget and/or depth-buffer should be cleared*/
        RasterizerConfig
            rasterizerConfig; /**<Describes the configuration the Rasterizer, i.e culling and polygon draw mode*/
        PerPixelOperations perPixelOperations; /**<Describes operations on each sample, i.e depth-buffer and blending*/
        InputLayout inputLayout;               /**<Describes how the Bindings are organized*/
        VertexLayout vertexLayout;             /**<Describes the format of the vertices in the vertex-buffer*/
        RenderPassInfo(std::vector<Shader> const &_shaderStages, std::variant<Texture, Window> _renderTarget,
                       ClearOperation _clearOperations = ClearOperation::none,
                       RasterizerConfig _rasterizerConfig = RasterizerConfig(),
                       PerPixelOperations _perPixelOperations = PerPixelOperations(),
                       InputLayout _inputLayout = InputLayout(), VertexLayout _vertexLayout = VertexLayout())
            : shaderStages(_shaderStages), renderTarget(_renderTarget), clearOperations(_clearOperations),
              rasterizerConfig(_rasterizerConfig), perPixelOperations(_perPixelOperations), inputLayout(_inputLayout),
              vertexLayout(_vertexLayout)
        {}
    };
    struct CommandBufferInfo {
        CommandBufferInfo() {}
    };

    /** \brief The abstract Interface to a Graphics API
     *
     */
    class Interface {
    public:
        // Resource Creation
        virtual ~Interface() = default;
        virtual Shader createShader(const ShaderInfo &shaderInfo) = 0;
        virtual Buffer createBuffer(const BufferInfo &bufferInfo) = 0;
        virtual Texture createTexture(const TextureInfo &textureInfo) = 0;
        virtual Window createWindow(const WindowInfo &windowInfo) = 0;
        virtual InputSet createInputSet(const InputSetInfo &inputSetInfo) = 0;
        virtual RenderPass createRenderPass(const RenderPassInfo &renderPassInfo) = 0;

        // Commands
        virtual void beginCommandBuffer() = 0;
        virtual void beginCommandBuffer(CommandBuffer cmdBuffer) = 0;
        virtual void setRenderPass(RenderPass renderPass, uint32_t framebufferIndex) = 0;
        virtual void bindVertexBuffer(Buffer buffer) = 0;
        virtual void bindIndexBuffer(Buffer buffer) = 0;
        virtual void bindInputSet(InputSet inputSet) = 0;
        virtual void draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount = 1,
                          uint32_t firstInstance = 0) = 0;
        virtual void drawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t vertexOffset,
                                 uint32_t instanceCount = 1, uint32_t firstInstance = 0) = 0;
        virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;
        virtual CommandBuffer endCommandBuffer() = 0;
        virtual void execute(CommandBuffer commandBuffer) = 0;

        virtual void updateBuffer(Buffer buffer, uint8_t const *data, size_t dataSize, uint32_t offset) = 0;
        virtual std::vector<uint8_t> readback(Buffer buffer) = 0;
        virtual std::vector<uint8_t> readback(Texture texture) = 0;

        // Window functions

        /** \brief Number of framebuffers used by a window.
         * \return Number of framebuffers used as backbuffers by a window
         */
        virtual uint32_t backbufferCount(Window window) = 0;

        /** \brief Index of the next available framebuffer & polling of events.
         * \return Index of next available framebuffer
         */
        virtual uint32_t nextFrame(Window window) = 0;

        /** \brief Polling of events.
         */
        virtual void pollEvents(Window window) = 0;

        /** \brief Shows the last acquired framebuffer on screen.
         */
        virtual void present(Window window) = 0;

        /** \brief Changes title of window.
         * \param title New title of window
         */
        virtual void setWindowTitle(Window window, const std::string &title) = 0;

        /** \brief True if user has issued a close command (pressed x) on the window.
         */
        virtual bool windowShouldClose(Window window) = 0;

        /** \brief True if a key from keyboard or mouse was pressed during the last event poll.
         * \param key Key code of the mouse or keyboard key
         */
        virtual bool keyDown(Window window, Key key) = 0;

        /** \brief x and y position of mouse in pixel coordinates relative to window
         * \return pair (x,y) in pixel coordinates
         */
        virtual std::pair<int, int> mousePosition(Window window) = 0;

        /** \brief Resolution of the primary monitor in pixels
         * \return pair (width,height) in pixel
         */
        virtual std::pair<uint32_t, uint32_t> screenResolution() = 0;

        // Freedom
        virtual void free(Shader shader) = 0;
        virtual void free(Buffer buffer) = 0;
        virtual void free(Texture texture) = 0;
        virtual void free(Window window) = 0;
        virtual void free(InputSet inputSet) = 0;
        virtual void free(RenderPass renderPass) = 0;
        virtual void free(CommandBuffer commandBuffer) = 0;
    };
}  // namespace tga
