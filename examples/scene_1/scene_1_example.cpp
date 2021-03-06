
#include <iostream>

#include <bg2e/bg2e.hpp>


class MyEventHandler : public  bg2e::wnd::EventHandler {
public:

	bg2e::scene::Node * createCube() {
		using namespace bg2e;
		base::MeshData data;
		utils::generateCube(1.0f, data);
		auto plist = new base::PolyList();
		plist->build(data);

		auto drawable = new scene::Drawable();
		drawable->addPolyList(plist);

		auto node = new scene::Node();
		node->addComponent(drawable);
		node->addComponent(new scene::Transform());

		return node;
	}
        
    void init() { 
		_sceneRoot = new bg2e::scene::Node();

       	bg2e::base::path dataPath("data");
		auto diffuse = bg2e::db::loadTexture(dataPath.pathAddingComponent("texture.jpg"));
		auto normal = bg2e::db::loadTexture(dataPath.pathAddingComponent("texture_nm.jpg"));

		
		auto cube1 = createCube();
		cube1->drawable()->material(0)->setDiffuse(diffuse);
		cube1->drawable()->material(0)->setNormal(normal);
		_sceneRoot->addChild(cube1);

		_cubeTransform = cube1->transform();
		_cubeTransform->matrix().rotate(bg2e::math::radians(45.0f), 0.0f, 1.0f, 0.0f);

		auto cube2 = createCube();
		cube2->drawable()->material(0)->setDiffuse(bg2e::math::color(1.0f, 0.5f, 0.5f, 0.4f));
		cube2->drawable()->material(0)->setIsTransparent(true);
		cube2->transform()->matrix()
			.translate(bg2e::math::float3(1.0f, 1.0f, 0.0f))
			.rotate(bg2e::math::degrees(45.0f), 0.0f, 1.0f, 0.0f);
		_sceneRoot->addChild(cube2);
		_matrixState = new bg2e::base::MatrixState();

		_pipeline = new bg2e::base::Pipeline(window()->viewId());
		_pipeline->setShader(new bg2e::shaders::Phong());
		_pipeline->setClearColor(bg2e::math::color(0x51B868FF));

		_light = new bg2e::base::Light(window()->viewId());
		_light->setPosition(bg2e::math::float3(2.0f, 2.0f, 2.0f));
		_light->setDirection(bg2e::math::float3(-0.2f, -0.5, -0.5f));
		bg2e::base::Light::ActivateLight(_light);
        
		

		auto camNode = new bg2e::scene::Node();
		_mainCamera = new bg2e::scene::Camera();
		auto projStrategy = new bg2e::base::OpticalProjectionStrategy();
		projStrategy->setFrameSize(35.0f);
		projStrategy->setFocalLength(55.0f);
		projStrategy->setNear(0.1f);
		projStrategy->setFar(100.0f);
		_mainCamera->camera().setProjectionStrategy(projStrategy);
		camNode->addComponent(_mainCamera.getPtr());
		auto cameraTrx = new bg2e::scene::Transform();
		cameraTrx->matrix().translate({ 0.0f, 0.0f, 10.0f });
		camNode->addComponent(cameraTrx);
		_sceneRoot->addChild(camNode);
		
		_drawVisitor = new bg2e::scene::DrawVisitor();
		_resizeVisitor = new bg2e::scene::ResizeVisitor();
		_updateVisitor = new bg2e::scene::UpdateVisitor();
    }
        
    void resize(uint32_t w, uint32_t h) {
        bgfx::setViewRect(window()->viewId(), 0, 0, window()->width(), window()->height());
		_resizeVisitor->resize(_sceneRoot.getPtr(), w, h);
        //_camera.setViewport({ window()->width(), window()->height() });
    }
    
    void update(float delta) {
		_matrixState->beginFrame();

		static float elapsed = 0;
		elapsed += (delta / 1000.0f);
		bg2e::math::float4x4 mtx = bg2e::math::float4x4::Identity();
		mtx.rotate(elapsed, 1.0f, 0.0f, 0.0f)
			.rotate(elapsed * 2.0f, 0.0f, 1.0f, 0.0f);
        _cubeTransform->matrix() = mtx;


		_updateVisitor->update(_sceneRoot.getPtr(), _matrixState.getPtr(), delta);
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
        

		_pipeline->beginDraw(_matrixState.getPtr());

		_renderQueue.begin(_mainCamera->camera());
		_drawVisitor->draw(_sceneRoot.getPtr(), &_renderQueue, _matrixState.getPtr());


        _pipeline->setDepthTest(bg2e::base::kDepthTestLess);
        _pipeline->setDepthTestEnabled(true);
        _pipeline->setBlendFunction(bg2e::base::kBlendFunctionDisabled, bg2e::base::kBlendFunctionDisabled);
		for (auto & elem : _renderQueue.opaqueQueue()) {
			_pipeline->draw(elem.polyList.getPtr(), elem.material.getPtr(), elem.transform, elem.inverseTransform);
		}
        
        _pipeline->setDepthTest(bg2e::base::kDepthTestLess);
        _pipeline->setDepthTestEnabled(false);
        _pipeline->setBlendFunction(bg2e::base::kBlendFunctionSrcAlpha, bg2e::base::kBlendFunctionOneMinusSrcAlpha);
        for (auto & elem : _renderQueue.transparentQueue()) {
            _pipeline->draw(elem.polyList.getPtr(), elem.material.getPtr(), elem.transform, elem.inverseTransform);
        }

        bgfx::frame();
    }
    
    void destroy() {
		//_plist = nullptr;
		//_material = nullptr;
		
    }
    
    void keyUp(const bg2e::wnd::KeyboardEvent & evt) {
        if (evt.keyCode() == bg2e::wnd::KeyboardEvent::KeyF1) {
            _showStats = !_showStats;
        }
    }
    
protected:
    bool _showStats = false;

	bg2e::ptr<bg2e::base::Pipeline> _pipeline;
	bg2e::ptr<bg2e::base::MatrixState> _matrixState;
	bg2e::ptr<bg2e::base::Light> _light;

	bg2e::ptr<bg2e::scene::Node> _sceneRoot;
	bg2e::ptr<bg2e::scene::Camera> _mainCamera;
    bg2e::ptr<bg2e::scene::Transform> _cubeTransform;

	bg2e::base::RenderQueue _renderQueue;
	bg2e::ptr<bg2e::scene::DrawVisitor> _drawVisitor;
	bg2e::ptr<bg2e::scene::UpdateVisitor> _updateVisitor;
	bg2e::ptr<bg2e::scene::ResizeVisitor> _resizeVisitor;
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
 
