
#include <bg2e/scene/camera.hpp>
#include <bg2e/db/json/value.hpp>
#include <bg2e/base/projection_strategy.hpp>
#include <bg2e/scene/transform.hpp>

namespace bg2e {
namespace scene {

	Camera::Camera()
	{

	}

	Camera::~Camera() {

	}

	Component * Camera::clone() {
		ptr<Camera> newCam = new Camera();

		newCam->_camera = _camera;

		return newCam.release();
	}

	void Camera::deserialize(bg2e::db::json::Value * value, const bg2e::base::path &) {
		using namespace bg2e::db::json;
		base::ProjectionStrategy * projectionStrategy = base::ProjectionStrategy::Factory(value);
		if (projectionStrategy) {
			_camera.setProjectionStrategy(projectionStrategy);
		}
		else {
			_camera.setProjection(Value::Float4x4((*value)["projection"]));
		}
	}

	bg2e::db::json::Value * Camera::serialize(const bg2e::base::path &) {
		using namespace bg2e::db::json;
		Value * cameraData = new Value();
		cameraData->setValue("type", "Camera");
		if (_camera.projectionStrategy()) {
			cameraData->setValue("projectionMethod", _camera.projectionStrategy()->serialize());
		}
		else {
			cameraData->setValue("projection", _camera.projection());
		}
		return cameraData;
	}

	void Camera::resize(uint32_t w, uint32_t h) {
		_camera.setViewport({ w, h });
	}

	void Camera::update(base::MatrixState * matrixState, float delta) {
		auto viewMatrix = Transform::WorldMatrix(node()).invert();
		_camera.setView(viewMatrix);

		// Update matrixes
		matrixState->view().load(_camera.view());
		matrixState->projection().load(_camera.projection());

	}
}
}
