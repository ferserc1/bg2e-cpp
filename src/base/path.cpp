
#include <bg2e/base/path.hpp>

#include <cctype>
#include <algorithm>

namespace bg2e {
namespace base {

	path path::pathAddingComponent(const std::string& component) const {
		path result(_pathString);
		result.addComponent(component);
		return result;
	}

	path path::pathAddingExtension(const std::string& extension) const {
		path result(_pathString);
		result.addExtension(extension);
		return result;
	}

	path path::pathRemovingLastComponent() const {
		path result(_pathString);
		result.removeLastComponent();
		return result;
	}

	path path::pathRemovingExtension() const {
		path result(_pathString);
		result.removeExtension();
		return result;
	}

	std::string path::fileName() const {
		std::string result;
		auto index = _pathString.find_last_of('/');
		if (index != std::string::npos) {
			result = _pathString.substr(index + 1);
		}
		return result;
	}

	std::string path::extension() const {
		std::string result;
		auto index = _pathString.find_last_of('.');
		if (index != std::string::npos) {
			result = _pathString.substr(index + 1);
		}
		return result;
	}
    
    std::string path::lowerCaseExtension() const {
        std::string result = extension();
        std::transform(
            result.begin(),
            result.end(),
            result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

	path& path::addComponent(const std::string& comp) {
		std::string fixedComp = comp;
		std::replace(fixedComp.begin(), fixedComp.end(), '\\', '/');

		if (_pathString.length() == 0) {
			_pathString = comp;
		}
		else if (_pathString.back() == '/' && comp.front() == '/') {
			_pathString = _pathString.substr(0, _pathString.size() - 1);
			_pathString += comp;
		}
		else if ((_pathString.back() == '/' && comp.front() != '/') ||
			(_pathString.back() != '/' && comp.front() == '/'))
		{
			_pathString += comp;
		}
		else {
			_pathString += '/' + comp;
		}
		fixSlashes();
		return *this;
	}

	path& path::addExtension(const std::string& ext) {
		if (_pathString.back() == '/') {
			_pathString = _pathString.substr(0, _pathString.size() - 1);
		}
		if (ext.front() == '.') {
			_pathString += ext;
		}
		else {
			_pathString += '.' + ext;
		}
		return *this;
	}

	path& path::removeLastComponent() {
		auto index = _pathString.find_last_of('/');
		if (index != std::string::npos) {
			_pathString = _pathString.substr(0, index);
		}
		return *this;
	}

	path& path::removeExtension() {
		auto index = _pathString.find_last_of('.');
		if (index != std::string::npos) {
			_pathString = _pathString.substr(0, index);
		}
		return *this;
	}

}
}
