
#include <bg2e/base/pipeline.hpp>

#include <stdexcept>

namespace bg2e {
namespace base {


	Pipeline::Pipeline(bgfx::ViewId viewId)
		:_viewId(viewId)
	{

	}

	Pipeline::~Pipeline() {

	}

	void Pipeline::beginDraw(const math::float4x4 & viewMatrix, const math::float4x4 & projMatrix) {
		if (!_shader.valid()) {
			throw std::runtime_error("Error drawing polyList: no shader configured in pipeline.");
		}

		bgfx::setViewTransform(_viewId, viewMatrix.raw(), projMatrix.raw());

		if (_clearFlags != 0) {
			bgfx::setViewClear(_viewId, _clearFlags, _clearColor.hexColor(), _clearDepth);
		}

		_shader->bindFrameUniforms(this);
	}

	void Pipeline::draw(PolyList* plist, Material* material, const math::float4x4 & modelMatrix, const math::float4x4 & inverseModelMatrix) {
		if (!_shader.valid()) {
			throw std::runtime_error("Error drawing polyList: no shader configured in pipeline.");
		}
		if (!plist || !material) {
			throw std::invalid_argument("Drawing error: invalid material or polyList");
		}

		bgfx::setTransform(modelMatrix.raw());

		// TODO: set state
		uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| _depthTest
            | _blendEquation
			| plist->cullFace()
			| BGFX_STATE_MSAA
			| plist->polygonMode()
			;

		bgfx::setState(state);

		_shader->bindUniforms(this, plist, material, modelMatrix, inverseModelMatrix);

		bgfx::setVertexBuffer(static_cast<uint8_t>(_viewId), plist->vertexBuffer());
		bgfx::setIndexBuffer(plist->indexBuffer());

		bgfx::submit(_viewId, _shader->programHandle());
	}
}
}