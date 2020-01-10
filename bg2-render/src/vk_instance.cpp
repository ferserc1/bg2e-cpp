
#include <bg2render/vk_instance.hpp>

#include <bg2e-config.hpp>

#include <stdexcept>
#include <iostream>
#include <string.h>


namespace bg2render {
	namespace vk {
		VkDebugUtilsMessageSeverityFlagBitsEXT Instance::VulkanDebugMessageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

		Instance* Instance::CreateDefault(bg2wnd::Window* window, const std::string& appName) {
			Instance* instance = nullptr;

			instance = new Instance();
			instance->configureAppName(appName);
			#ifdef _DEBUG
				instance->setDebugCallback([](
					VkDebugUtilsMessageSeverityFlagBitsEXT severity,
					VkDebugUtilsMessageTypeFlagsEXT type,
					const VkDebugUtilsMessengerCallbackDataEXT * pData) {
					if (severity >= VulkanDebugMessageSeverity) {
						std::cout << pData->pMessage << std::endl;
					}
				});

				std::vector<VkExtensionProperties> extensionData;
				instance->enumerateInstanceExtensionProperties(extensionData);
				std::cout << "Available Vulkan extensions: " << std::endl;
				for (const auto& ext : extensionData) {
					std::cout << "\t" << ext.extensionName << std::endl;
				}
			#endif

			std::vector<const char*> extensions;
			window->getVulkanRequiredInstanceExtensions(extensions);
			instance->configureRequiredExtensions(extensions);
			instance->create();

			// It's important to link the window surface to vulkan instance BEFORE
			// choose physical device, if you want to get support for presentation queues
			instance->setSurface(window->createVulkanSurface(instance->instance()));

			instance->choosePhysicalDevices();

			return instance;
		}


		Instance::Instance()
		{

		}

		Instance::~Instance() {
			if (_surface != VK_NULL_HANDLE) {
				vkDestroySurfaceKHR(_instance, _surface, nullptr);
			}
			_renderDevice = nullptr;
			_presentDevice = nullptr;
			if (_destroyDebugCallback) {
				destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
			}

			if (_instance != VK_NULL_HANDLE) {
				vkDestroyInstance(_instance, nullptr);
			}
		}

		void Instance::configureRequiredExtensions(const std::vector<const char*>& ext) {
			for (auto& e : ext) {
				_requiredExtensions.push_back(e);
			}
		}

		void Instance::configureRequiredLayers(const std::vector<const char*>& layers) {
			for (auto& l : layers) {
				_requiredLayers.push_back(l);
			}
		}

		void Instance::create() {
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = _appName.c_str();
			appInfo.applicationVersion = VK_MAKE_VERSION(_version[0], _version[1], _version[2]);
			appInfo.pEngineName = "bg2 engine";
			appInfo.engineVersion = VK_MAKE_VERSION(bg2e_MAJOR_VERSION, bg2e_MINOR_VERSION, bg2e_REV_VERSION);
			appInfo.apiVersion = VK_API_VERSION_1_0;


			bool configureValidationLayers = false;
			if (_debugCallback && checkValidationLayerSupport()) {
				_requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				_requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
				configureValidationLayers = true;
			}

			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(_requiredExtensions.size());
			if (_requiredExtensions.size() > 0) {
				createInfo.ppEnabledExtensionNames = _requiredExtensions.data();
			}

			createInfo.enabledLayerCount = static_cast<uint32_t>(_requiredLayers.size());
			if (_requiredLayers.size() > 0) {
				createInfo.ppEnabledLayerNames = _requiredLayers.data();
			}

			if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
				throw std::runtime_error("Could not create Vulkan instance");
			}

			if (configureValidationLayers) {
				VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				createInfo.pfnUserCallback = debugCallback;
				createInfo.pUserData = this;
				if (createDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
					throw std::runtime_error("Failed to setup debug messenter");
				}
				_destroyDebugCallback = true;
			}
		}

		void Instance::enumerateInstanceExtensionProperties(std::vector<VkExtensionProperties>& ext) {
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			ext.resize(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, ext.data());
		}

		void Instance::enumeratePhysicalDevices(std::vector<std::shared_ptr<PhysicalDevice>> & devices) {
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
            if (deviceCount == 0) {
                throw std::runtime_error("Failed to find Vulkan compatible GPUs");
            }

            std::vector<VkPhysicalDevice> vkDevices(deviceCount);
            vkEnumeratePhysicalDevices(_instance, &deviceCount, vkDevices.data());

            for (auto dev : vkDevices) {
                devices.push_back(std::make_shared<PhysicalDevice>(this,dev,_surface));
            }
		}

		void Instance::choosePhysicalDevices() {
			std::vector<std::shared_ptr<PhysicalDevice>> devices;
			enumeratePhysicalDevices(devices);

			// In this version, we'll use the same device for render and present
			for (const auto & device : devices) {
				if (_renderPhysicalDevice ==nullptr && 
					isDeviceSuitableForTask(device.get(), kDeviceTaskRender) &&
					isDeviceSuitableForTask(device.get(), kDeviceTaskPresent)) {
					_renderPhysicalDevice = device;
				}
			}

			if (_renderPhysicalDevice == nullptr) {
				throw std::runtime_error("No such suitable Vulkan device for render.");
			}

			// We use the same device for presentation and render
			_renderDevice = std::make_shared<Device>(_renderPhysicalDevice.get(), kDeviceTaskRender | kDeviceTaskPresent);
			if (_debugMessenger) {
				_renderDevice->configureEnabledLayers(std::vector<const char*>{
					"VK_LAYER_KHRONOS_validation"
				});
			}
			_renderDevice->configureEnabledExtensions(std::vector<const char*>{
				"VK_KHR_swapchain"
			});
			
			_renderDevice->create();

			_presentDevice = _renderDevice;
		}

		bool Instance::isDeviceSuitableForTask(PhysicalDevice* dev, DeviceTask task) {
			VkPhysicalDeviceFeatures features;
			dev->getFeatures(features);

			if (task == kDeviceTaskRender) {
				return dev->queueIndices().graphicsFamily != -1 && features.samplerAnisotropy;
			}
			else if (task == kDeviceTaskPresent &&
				dev->checkExtensionSupport(std::vector<const char*>{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }))
			{
				return dev->queueIndices().presentFamily != -1 &&
					!dev->getSwapChainSupport().formats.empty() &&
					!dev->getSwapChainSupport().presentModes.empty();
			}
			return false;
		}

		bool Instance::checkValidationLayerSupport() {
			if (_debugCallback) {
				const std::vector<const char*> validationLayers = {
					"VK_LAYER_KHRONOS_validation"
				};

				uint32_t layerCount;
				vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
				std::vector<VkLayerProperties> availableLayers(layerCount);
				vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

				bool supported = true;
				for (const char* layerName : validationLayers) {
					bool layerFound = false;
					for (const auto& layerProperties : availableLayers) {
						if (strcmp(layerName, layerProperties.layerName) == 0) {
							layerFound = true;
							break;
						}
					}

					if (!layerFound) {
						supported = false;
						break;
					}
				}

				if (!supported) {
					std::cerr << "WARNING: Validation debug function set, but no validation layers supported." << std::endl;
				}

				return supported;
			}
			return false;
		}

		VkResult Instance::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr) {
				return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
			}
			else {
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		void Instance::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr) {
				func(instance, debugMessenger, pAllocator);
			}
		}
	}
}