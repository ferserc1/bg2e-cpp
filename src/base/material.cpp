
#include <bg2e/base/material.hpp>
#include <bg2e/utils/texture_generator.hpp>
#include <bg2e/utils/texture_cache.hpp>

namespace bg2e {
namespace base {

	Material::Material()
        :_fresnel(0xFFFFFFFF)
        ,_isTransparent(false)
        ,_alphaCutoff(0.5f)
        ,_heightIntensity(1.0f)
        ,_castShadows(true)
        ,_cullFace(true)
        ,_unlit(false)
	{
		_diffuse.texture = utils::TextureCache::Get()->whiteTexture();
		_normal.texture = utils::TextureCache::Get()->normalMapTexture();
		_ambientOcclussion.texture = utils::TextureCache::Get()->whiteTexture();
		_metallic.texture = utils::TextureCache::Get()->blackTexture();
		_roughness.texture = utils::TextureCache::Get()->whiteTexture();
		_height.texture = utils::TextureCache::Get()->blackTexture();
		_lightEmission.texture = utils::TextureCache::Get()->blackTexture();
	}

	Material::~Material() {

	}

	Material * Material::clone() {
		Material * newMat = new Material();

		newMat->_diffuse = _diffuse;
		newMat->_metallic = _metallic;
		newMat->_roughness = _roughness;
		newMat->_lightEmission = _lightEmission;
		newMat->_ambientOcclussion = _ambientOcclussion;
		newMat->_height = _height;
		newMat->_normal = _normal;
		newMat->_fresnel = _fresnel;
		newMat->_isTransparent = _isTransparent;
		newMat->_alphaCutoff = _alphaCutoff;
		newMat->_heightIntensity = _heightIntensity;
		newMat->_castShadows = _castShadows;
		newMat->_cullFace = _cullFace;
		newMat->_unlit = _unlit;

		return newMat;
	}

	void Material::setDiffuse(const math::color& c) {
		_diffuse.texture = utils::textureWithColor(c);
	}

	void Material::setMetallic(float m) {
		_metallic.texture = utils::textureWithColor(math::color(m, m, m, 1.0f));
		_metallic.channel = 0;
	}

	void Material::setRoughness(float r) {
		_roughness.texture = utils::textureWithColor(math::color(r, r, r, 1.0f));
		_roughness.channel = 0;
	}

	void Material::setLightEmission(float e) {
		_lightEmission.texture = utils::textureWithColor(math::color(e, e, e, 1.0f));
		_lightEmission.channel = 0;
	}

	void Material::setAmbientOcclussion(float a) {
		_ambientOcclussion.texture = utils::textureWithColor(math::color(a, a, a, 1.0f));
		_ambientOcclussion.channel = 0;
	}

	void Material::setHeight(float h) {
		_height.texture = utils::textureWithColor(math::color(h, h, h, 1.0f));
		_height.channel = 0;
	}

	void Material::setNormal(const math::color& c) {
		_normal.texture = utils::textureWithColor(c);
	}


	void Material::setDiffuse(Texture* t) {
		_diffuse.texture = t;
	}

	void Material::setMetallic(Texture* t, uint16_t channel) {
		_metallic.texture = t;
		_metallic.channel = channel;
	}

	void Material::setRoughness(Texture* t, uint16_t channel) {
		_roughness.texture = t;
		_roughness.channel = channel;
	}

	void Material::setLightEmission(Texture* t, uint16_t channel) {
		_lightEmission.texture = t;
		_lightEmission.channel = channel;
	}

	void Material::setAmbientOcclussion(Texture* t, uint16_t channel) {
		_ambientOcclussion.texture = t;
		_ambientOcclussion.channel = channel;
	}

	void Material::setHeight(Texture* t, uint16_t channel) {
		_height.texture = t;
		_height.channel = channel;
	}

	void Material::setNormal(Texture* t) {
		_normal.texture = t;
	}


}
}
