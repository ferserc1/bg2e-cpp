

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
#include <bg2math/matrix.hpp>

#include <iostream>
#include <array>
#include <chrono>

class MyRendererDelegate : public bg2render::RendererDelegate {
public:
	struct Vertex {
		bg2math::float2 pos;
		bg2math::float3 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

	std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	VkVertexInputBindingDescription bindingDescription;
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions;

	struct UniformBufferObject {
		bg2math::float4x4 model;
		bg2math::float4x4 view;
		bg2math::float4x4 proj;
	};

	// TODO: Create helper descriptor pool that can auto release it
	VkDescriptorPool _descriptorPool;
	bg2render::vk::Instance * _instance;
	std::vector<VkDescriptorSet> _descriptorSets;

	virtual bg2render::Pipeline * configurePipeline(bg2render::vk::Instance* instance, bg2render::SwapChain* swapChain, const bg2math::int2& frameSize) {
		bg2render::Pipeline * pipeline = new bg2render::Pipeline(instance);

		bg2base::path shaderPath = "shaders";
		auto vshader = bg2db::loadBuffer(shaderPath.pathAddingComponent("basic.vert.spv"));
		auto fshader = bg2db::loadBuffer(shaderPath.pathAddingComponent("basic.frag.spv"));
		pipeline->addShader(vshader, VK_SHADER_STAGE_VERTEX_BIT, "main");
		pipeline->addShader(fshader, VK_SHADER_STAGE_FRAGMENT_BIT, "main");

		// Pipeline layout
		bg2render::vk::PipelineLayout* pipelineLayout = new bg2render::vk::PipelineLayout(instance);
		pipelineLayout->addDescriptorSetLayoutBinding(
			0,	// binding
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			1,	// descriptorCount
			VK_SHADER_STAGE_VERTEX_BIT	// stage flags
		);
		pipelineLayout->create();
		pipeline->setPipelineLayout(pipelineLayout);

		bindingDescription = Vertex::getBindingDescription();
		attributeDescriptions = Vertex::getAttributeDescriptions();

		pipeline->vertexInputInfo().vertexBindingDescriptionCount = 1;
		pipeline->vertexInputInfo().pVertexBindingDescriptions = &bindingDescription;
		pipeline->vertexInputInfo().vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		pipeline->vertexInputInfo().pVertexAttributeDescriptions = attributeDescriptions.data();

		pipeline->inputAssemblyInfo().topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline->setViewport(frameSize);
		pipeline->rasterizationStateInfo().cullMode = VK_CULL_MODE_NONE;
		pipeline->rasterizationStateInfo().frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		pipeline->loadDefaultBlendAttachments();


		pipeline->createDefaultRenderPass(swapChain->format());

		return pipeline;
	}

	virtual void recordCommandBuffer(float delta, bg2render::vk::CommandBuffer* cmdBuffer, bg2render::Pipeline* pipeline, bg2render::SwapChain* swapChain, uint32_t frameIndex) {
		cmdBuffer->bindPipeline(pipeline);
		cmdBuffer->bindVertexBuffer(0, 1, _vertexBuffer);
		cmdBuffer->bindIndexBuffer(_indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(cmdBuffer->commandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout()->pipelineLayout(), 0, 1, &_descriptorSets[frameIndex], 0, nullptr);

		cmdBuffer->drawIndexed(indices.size(), 1, 0, 0, 0);
	}

	virtual void updateUniformBuffers(uint32_t currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo = {};
		ubo.model = bg2math::float4x4::Rotation(time * bg2math::radians(90.0f), 0.0f, 0.0f, 1.0f);
		ubo.view = bg2math::float4x4::LookAt(bg2math::float3(2.0f, 2.0f, 2.0f), bg2math::float3(0.0f, 0.0f, 0.0f), bg2math::float3(0.0f, 0.0f, 1.0f));
		auto extent = renderer()->swapChain()->extent();
		auto ratio = static_cast<float>(extent.width) / static_cast<float>(extent.height);
 		ubo.proj = bg2math::float4x4::Perspective(60.0f, ratio, 0.1f, 100.0f);
		ubo.proj.element(1, 1) *= -1.0;

		void* data;
		_uniformBuffersMemory[currentImage]->map(0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		_uniformBuffersMemory[currentImage]->unmap();
	}

	virtual void initDone(bg2render::vk::Instance * instance) {
		_vertexBuffer = std::make_unique<bg2render::VertexBuffer>(instance);
		_vertexBuffer->create<Vertex>(vertices, renderer()->commandPool());

		_indexBuffer = std::make_unique<bg2render::IndexBuffer>(instance);
		_indexBuffer->create<uint16_t>(indices, renderer()->commandPool());

		// Uniform buffers
		auto simultaneousFrames = renderer()->swapChain()->images().size();
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		_uniformBuffers.resize(simultaneousFrames);
		_uniformBuffersMemory.resize(simultaneousFrames);

		for (size_t i = 0; i < simultaneousFrames; ++i) {
			bg2render::BufferUtils::CreateBufferMemory(
				instance,
				bufferSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				_uniformBuffers[i], _uniformBuffersMemory[i]);
		}
		
		// Descriptor sets
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(simultaneousFrames);
		
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(simultaneousFrames);
		if (vkCreateDescriptorPool(instance->renderDevice()->device(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create descriptor pool");
		}
		_instance = instance;

		std::vector<VkDescriptorSetLayout> layouts(simultaneousFrames, renderer()->pipeline()->pipelineLayout()->descriptorSetLayout());
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(simultaneousFrames);
		allocInfo.pSetLayouts = layouts.data();

		_descriptorSets.resize(simultaneousFrames);
		if (vkAllocateDescriptorSets(instance->renderDevice()->device(), &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate descriptor sets");
		}

		for (size_t i = 0; i < simultaneousFrames; ++i) {
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = _uniformBuffers[i]->buffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = _descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(instance->renderDevice()->device(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	virtual void cleanup() {
		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;
		_uniformBuffers.clear();
		_uniformBuffersMemory.clear();

		vkDestroyDescriptorPool(_instance->renderDevice()->device(), _descriptorPool, nullptr);
	}

private:
	// Vertex buffers
	std::unique_ptr<bg2render::VertexBuffer> _vertexBuffer;
	std::unique_ptr<bg2render::IndexBuffer> _indexBuffer;

	// Uniform buffer
	std::vector<std::unique_ptr<bg2render::vk::Buffer>> _uniformBuffers;
	std::vector<std::unique_ptr<bg2render::vk::DeviceMemory>> _uniformBuffersMemory;

};

class MyWindowDelegate : public bg2wnd::WindowDelegate {
public:
	MyWindowDelegate(bool enableValidation) :_enableValidationLayers(enableValidation) {}

	void init() {
		_instance = std::shared_ptr<bg2render::vk::Instance>(bg2render::vk::Instance::CreateDefault (window(),"bg2 vulkan test"));

		_renderer = std::make_unique<bg2render::Renderer>(_instance.get());
		_renderer->setDelegate(new MyRendererDelegate());
		_renderer->init(window()->size());
    }

    void resize(const bg2math::int2 & size) {
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
