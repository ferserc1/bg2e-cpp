
#include <iostream>

#include <bg2e/bg2e.hpp>

#define RENDER_TEXTURE_PASS 1

class MyEventHandler : public  bg2e::wnd::EventHandler {
public:
        
    void init() {
        bg2e::base::MeshData meshData;
        bg2e::utils::generateCube(2.0f, meshData);
    
        _plist = new bg2e::base::PolyList();
        _plist->build(meshData);
         
       	bg2e::base::path dataPath("data");
		auto diffuse = bg2e::db::loadTexture(dataPath.pathAddingComponent("texture.jpg"));
		//auto normal = bg2e::db::loadTexture(dataPath.pathAddingComponent("texture_nm.jpg"));

		_material = new bg2e::base::Material();
		_material->setDiffuse(diffuse);
		//_material->setNormal(normal);

		_matrixState = new bg2e::base::MatrixState();

		_pipeline = new bg2e::base::Pipeline(window()->viewId());
		_pipeline->setShader(new bg2e::shaders::Phong());
		_pipeline->setClearColor(bg2e::math::color(0x51B868FF));

		_light = new bg2e::base::Light(window()->viewId());
		_light->setPosition(bg2e::math::float3(2.0f, 2.0f, 2.0f));
		_light->setDirection(bg2e::math::float3(-0.5, -0.5, -0.5));
		bg2e::base::Light::ActivateLight(_light);
        
        bgfx::TextureHandle fbtextures[] =
        {
            bgfx::createTexture2D(
                _fbSize.width(),
                _fbSize.height(),
                false,
                1,
                bgfx::TextureFormat::RGBA8,
                BGFX_TEXTURE_RT),
            bgfx::createTexture2D(
                _fbSize.width(),
                _fbSize.height(),
                false,
                1,
                bgfx::TextureFormat::D16,
                BGFX_TEXTURE_RT_WRITE_ONLY),
            
        };
        _fbHandle = bgfx::createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures, true);
    }
        
    void resize(uint32_t w, uint32_t h) {
        bgfx::setViewRect(window()->viewId(), 0, 0, window()->width(), window()->height());
    }
    
    void update(float delta) {
		_matrixState->beginFrame();
		

        // Render texture
        const bg2e::math::float4x4 fbView = bg2e::math::float4x4::Identity();
        const bg2e::math::float4x4 fbProj = bg2e::math::float4x4::Perspective(60.0f, 1.0f, 0.1f, 100.0f);
        bgfx::setViewRect(RENDER_TEXTURE_PASS, 0, 0, _fbSize.width(), _fbSize.height());
        bgfx::setViewFrameBuffer(RENDER_TEXTURE_PASS, _fbHandle);
        bgfx::setViewTransform(RENDER_TEXTURE_PASS, fbView.raw(), fbProj.raw());
        bgfx::setViewClear(RENDER_TEXTURE_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x305089ff, 1.0f, 0);

        const bg2e::math::float3 at = { 0.0f, 0.0f, 0.0f };
        const bg2e::math::float3 eye = { 0.0f, 0.0f, -5.0f };
        const bg2e::math::float3 up = { 0.0f, 1.0f, 0.0f };
        auto aspectRatio = static_cast<float>(window()->width()) / static_cast<float>(window()->height());
		_matrixState->view()
			.identity()
			.lookAt(eye, at, up);
		_matrixState->projection()
			.perspective(60.0f, aspectRatio, 0.1f, 100.0f);
        
		_pipeline->beginDraw(_matrixState.getPtr());

        static float elapsed = 0;
        elapsed += (delta / 1000.0f);
        bg2e::math::float4x4 mtx = bg2e::math::float4x4::Identity();
        mtx.rotate(elapsed, 1.0f, 0.0f, 0.0f)
            .rotate(elapsed * 2.0f, 0.0f, 1.0f, 0.0f);
		_pipeline->draw(_plist.getPtr(), _material.getPtr(), _matrixState->model().matrix(), _matrixState->model().inverseMatrix());
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
		_plist = nullptr;
		_material = nullptr;
		_plist = nullptr;
        
        bgfx::destroy(_fbHandle);
    }
    
    void keyUp(const bg2e::wnd::KeyboardEvent & evt) {
        if (evt.keyCode() == bg2e::wnd::KeyboardEvent::KeyF1) {
            _showStats = !_showStats;
        }
    }
    
protected:
    bool _showStats = false;

	bg2e::ptr<bg2e::base::MatrixState> _matrixState;
    bg2e::ptr<bg2e::base::PolyList> _plist;
	bg2e::ptr<bg2e::base::Material> _material;
	bg2e::ptr<bg2e::base::Pipeline> _pipeline;
	bg2e::ptr<bg2e::base::Light> _light;
    
    // render to texture example
    bg2e::math::int2 _fbSize = bg2e::math::int2(512,512);
    bgfx::FrameBufferHandle _fbHandle = BGFX_INVALID_HANDLE;
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
 
