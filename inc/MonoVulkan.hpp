#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

#include "Tracy.hpp"
#include "TracyVulkan.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>

constexpr uint32_t WIDTH = 1600;
constexpr uint32_t HEIGHT = 900;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

static float s_scale[3] = {0.7f, 0.7f, 0.7f};
static float s_rotate[3] = {0.f, 0.f, 0.f};
static float s_translate[3] = {0.f, -5.f, 0.f};
static float s_viewPos[3] = {15.f, 0.f, 0.f};
static float s_nearPlane = 0.1f;
static float s_farPlane = 100.f;

const std::string MODEL_PATH = "res/models/wooden_watch_tower2.obj";
// const std::string MODEL_PATH = "res/models/Snowflake.obj";
const std::string TEXTURE_PATH = "res/textures/Wood_Tower_Col.jpg";

constexpr int SNOWFLAKE_COUNT = 100;
constexpr int VORTEX_COUNT = 10;

struct Vortex {
	alignas(16) glm::vec3 origin;
	alignas(4) float velocity;
	alignas(4) float radius;
	alignas(4) float length;
};

struct Snowflake {
	glm::vec3 position;
	float weight;
};

struct PushConstantData{
	float snowflakeCount = SNOWFLAKE_COUNT;
	float vortexCount = VORTEX_COUNT;
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

template <std::size_t Last = 0, typename TF, typename TArray, typename... TRest>
constexpr auto with_acc_sizes(TF&& f, const TArray& array, const TRest&... rest)
{
    f(array, std::integral_constant<std::size_t, Last>{});

    if constexpr(sizeof...(TRest) != 0)
    {
        with_acc_sizes<Last + std::tuple_size_v<TArray>>(f, rest...); 
    }
}

template<typename T, std::size_t... Sizes>
constexpr auto concat(const std::array<T, Sizes>&... arrays)
{
    std::array<T, (Sizes + ...)> result{};

    with_acc_sizes([&](const auto& arr, auto offset)
    {
        std::copy(arr.begin(), arr.end(), result.begin() + offset);
    }, arrays...);

    return result;
}

float generateRandomFloat(float low, float high){
	return low + (static_cast<float>(rand()) / (RAND_MAX / (high - low)));
}
 
void testAlignment()
{
	struct Struct1{
		glm::vec3 pos;
	};

	struct Struct2{
		alignas(16) glm::vec3 pos;
	};

	struct Struct3{
		alignas(16) glm::vec3 pos;
		float weight;
	};

	struct Struct4{
		glm::vec3 pos;
		float weight;
	};

	struct Struct5{
		float weight;
		alignas(16) glm::vec3 pos;
	};

	struct Struct6{
		alignas(16) glm::vec3 pos;
		alignas(16) glm::vec3 weight;
	};

		std::cout << "\nsize of Struct1 is: " << sizeof(Struct1)
				<< "\nsize of Struct2 is:" << sizeof(Struct2)
				<< "\nsize of Struct3 is:" << sizeof(Struct3)
				<< "\noffset of weight is:" << offsetof(Struct3, weight)
				<< "\nsize of Struct4 is:" << sizeof(Struct4)
				<< "\noffset of weight is:" << offsetof(Struct4, weight)
				<< "\nnsize of Struct5 is:" << sizeof(Struct5)
				<< "\noffset of weight is:" << offsetof(Struct5, weight)
				<< "\nsize of Struct6 is:" << sizeof(Struct6)
				<< "\noffset of weight is:" << offsetof(Struct6, weight) << std::endl;

}
