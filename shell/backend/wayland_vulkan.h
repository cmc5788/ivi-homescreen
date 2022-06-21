// Copyright 2020 Toyota Connected North America
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cstdint>
#include <vector>

#include "backend/backend.h"
#include "bluevk/BlueVK.h"
#include "third_party/flutter/flutter_embedder.h"

class VulkanBackend : Backend {
 public:
  VulkanBackend(struct wl_display* display,
                struct wl_surface* surface,
                uint32_t width,
                uint32_t height,
                bool enable_validation_layers);
  ~VulkanBackend();
  static void Resize(void* user_data,
                     size_t index,
                     Engine* engine,
                     int32_t width,
                     int32_t height);
  static void CreateSurface(void* user_data,
                            size_t index,
                            wl_surface* surface,
                            int32_t width,
                            int32_t height);

 private:
  static constexpr VkPresentModeKHR kPreferredPresentMode =
      VK_PRESENT_MODE_FIFO_KHR;

  struct {
    std::vector<const char*> enabled_instance_extensions;
    std::vector<const char*> enabled_device_extensions;
    std::vector<const char*> enabled_layer_extensions;
    VkInstance instance{};
    VkSurfaceKHR surface{VK_NULL_HANDLE};

    VkPhysicalDevice physical_device{};
    VkPhysicalDeviceFeatures physical_device_features{};
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties{};
    VkDevice device{};
    uint32_t queue_family_index{};
    VkQueue queue{};

    bool debugMarkersSupported{false};
    bool portabilitySubsetSupported{false};
    bool maintenanceSupported[3]{false};

    VkCommandPool swapchain_command_pool{};
    std::vector<VkCommandBuffer> present_transition_buffers;

    VkFence image_ready_fence{};
    VkSemaphore present_transition_semaphore{};

    VkSurfaceFormatKHR surface_format{};
    VkSwapchainKHR swapchain{};
    std::vector<VkImage> swapchain_images;
    uint32_t last_image_index{};

    bool resize_pending = false;
  } state_;

  struct wl_display* wl_display_;
  struct wl_surface* wl_surface_;
  uint32_t width_;
  uint32_t height_;
  bool enable_validation_layers_;
  bool resize_pending_;

  VkDebugUtilsMessengerEXT debugMessenger_{};

  void createInstance();
  void setupDebugMessenger();
  static VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
      const VkAllocationCallbacks* pAllocator,
      VkDebugUtilsMessengerEXT* pDebugMessenger);
  static void DestroyDebugUtilsMessengerEXT(
      VkInstance instance,
      VkDebugUtilsMessengerEXT debugMessenger,
      const VkAllocationCallbacks* pAllocator);
  void createSurface(struct wl_display* display, struct wl_surface* surface);
  void findPhysicalDevice();
  void createLogicalDevice();
  bool InitializeSwapchain();

  static std::vector<VkLayerProperties> enumerateInstanceLayerProperties();
  static std::vector<VkExtensionProperties>
  enumerateInstanceExtensionProperties();

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData);

  static FlutterVulkanImage GetNextImageCallback(
      void* user_data,
      const FlutterFrameInfo* frame_info);
  static bool PresentCallback(void* user_data, const FlutterVulkanImage* image);
  static void* GetInstanceProcAddressCallback(
      void* user_data,
      FlutterVulkanInstanceHandle instance,
      const char* procname);
};