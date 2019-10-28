
#include <iostream>
#include <bx/bx.h>
#include <bx/file.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bgfx/embedded_shader.h>

#include <bg2e/wnd.hpp>
#include <bg2e/utils.hpp>

#include "example_shaders.h"

struct PosColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
{
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeTriList[] =
{
    0, 1, 2, // 0
    1, 3, 2,
    4, 6, 5, // 2
    5, 6, 7,
    0, 2, 4, // 4
    4, 2, 6,
    1, 5, 3, // 6
    5, 7, 3,
    0, 4, 1, // 8
    4, 5, 1,
    2, 3, 6, // 10
    6, 3, 7,
};

class MyEventHandler : public  bg2e::wnd::EventHandler {
public:
        
    void init() {
        std::cout << "Init" << std::endl;
    
        PosColorVertex::init();
        
        _vertexBuffer = bgfx::createVertexBuffer(
            bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
            PosColorVertex::ms_layout);
                
        _indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));
        
        bgfx::RendererType::Enum type = bgfx::getRendererType();

        bgfx::ShaderHandle vsh = bgfx::createEmbeddedShader(_shaders, type, "shaders::basic_vertex");
        bgfx::ShaderHandle fsh = bgfx::createEmbeddedShader(_shaders, type, "shaders::basic_fragment");

        // Create program from shaders.
        _program = bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
    }
    
    static const bgfx::EmbeddedShader _shaders[];
    
    void resize(uint32_t w, uint32_t h) {
        std::cout << "Resize: " << w << ", " << h << std::endl;
        bgfx::setViewRect(window()->viewId(), 0, 0, window()->width(), window()->height());
    }
    
    void update(float delta) {
        auto viewId = window()->viewId();
        
        
        const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };

        // Set view and projection matrix for view 0.
        {
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            auto aspectRatio = static_cast<float>(window()->width()) / static_cast<float>(window()->height());
            bx::mtxProj(proj, 60.0f, aspectRatio, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);
        }
        
        bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303960ff, 1.0f, 0.0f);
        
        bgfx::touch(window()->viewId());
        uint64_t state = 0
            | BGFX_STATE_WRITE_R
            | BGFX_STATE_WRITE_G
            | BGFX_STATE_WRITE_B
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LESS
            | BGFX_STATE_CULL_CW
            | BGFX_STATE_MSAA
            | UINT64_C(0)// triangle list     BGFX_STATE_PT_TRISTRIP
            ;
        
        float mtx[16];
        static float elapsed = 0;
        elapsed += (delta / 1000.0f);
        bx::mtxIdentity(mtx);
        bx::mtxRotateXY(mtx, elapsed, elapsed);
        mtx[12] = 0.0f;
        mtx[13] = 0.0f;
        mtx[14] = 0.0f;
        
        bgfx::setTransform(mtx);
        
        bgfx::setVertexBuffer(window()->viewId(), _vertexBuffer);
        bgfx::setIndexBuffer(_indexBuffer);
        
        bgfx::setState(state);
        
        bgfx::submit(window()->viewId(), _program);
    }
    
    void draw() {
        bgfx::dbgTextClear();
        //bgfx::dbgTextImage(bx::max<uint16_t>(uint16_t(width / 2 / 8), 20) - 20, bx::max<uint16_t>(uint16_t(height / 2 / 16), 6) - 6, 40, 12, s_logo, 160);
        bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
        bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");
        bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
        bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
        const bgfx::Stats* stats = bgfx::getStats();
        bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
        // Enable stats or debug text.
        bgfx::setDebug(_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
        
        bgfx::frame();
    }
    
    void destroy() {
        std::cout << "Destroy" << std::endl;
        
        // TODO: Destroy resources
        
    }
    
    void keyUp(const bg2e::wnd::KeyboardEvent & evt) {
        if (evt.keyCode() == bg2e::wnd::KeyboardEvent::KeyF1) {
            _showStats = !_showStats;
        }
        std::cout << "Key up" << std::endl;
    }
    
protected:
    bool _showStats = false;
    
    bgfx::VertexBufferHandle _vertexBuffer;
    bgfx::IndexBufferHandle _indexBuffer;
    bgfx::ProgramHandle _program;
    
    bx::DefaultAllocator _allocator;
    bx::FileReaderI * _fileReader;
};

const bgfx::EmbeddedShader MyEventHandler::_shaders[] = {
   BGFX_EMBEDDED_SHADER(shaders::basic_vertex),
   BGFX_EMBEDDED_SHADER(shaders::basic_fragment),
   
   BGFX_EMBEDDED_SHADER_END()
};

int main(int argc, char ** argv) {
    // Create the main loop to initialize the window system
    bg2e::wnd::MainLoop mainLoop;
    
    
    bg2e::wnd::Window * win = new bg2e::wnd::Window();
    win->registerEventHandler(new MyEventHandler());
    win->create(1024,768,"Hello world");
    
    mainLoop.registerWindow(win);
    
    return mainLoop.run();
}
 
