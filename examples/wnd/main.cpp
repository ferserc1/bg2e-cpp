

#include <bg2wnd/application.hpp>
#include <bg2wnd/window.hpp>
#include <bg2wnd/window_delegate.hpp>
#include <bg2render/vk_instance.hpp>
#include <bg2render/vk_device.hpp>
#include <bg2render/swap_chain.hpp>
#include <bg2render/renderer.hpp>
#include <bg2math/vector.hpp>
#include <bg2math/utils.hpp>
#include <bg2db/buffer_load.hpp>
#include <bg2render/pipeline.hpp>
#include <bg2render/vertex_buffer.hpp>
#include <bg2render/index_buffer.hpp>
#include <bg2render/buffer_utils.hpp>
#include <bg2render/vk_descriptor_pool.hpp>
#include <bg2render/single_time_command_buffer.hpp>
#include <bg2render/vk_image_view.hpp>
#include <bg2render/vk_sampler.hpp>
#include <bg2render/vk_image.hpp>
#include <bg2render/texture.hpp>
#include <bg2render/drawable_item.hpp>
#include <bg2render/drawable_descriptor.hpp>
#include <bg2math/matrix.hpp>
#include <bg2base/image.hpp>
#include <bg2db/image_load.hpp>

#include <iostream>
#include <array>
#include <chrono>

class MyWindowDelegate : public bg2wnd::WindowDelegate {
public:
	MyWindowDelegate() {}

	void init() {
        std::cout << window()->title() << ": Init" << std::endl;
    }

    void resize(const bg2math::int2 & size) {
        std::cout << window()->title() << ": Resize (" << size.width() << "," << size.height() << ")" << std::endl;
    }

    void update(float delta) {
        std::cout << window()->title() << ": Update (" << delta * 1000.0f << "ms)" << std::endl;
    }

    void draw() {
        std::cout << window()->title() << ": Draw" << std::endl;
    }

    void cleanup() {
        std::cout << window()->title() << ": Cleanup" << std::endl;
    }

    void keyUp(const bg2wnd::KeyboardEvent & e) {}
    void charPress(const bg2wnd::KeyboardEvent & e) {}
    void keyDown(const bg2wnd::KeyboardEvent & e) {}
    void mouseMove(const bg2wnd::MouseEvent & e) {}
    void mouseDown(const bg2wnd::MouseEvent & e) {}
    void mouseUp(const bg2wnd::MouseEvent & e) {}
    void mouseWheel(const bg2wnd::MouseEvent & e) {}
};

int main(int argc, char ** argv) {
	bg2wnd::Application * app = bg2wnd::Application::Get();

    auto window = bg2wnd::Window::New();
    window->setWindowDelegate(new MyWindowDelegate());
    window->setTitle("Window 1");
    window->setPosition({ 40, 40, });
    window->setSize({ 800, 600 });
    app->addWindow(window);

	auto win2 = bg2wnd::Window::New();
    win2->setTitle("Window 2");
    win2->setPosition({ 110, 110 });
    win2->setSize({ 640, 480 });
	win2->setWindowDelegate(new MyWindowDelegate());
	app->addWindow(win2);

    return app->run();
}
