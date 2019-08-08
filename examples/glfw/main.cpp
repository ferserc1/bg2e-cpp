

#include <bg2wnd/application.hpp>
#include <bg2wnd/window.hpp>
#include <bg2wnd/window_delegate.hpp>
#include <bg2render/vk_instance.hpp>
#include <bg2render/vk_device.hpp>
#include <bg2render/swap_chain.hpp>
#include <bg2render/renderer.hpp>
#include <bg2math/vector.hpp>
#include <bg2db/buffer_load.hpp>
#include <bg2render/pipeline.hpp>

#include <iostream>

#include <vulkan/vulkan.h>

#include <string.h>


class MyRendererDelegate : public bg2render::RendererDelegate {
public:
	virtual bg2render::Pipeline * configurePipeline(bg2render::vk::Instance* instance, bg2render::SwapChain* swapChain, const bg2math::int2& frameSize) {
		bg2render::Pipeline * pipeline = new bg2render::Pipeline(instance);

		bg2base::path shaderPath = "shaders";
		auto vshader = bg2db::loadBuffer(shaderPath.pathAddingComponent("basic.vert.spv"));
		auto fshader = bg2db::loadBuffer(shaderPath.pathAddingComponent("basic.frag.spv"));
		pipeline->addShader(vshader, VK_SHADER_STAGE_VERTEX_BIT, "main");
		pipeline->addShader(fshader, VK_SHADER_STAGE_FRAGMENT_BIT, "main");

		pipeline->inputAssemblyInfo().topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline->setViewport(frameSize);
		pipeline->rasterizationStateInfo().cullMode = VK_CULL_MODE_BACK_BIT;
		pipeline->rasterizationStateInfo().frontFace = VK_FRONT_FACE_CLOCKWISE;

		pipeline->loadDefaultBlendAttachments();

		pipeline->createDefaultLayout();

		pipeline->createDefaultRenderPass(swapChain->format());

		return pipeline;
	}

	void recordCommandBuffer(float delta, bg2render::vk::CommandBuffer* cmdBuffer, bg2render::Pipeline* pipeline, bg2render::SwapChain* swapChain) {
		cmdBuffer->bindPipeline(pipeline);
		cmdBuffer->draw(
			3,	// vertex count
			1,	// instance count
			0,	// first vertex
			0	// first instance
		);
	}
};

class MyWindowDelegate : public bg2wnd::WindowDelegate {
public:
	MyWindowDelegate(bool enableValidation) :_enableValidationLayers(enableValidation) {}

	void init() {
		_instance = std::shared_ptr<bg2render::vk::Instance>(new bg2render::vk::Instance());
		_instance->configureAppName("bg2e vulkan test");
		if (_enableValidationLayers) {
			_instance->setDebugCallback([](
				VkDebugUtilsMessageSeverityFlagBitsEXT severity,
				VkDebugUtilsMessageTypeFlagsEXT type,
				const VkDebugUtilsMessengerCallbackDataEXT* pData) {
					if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
						std::cout << pData->pMessage << std::endl;
					}
				});
		}

		std::vector<VkExtensionProperties> extensionData;
		_instance->enumerateInstanceExtensionProperties(extensionData);
		std::cout << "Available extensions:" << std::endl;
		for (const auto& ext : extensionData) {
			std::cout << "\t" << ext.extensionName << std::endl;
		}

		std::vector<const char*> extensions;
		window()->getVulkanRequiredInstanceExtensions(extensions);
		_instance->configureRequiredExtensions(extensions);

		_instance->create();

		// It's important to link the window surface to vulkan instance BEFORE
		// choose physical device, if you want to get support for presentation queues
		_instance->setSurface(window()->createVulkanSurface(_instance->instance()));

		_instance->choosePhysicalDevices();


		_renderer = std::make_unique<bg2render::Renderer>(_instance.get());
		_renderer->setDelegate(new MyRendererDelegate());
		_renderer->init(window()->size());
    }


    void resize(const bg2math::int2 & size) {
        std::cout << "resize: " << size.x() << ", " << size.y() << std::endl;
		_renderer->resize(size);
    }

    void update(float delta) {
		_renderer->update(delta);
    }

    void draw() {
		_renderer->draw();
    }

    void cleanup() {
		_renderer = nullptr;
		_instance = nullptr;
    }

    void keyUp(const bg2wnd::KeyboardEvent & e) {}
    void keyDown(const bg2wnd::KeyboardEvent & e) {}
    void mouseMove(const bg2wnd::MouseEvent & e) {}
    void mouseDown(const bg2wnd::MouseEvent & e) {}
    void mouseUp(const bg2wnd::MouseEvent & e) {}
    void mouseWheel(const bg2wnd::MouseEvent & e) {}

private:
	std::shared_ptr<bg2render::vk::Instance> _instance;
	std::unique_ptr<bg2render::Renderer> _renderer;

	bool _enableValidationLayers;
};

int main(int argc, char ** argv) {
    bg2wnd::Application app;

    auto window = bg2wnd::Window::Create();
    window->setWindowDelegate(new MyWindowDelegate(true));
    app.addWindow(window);
    app.run();

    return 0;
}
