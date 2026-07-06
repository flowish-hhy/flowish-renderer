
#include "FlowishSurface.h"

#include <stdexcept>

FlowishSurface::FlowishSurface(VkInstance instance, GLFWwindow *window)
    : _instance(instance) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &_surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

FlowishSurface::~FlowishSurface() {
    if (_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
    }
}
