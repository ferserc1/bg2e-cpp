
#include <bg2render/vk_command_buffer.hpp>

#include <stdexcept>
#include <array>

namespace bg2render {
    namespace vk {
        
		void CommandBuffer::Allocate(Device * dev, VkCommandPool pool, VkCommandBufferLevel level, uint32_t count, std::vector<std::shared_ptr<CommandBuffer>>& result) {
			std::vector<VkCommandBuffer> vkCommandBuffers(count);

			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = pool;
			allocInfo.level = level;
			allocInfo.commandBufferCount = count;
			if (vkAllocateCommandBuffers(dev->device(), &allocInfo, vkCommandBuffers.data()) != VK_SUCCESS) {
				throw std::runtime_error("Failed to allocate command buffers");
			}

			result.clear();
			for (auto cb : vkCommandBuffers) {
				result.push_back(std::make_shared<CommandBuffer>(dev->device(), pool, cb));
			}
		}

		CommandBuffer* CommandBuffer::Allocate(Device* dev, VkCommandPool pool, VkCommandBufferLevel level) {
			std::vector<VkCommandBuffer> vkCommandBuffers(1);

			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = pool;
			allocInfo.level = level;
			allocInfo.commandBufferCount = 1;
			if (vkAllocateCommandBuffers(dev->device(), &allocInfo, vkCommandBuffers.data()) != VK_SUCCESS) {
				throw std::runtime_error("Failed to allocate command buffers");
			}

			return new CommandBuffer(dev->device(), pool, vkCommandBuffers[0]);
		}

		void CommandBuffer::Free(std::vector<std::shared_ptr<CommandBuffer>>& cmdBuffers) {
			std::vector<VkCommandBuffer> vkCommandBuffers;
			VkCommandPool pool = VK_NULL_HANDLE;
			VkDevice dev = VK_NULL_HANDLE;
			for (auto cb : cmdBuffers) {
				if (pool == VK_NULL_HANDLE) {
					pool = cb->_pool;
					dev = cb->_device;
				}
				vkCommandBuffers.push_back(cb->commandBuffer());
			}

			if (pool != VK_NULL_HANDLE) {
				vkFreeCommandBuffers(dev, pool, static_cast<uint32_t>(vkCommandBuffers.size()), vkCommandBuffers.data());
				cmdBuffers.clear();
			}
		}

		void CommandBuffer::Free(CommandBuffer* cmdBuffer) {
			std::vector<VkCommandBuffer> vkCommandBuffers;
			vkCommandBuffers.push_back(cmdBuffer->commandBuffer());
			vkFreeCommandBuffers(cmdBuffer->_device, cmdBuffer->_pool, 1, vkCommandBuffers.data());
		}

		CommandBuffer::CommandBuffer(VkDevice dev, VkCommandPool pool, VkCommandBuffer cmdBuffer)
			:_device(dev)
			,_pool(pool)
			,_commandBuffer(cmdBuffer)
		{

		}

		void CommandBuffer::begin(VkCommandBufferUsageFlags usage) {
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = usage;
			vkBeginCommandBuffer(commandBuffer(), &beginInfo);
		}

		void CommandBuffer::end() {
			vkEndCommandBuffer(commandBuffer());
		}

		void CommandBuffer::beginRenderPass(RenderPass* rp, VkFramebuffer fb, SwapChain* swapChain, const bg2math::color& clearColor, VkSubpassContents subpassContents) {
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = rp->renderPass();
			renderPassInfo.framebuffer = fb;
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChain->extent();
			VkClearValue vkClearColor = { clearColor.x(), clearColor.y(), clearColor.z(), clearColor.w() };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &vkClearColor;
			vkCmdBeginRenderPass(commandBuffer(), &renderPassInfo, subpassContents);
		}

		void CommandBuffer::beginRenderPass(RenderPass* rp, VkFramebuffer fb, SwapChain* swapChain, const bg2math::color& clearColor, float depthClearColor, VkSubpassContents subpassContents) {
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = rp->renderPass();
			renderPassInfo.framebuffer = fb;
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChain->extent();
			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = { clearColor.x(), clearColor.y(), clearColor.z(), clearColor.w() };
			clearValues[1].depthStencil = { depthClearColor, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
			vkCmdBeginRenderPass(commandBuffer(), &renderPassInfo, subpassContents);
		}

		void CommandBuffer::beginRenderPass(RenderPass* rp, VkFramebuffer fb, SwapChain* swapChain, const bg2math::color& clearColor, float depthClearColor, uint32_t stencilClearColor, VkSubpassContents subpassContents) {
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = rp->renderPass();
			renderPassInfo.framebuffer = fb;
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChain->extent();
			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = { clearColor.x(), clearColor.y(), clearColor.z(), clearColor.w() };
			clearValues[1].depthStencil = { depthClearColor, stencilClearColor };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
			vkCmdBeginRenderPass(commandBuffer(), &renderPassInfo, subpassContents);
		}

		void CommandBuffer::endRenderPass() {
			vkCmdEndRenderPass(commandBuffer());
		}

		void CommandBuffer::bindPipeline(Pipeline* pipeline, VkPipelineBindPoint bindPoint) {
			vkCmdBindPipeline(commandBuffer(), bindPoint, pipeline->pipeline());
		}

		void CommandBuffer::bindVertexBuffer(uint32_t firstBinding, uint32_t bindingCount, VertexBuffer* buffer, uint32_t offset) {
			VkBuffer buffers[] = { buffer->buffer()->buffer() };
			VkDeviceSize offsets[] = { offset };
			vkCmdBindVertexBuffers(commandBuffer(), firstBinding, bindingCount, buffers, offsets);
		}

		void CommandBuffer::bindVertexBuffer(uint32_t firstBinding, uint32_t bindingCount, vk::Buffer* buffer, uint32_t offset) {
			VkBuffer buffers[] = { buffer->buffer() };
			VkDeviceSize offsets[] = { offset };
			vkCmdBindVertexBuffers(commandBuffer(), firstBinding, bindingCount, buffers, offsets);
		}

		void CommandBuffer::bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, const std::vector<vk::Buffer*>& buffers, const std::vector<VkDeviceSize>& offsets) {
			std::vector<VkBuffer> vkBuffers;
			for (auto b : buffers) {
				vkBuffers.push_back(b->buffer());
			}
			vkCmdBindVertexBuffers(commandBuffer(), firstBinding, bindingCount, vkBuffers.data(), offsets.data());
		}

		void CommandBuffer::bindIndexBuffer(IndexBuffer* buffer, VkDeviceSize offset, VkIndexType indexType) {
			vkCmdBindIndexBuffer(commandBuffer(), buffer->buffer()->buffer(), offset, indexType);
		}

		void CommandBuffer::bindIndexBuffer(vk::Buffer* buffer, VkDeviceSize offset, VkIndexType indexType) {
			vkCmdBindIndexBuffer(commandBuffer(), buffer->buffer(), offset, indexType);
		}

		void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
			vkCmdDraw(commandBuffer(), vertexCount, instanceCount, firstVertex, firstInstance);
		}

		void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
			vkCmdDrawIndexed(commandBuffer(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}

		void CommandBuffer::copyBuffer(vk::Buffer* src, vk::Buffer* dst, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = src->size();
			vkCmdCopyBuffer(commandBuffer(), src->buffer(), dst->buffer(), 1, &copyRegion);
		}

		void CommandBuffer::bindDescriptorSet(VkPipelineBindPoint bindPoint, PipelineLayout* pl, uint32_t firstSet, DescriptorSet* ds) {
			VkDescriptorSet vkDescSet = ds->descriptorSet();
			vkCmdBindDescriptorSets(commandBuffer(), bindPoint, pl->pipelineLayout(), firstSet, 1, &vkDescSet, 0, nullptr);
		}

		void CommandBuffer::pipelineBarrier(
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			VkDependencyFlags dependencyFlags,
			uint32_t memoryBarrierCount,
			const VkMemoryBarrier* pMemoryBarriers,
			uint32_t bufferMemoryBarrierCount,
			const VkBufferMemoryBarrier* pBufferMemoryBarriers,
			uint32_t imageMemoryBarrierCount,
			const VkImageMemoryBarrier* pImageMemoryBarriers)
		{
			vkCmdPipelineBarrier(
				commandBuffer(),
				srcStageMask, dstStageMask,
				dependencyFlags,
				memoryBarrierCount, pMemoryBarriers,
				bufferMemoryBarrierCount, pBufferMemoryBarriers,
				imageMemoryBarrierCount, pImageMemoryBarriers);
		}
    }
}