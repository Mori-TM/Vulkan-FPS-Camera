#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <string.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
		
#include "VulkanUtils.h" 					
#include "TexLoader.h"

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  800.0 / 2.0;
float fov   =  45.0f;

glm::vec3 deltaForward = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 deltaSide = glm::vec3(0.0, 0.0, 0.0);

//class Vertex
//{
//public:	
struct Vertex
{
	glm::vec3 Pos;
	glm::vec4 Color;
	glm::vec2 UVcoord;
	
	Vertex(glm::vec3 Pos, glm::vec4 Color, glm::vec2 UVcoord)
		: Pos(Pos), Color(Color), UVcoord(UVcoord)
	{}
	
	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription VertexInputBindingDescription;
		VertexInputBindingDescription.binding = 0;
		VertexInputBindingDescription.stride = sizeof(Vertex);
		VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		return VertexInputBindingDescription;
	}
	
	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription()
	{
		std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescription(3);
		VertexInputAttributeDescription[0].location = 0;
		VertexInputAttributeDescription[0].binding = 0;
		VertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		VertexInputAttributeDescription[0].offset = offsetof(Vertex, Pos);
		
		VertexInputAttributeDescription[1].location = 1;
		VertexInputAttributeDescription[1].binding = 0;
		VertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		VertexInputAttributeDescription[1].offset = offsetof(Vertex, Color);
		
		VertexInputAttributeDescription[2].location = 2;
		VertexInputAttributeDescription[2].binding = 0;
		VertexInputAttributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
		VertexInputAttributeDescription[2].offset = offsetof(Vertex, UVcoord);
		
		return VertexInputAttributeDescription;
	}
};
/*
std::vector<Vertex> Vertices =
{
	Vertex({-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}),
	Vertex({ 0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}),
	Vertex({-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}),	
	Vertex({ 0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}),

};

std::vector<uint32_t> Indices =
{
	0, 1, 2,
	0, 3, 1
};
*/

std::vector<Vertex> Vertices1 = 
{
	Vertex({-0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f, -1.0f}),
	Vertex({ 0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f, -1.0f}),
	Vertex({ 0.5f,  0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f, -1.0f}),
	Vertex({ 0.5f,  0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f, -1.0f}),
	Vertex({-0.5f,  0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f, -1.0f}),
	Vertex({-0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f, -1.0f}),
	
	Vertex({-0.5f, -0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  1.0f}),
	Vertex({ 0.5f, -0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  1.0f}),
	Vertex({ 0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  1.0f}),
	Vertex({ 0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  1.0f}),
	Vertex({-0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  1.0f}),
	Vertex({-0.5f, -0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  1.0f}),
	
	Vertex({-0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({-0.5f,  0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({-0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({-0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({-0.5f, -0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({-0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	
	Vertex({0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({0.5f,  0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({0.5f, -0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	Vertex({0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {0.0f,  0.0f}),
	
	Vertex({-0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},  {-1.0f,  0.0f}),
	Vertex({ 0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},  {-1.0f,  0.0f}),
	Vertex({ 0.5f, -0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},  {-1.0f,  0.0f}),
	Vertex({ 0.5f, -0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},  {-1.0f,  0.0f}),
	Vertex({-0.5f, -0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},  {-1.0f,  0.0f}),
	Vertex({-0.5f, -0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},  {-1.0f,  0.0f}),

	Vertex({-0.5f,  0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {1.0f,  0.0f}),
	Vertex({ 0.5f,  0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {1.0f,  0.0f}),
	Vertex({ 0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {1.0f,  0.0f}),
	Vertex({ 0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {1.0f,  0.0f}),
	Vertex({-0.5f,  0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0},   {1.0f,  0.0f}),
	Vertex({-0.5f,  0.5f, -0.5f}, {1.0, 1.0, 1.0, 1.0},   {1.0f,  0.0f}),
};

std::vector<uint32_t> Indices1 =
{
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36
	
};

std::vector<Vertex> Vertices =
{	
	Vertex({-1.0, -1.0,  1.0}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0, 0.0}),
    Vertex({ 1.0, -1.0,  1.0}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0, 0.0}),
    Vertex({ 1.0,  1.0,  1.0},	{0.0f, 0.0f, 1.0f, 1.0f}, {1.0, 1.0}),	
    Vertex({-1.0,  1.0,  1.0},	{0.0f, 1.0f, 0.0f, 1.0f}, {0.0, 1.0}),
    
    Vertex({-1.0, -1.0, -1.0}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0, 0.0}),
    Vertex({ 1.0, -1.0, -1.0}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0, 0.0}),
    Vertex({ 1.0,  1.0, -1.0}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0, 1.0}),
    Vertex({-1.0,  1.0, -1.0}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0, 1.0}),
};

std::vector<uint32_t> Indices =
{
	// front
	0, 1, 2,
	2, 3, 0,
	// right
	1, 5, 6,
	6, 2, 1,
	// back
	7, 6, 5,
	5, 4, 7,
	// left
	4, 0, 3,
	3, 7, 4,
	// bottom
	4, 5, 1,
	1, 0, 4,
	// top
	3, 2, 6,
	6, 7, 3
};

void printStats(VkPhysicalDevice &device)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	
	printf("Grafikkarte: %s\n", properties.deviceName);
	uint32_t apiver = properties.apiVersion;
	printf("API version: %d.%d.%d\n", VK_VERSION_MAJOR(apiver), VK_VERSION_MINOR(apiver), VK_VERSION_PATCH(apiver));
	printf("Driver Version: %d\n", properties.driverVersion);
	printf("Vendor ID: %d\n", properties.vendorID);
	printf("Device ID: %d\n", properties.deviceID);
	printf("Device Typ: %d\n", properties.deviceType);
	printf("discrete Queue Priorities: %d\n", properties.limits.discreteQueuePriorities);
	
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);
	printf("GeometryShader: %d\n\n", features.geometryShader);
	
	VkPhysicalDeviceMemoryProperties memprop;
	vkGetPhysicalDeviceMemoryProperties(device, &memprop);
	
	uint32_t AmountOfQueueFamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &AmountOfQueueFamilies, NULL);
	VkQueueFamilyProperties *familyProperties = new VkQueueFamilyProperties[AmountOfQueueFamilies];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &AmountOfQueueFamilies, familyProperties);
	printf("Amount Of Queue Families: %d\n\n", AmountOfQueueFamilies);
	for(int i = 0; i < AmountOfQueueFamilies; i++)
	{
		printf("Queue Fmaily #%d\n", i);
		printf("VK_QUEUE_GRAPHICS_BIT: %d\n", ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0));
		printf("VK_QUEUE_COMPUTE_BIT: %d\n", ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0));
		printf("VK_QUEUE_TRANSFER_BIT: %d\n", ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0));
		printf("VK_QUEUE_SPARSE_BINDING_BIT: %d\n", ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0));
		printf("Queue Count: %d\n", familyProperties[i].queueCount);
		printf("Timestamp Valid Bits: %d\n", familyProperties[i].timestampValidBits);
		
		uint32_t width = familyProperties[i].minImageTransferGranularity.width;
		uint32_t height = familyProperties[i].minImageTransferGranularity.height;
		uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;
		
		printf("Min Image Timestemp Granularity %d, %d, %d\n\n", width, height, depth);
	}
	
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);
	printf("Surface Capabilities:\n");
	printf("\t minImageCount: %d\n", surfaceCapabilities.minImageCount);
	printf("\t maxImageCount: %d\n", surfaceCapabilities.maxImageCount);
	printf("\t currentExtent: %d/%d\n", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
	printf("\t minImageExtent: %d/%d\n", surfaceCapabilities.minImageExtent.width, surfaceCapabilities.minImageExtent.height);
	printf("\t maxImageExtent: %d/%d\n", surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.maxImageExtent.height);
	printf("\t maxImageArrayLayers: %d\n", surfaceCapabilities.maxImageArrayLayers);
	printf("\t supportedTransforms: %d\n", surfaceCapabilities.supportedTransforms);
	printf("\t currentTransform: %d\n", surfaceCapabilities.currentTransform);
	printf("\t supportedCompositeAlpha: %d\n", surfaceCapabilities.supportedCompositeAlpha);
	printf("\t supportedUsageFlags: %d\n", surfaceCapabilities.supportedUsageFlags);
	
	uint32_t amountOfFormats = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, NULL);
	VkSurfaceFormatKHR *surfaceFormats = new VkSurfaceFormatKHR[amountOfFormats];
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, surfaceFormats);
	
	printf("Amount of Formats: %d\n", amountOfFormats);
	for(int i = 0; i < amountOfFormats; i++)
	{
		printf("\t Format: %d\n", surfaceFormats[i].format);
	}
	
	uint32_t amountOfPresentationModes = 0;						
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes, NULL);
	VkPresentModeKHR *presentModes = new VkPresentModeKHR[amountOfPresentationModes];
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes, presentModes);
	
	printf("Amount of Presentation Modes: %d\n", amountOfPresentationModes);
	for(int i = 0; i < amountOfPresentationModes; i++)
	{
		printf("\t Supported presentation mode: %d\n", presentModes[i]);
	}
		
	delete[] familyProperties;
	delete[] surfaceFormats;
	delete[] presentModes;
}

std::vector<char> readFile(const std::string &filename)
{
	std::ifstream File(filename, std::ios::binary | std::ios::ate);
	
	if(File)
	{
		size_t FileSize = (size_t)File.tellg();
		std::vector<char> FileBuffer(FileSize);
		File.seekg(0);
		File.read(FileBuffer.data(), FileSize);
		File.close();
		return FileBuffer;
	}
	else
	{
		throw printf("Failed to open File");
	}
}

void RecreateSwapchain();

void OnWindowResized(GLFWwindow *window, int w, int h)
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[0], surface, &surfaceCapabilities);
	
	if(w > surfaceCapabilities.maxImageExtent.width) w = surfaceCapabilities.maxImageExtent.width;
	if(h > surfaceCapabilities.maxImageExtent.height) w = surfaceCapabilities.maxImageExtent.height;
	
	if(w == 0 || h == 0) return;
	
	Width = w;
	Height = h;
	RecreateSwapchain();
}

float Move = 0.0;

void Keyboard(GLFWwindow* window, int Key, int scancode, int action, int mods)
{
	if(Key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) exit(0);
	
	if(Key == GLFW_KEY_W && action == GLFW_PRESS){ deltaForward = glm::vec3(1.0, 1.0, 1.0); }
	if(Key == GLFW_KEY_S && action == GLFW_PRESS){ deltaForward = -glm::vec3(1.0, 1.0, 1.0); }
	if(Key == GLFW_KEY_A && action == GLFW_PRESS){ deltaSide = -glm::vec3(1.0, 1.0, 1.0); }
	if(Key == GLFW_KEY_D && action == GLFW_PRESS){ deltaSide = glm::vec3(1.0, 1.0, 1.0); }
	
	if(Key == GLFW_KEY_W && action == GLFW_RELEASE){ deltaForward = glm::vec3(0.0, 0.0, 0.0); }
	if(Key == GLFW_KEY_S && action == GLFW_RELEASE){ deltaForward = glm::vec3(0.0, 0.0, 0.0); }
	if(Key == GLFW_KEY_A && action == GLFW_RELEASE){ deltaSide = glm::vec3(0.0, 0.0, 0.0);}
	if(Key == GLFW_KEY_D && action == GLFW_RELEASE){ deltaSide = glm::vec3(0.0, 0.0, 0.0);}
}

void Mouse(GLFWwindow* window, double xpos, double ypos)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, Width/2, Height/2);
	if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
	
	float xoffset;
	float yoffset;
	
    xoffset = (xpos - Width/2);
    yoffset = (ypos - Width/2);
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
    

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
		pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = -sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}  

void StartGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	
	window = glfwCreateWindow(Width, Height, "Vulkan Tutorial", NULL, NULL);
	glfwSetWindowSizeCallback(window, OnWindowResized);
	glfwSetKeyCallback(window, Keyboard);
	glfwSetCursorPosCallback(window, Mouse);
}

void CreateShaderModule(const std::vector<char> &code, VkShaderModule *ShaderModule)
{
	VkShaderModuleCreateInfo ShaderCreateInfo;
	ShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderCreateInfo.pNext = NULL;
	ShaderCreateInfo.flags = 0;
	ShaderCreateInfo.codeSize = code.size();
	ShaderCreateInfo.pCode = (uint32_t*)code.data();
	
	VkResult result = vkCreateShaderModule(device, &ShaderCreateInfo, NULL, ShaderModule);
	ASSERT_VULKAN(result);
}

void CreateInstance()
{
	VkApplicationInfo appinfo;
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;		//Was das ist
	appinfo.pNext = NULL;									//Extensions	//stat NULL kan auch nullptr genutzt werden
	appinfo.pApplicationName = "1st Vulkan";				//App Name
	appinfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);	//App Version
	appinfo.pEngineName = "First Vulkan Engine";			//Engine Name
	appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);		//Engine Version
	appinfo.apiVersion = VK_API_VERSION_1_0;				//Vulkan Version
	
	const std::vector<const char*> validationLayers = 
	{
		"VK_LAYER_LUNARG_standard_validation"	
	};
	
	uint32_t amountOfGlfwExtensions = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);
	
	VkInstanceCreateInfo InstanceInfo;
	InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;	//Was das ist
	InstanceInfo.pNext = NULL;										//Extensions
	InstanceInfo.flags = 0;
	InstanceInfo.pApplicationInfo = &appinfo;
	InstanceInfo.enabledLayerCount = validationLayers.size();
	InstanceInfo.ppEnabledLayerNames = validationLayers.data();
	InstanceInfo.enabledExtensionCount = amountOfGlfwExtensions;
	InstanceInfo.ppEnabledExtensionNames = glfwExtensions;
	
	VkResult result = vkCreateInstance(&InstanceInfo, NULL, &instance);
	ASSERT_VULKAN(result);
}

void PrintInstanceLayers()
{
	uint32_t amountOfLayers = 0;
	vkEnumerateInstanceLayerProperties(&amountOfLayers, NULL);
	VkLayerProperties *layers = new VkLayerProperties[amountOfLayers];
	vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);
	
	printf("Amount of Instance Layers: %d\n", amountOfLayers);
	for(int i = 0; i < amountOfLayers; i++)
	{
		printf("Name: %s\n", layers[i].layerName);
		printf("Spec Version: %d\n", layers[i].specVersion);
		printf("Impl Version: %d\n", layers[i].implementationVersion);
		printf("Description: %s\n\n", layers[i].description);
	}
	
	delete[] layers;
}

void PrintInstanceExtensions()
{
	uint32_t amountOfExtensions = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &amountOfExtensions, NULL);
	VkExtensionProperties *extensions = new VkExtensionProperties[amountOfExtensions];
	vkEnumerateInstanceExtensionProperties(NULL, &amountOfExtensions, extensions);
	
	printf("Amount of Extensions: %d\n", amountOfExtensions);
	for(int i = 0; i < amountOfExtensions; i++)
	{
		printf("Name: %s\n", extensions[i].extensionName);
		printf("Spec Version: %d\n\n", extensions[i].specVersion);
	}	
	
	delete[] extensions;
}

void CreateGlfwWindowSurface()
{
	VkResult result = glfwCreateWindowSurface(instance, window, NULL, &surface);
	ASSERT_VULKAN(result);												//Error Handling
}

std::vector<VkPhysicalDevice> GetAllPhysicalDevices()
{
	uint32_t amountofPhysicalDevices = 0;
	VkResult result = vkEnumeratePhysicalDevices(instance, &amountofPhysicalDevices, NULL);	//Grafikkarten Anzahl
	ASSERT_VULKAN(result);																	//Error Handling
	
	std::vector<VkPhysicalDevice> physicalDevices;
	physicalDevices.resize(amountofPhysicalDevices);
	
	result = vkEnumeratePhysicalDevices(instance, &amountofPhysicalDevices, physicalDevices.data());
	ASSERT_VULKAN(result);
	
	return physicalDevices;
}

void PrintStatsOfAllPhysicalDevices()
{																
	for(int i = 0; i < physicalDevices.size(); i++)
	{
		printStats(physicalDevices[i]);
	}
}

void CreateLogicalDevice()
{
	float queuePrios[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
	VkDeviceQueueCreateInfo deviceQueueCreateInfo;		
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;			
	deviceQueueCreateInfo.pNext = NULL;
	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.queueFamilyIndex = 0;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePrios;	
	
	VkPhysicalDeviceFeatures UsedFeatures = {};
	UsedFeatures.samplerAnisotropy = VK_TRUE;
	
	
	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	
	VkDeviceCreateInfo DeviceCreateInfo;	
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pNext = NULL;
	DeviceCreateInfo.flags = 0;
	DeviceCreateInfo.queueCreateInfoCount = 1;
	DeviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	DeviceCreateInfo.enabledLayerCount = 0;
	DeviceCreateInfo.ppEnabledLayerNames = NULL;
	DeviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	DeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	DeviceCreateInfo.pEnabledFeatures = &UsedFeatures;		
	
	VkResult result = vkCreateDevice(physicalDevices[0], &DeviceCreateInfo, NULL, &device);							
	ASSERT_VULKAN(result);
}

void InitQueue()
{
	vkGetDeviceQueue(device, 0, 0, &queue);
}

void CheckSurfaceSupport()
{
	VkBool32 surfaceSupport = false;
	VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, surface, &surfaceSupport);
	ASSERT_VULKAN(result);
	
	if(!surfaceSupport)
	{
		printf("Surface Not Supported");
		exit(0);
	}
}

void CreateSwapchain()
{
	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = NULL;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = 3;
	swapchainCreateInfo.imageFormat = BGRA;
	swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCreateInfo.imageExtent = VkExtent2D{ Width, Height };
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.pQueueFamilyIndices = NULL;
	swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; 
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = swapchain;
	
	VkResult result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);
	ASSERT_VULKAN(result);
}

void CreateImageViews()
{
	vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, NULL);
	VkImage *swapchainImages = new VkImage[amountOfImagesInSwapchain];
	VkResult result = vkGetSwapchainImagesKHR(device, swapchain, & amountOfImagesInSwapchain, swapchainImages);
	ASSERT_VULKAN(result);
	
	imageViews = new VkImageView[amountOfImagesInSwapchain];
	for(int i = 0; i < amountOfImagesInSwapchain; i++)
	{
		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = NULL;
		imageViewCreateInfo.flags =  0;
		imageViewCreateInfo.image = swapchainImages[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = BGRA;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		
		result = vkCreateImageView(device, &imageViewCreateInfo, NULL, &imageViews[i]);
		ASSERT_VULKAN(result);
	}
	
	delete[] swapchainImages;
}

void CreateRenderPass()
{
	VkAttachmentDescription AttachmentDescription;
	AttachmentDescription.flags = 0;
	AttachmentDescription.format = BGRA;
	AttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	AttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	AttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	AttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	AttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	AttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	AttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	
	VkAttachmentReference AttachmentReference;
	AttachmentReference.attachment = 0;
	AttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription SubpassDescription;
	SubpassDescription.flags = 0;
	SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	SubpassDescription.inputAttachmentCount = 0;
	SubpassDescription.pInputAttachments = NULL;
	SubpassDescription.colorAttachmentCount = 1;
	SubpassDescription.pColorAttachments = &AttachmentReference;
	SubpassDescription.pResolveAttachments = NULL;
	SubpassDescription.pDepthStencilAttachment = NULL;
	SubpassDescription.preserveAttachmentCount = 0;
	SubpassDescription.pPreserveAttachments = NULL;
	
	VkSubpassDependency SubpassDependency;
	SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	SubpassDependency.dstSubpass = 0;
	SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	SubpassDependency.srcAccessMask = 0;
	SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	SubpassDependency.dependencyFlags = 0;
	
	VkRenderPassCreateInfo RenderPassCreateInfo;
	RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassCreateInfo.pNext = NULL;
	RenderPassCreateInfo.flags = 0;
	RenderPassCreateInfo.attachmentCount = 1;
	RenderPassCreateInfo.pAttachments = &AttachmentDescription;
	RenderPassCreateInfo.subpassCount = 1;
	RenderPassCreateInfo.pSubpasses = &SubpassDescription;
	RenderPassCreateInfo.dependencyCount = 1;
	RenderPassCreateInfo.pDependencies = &SubpassDependency;
	
	VkResult result = vkCreateRenderPass(device, &RenderPassCreateInfo, NULL, &RenderPass);
	ASSERT_VULKAN(result);
}

void CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding;
	DescriptorSetLayoutBinding.binding = 0;													//Shader Binding Position
	DescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorSetLayoutBinding.descriptorCount = 1;
	DescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	DescriptorSetLayoutBinding.pImmutableSamplers = NULL;
	
	VkDescriptorSetLayoutBinding SamplerDescriptorSetLayoutBinding;
	SamplerDescriptorSetLayoutBinding.binding = 1;
	SamplerDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	SamplerDescriptorSetLayoutBinding.descriptorCount = 1;
	SamplerDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	SamplerDescriptorSetLayoutBinding.pImmutableSamplers = NULL;
	
	std::vector<VkDescriptorSetLayoutBinding> DescriptorSets;
	DescriptorSets.push_back(DescriptorSetLayoutBinding);
	DescriptorSets.push_back(SamplerDescriptorSetLayoutBinding);
	
	VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo;
	DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	DescriptorSetLayoutCreateInfo.pNext = NULL;
	DescriptorSetLayoutCreateInfo.flags = 0;
	DescriptorSetLayoutCreateInfo.bindingCount = DescriptorSets.size();
	DescriptorSetLayoutCreateInfo.pBindings = DescriptorSets.data();
	
	VkResult result = vkCreateDescriptorSetLayout(device, &DescriptorSetLayoutCreateInfo, NULL, &DescriptorSetLayout);
	ASSERT_VULKAN(result);
}

void CreatePipeline()
{
	auto ShaderCodeVert = readFile("VertexShader.spv");
	auto ShaderCodeFrag = readFile("FragmentShader.spv");
	
	CreateShaderModule(ShaderCodeVert, &ShaderModuleVert);
	CreateShaderModule(ShaderCodeFrag, &ShaderModuleFrag);
	
	VkPipelineShaderStageCreateInfo ShaderStageCreateInfoVert;
	ShaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStageCreateInfoVert.pNext = NULL;
	ShaderStageCreateInfoVert.flags = 0;
	ShaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
	ShaderStageCreateInfoVert.module = ShaderModuleVert;
	ShaderStageCreateInfoVert.pName = "main";
	ShaderStageCreateInfoVert.pSpecializationInfo = NULL;
	
	VkPipelineShaderStageCreateInfo ShaderStageCreateInfoFrag;
	ShaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStageCreateInfoFrag.pNext = NULL;
	ShaderStageCreateInfoFrag.flags = 0;
	ShaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	ShaderStageCreateInfoFrag.module = ShaderModuleFrag;
	ShaderStageCreateInfoFrag.pName = "main";
	ShaderStageCreateInfoFrag.pSpecializationInfo = NULL;
	
	VkPipelineShaderStageCreateInfo ShaderStages[] = { ShaderStageCreateInfoVert, ShaderStageCreateInfoFrag };
	
	auto VertexBindingDescription = Vertex::GetBindingDescription();
	auto VertexAttributeDescription = Vertex::GetAttributeDescription();
	
	VkPipelineVertexInputStateCreateInfo VertexInputCreateInfo;
	VertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputCreateInfo.pNext = NULL;
	VertexInputCreateInfo.flags = 0;
	VertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	VertexInputCreateInfo.pVertexBindingDescriptions = &VertexBindingDescription;
	VertexInputCreateInfo.vertexAttributeDescriptionCount = VertexAttributeDescription.size();
	VertexInputCreateInfo.pVertexAttributeDescriptions = VertexAttributeDescription.data();
	
	VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo;
	InputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputAssemblyCreateInfo.pNext = NULL;
	InputAssemblyCreateInfo.flags = 0;
	InputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;
	
	VkViewport Viewport;
	Viewport.x = 0.0f;
	Viewport.y = 0.0f;
	Viewport.width = Width;
	Viewport.height = Height;
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;
	
	VkRect2D Scissor;
	Scissor.offset = {0, 0};
	Scissor.extent = {Width, Height};
	
	VkPipelineViewportStateCreateInfo ViewportStateCreateInfo;
	ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ViewportStateCreateInfo.pNext = NULL;
	ViewportStateCreateInfo.flags = 0;
	ViewportStateCreateInfo.viewportCount = 1;
	ViewportStateCreateInfo.pViewports = &Viewport;
	ViewportStateCreateInfo.scissorCount = 1;
	ViewportStateCreateInfo.pScissors = &Scissor;
	
	VkPipelineRasterizationStateCreateInfo RasterCreateInfo;
	RasterCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	RasterCreateInfo.pNext = NULL;
	RasterCreateInfo.flags = 0;
	RasterCreateInfo.depthClampEnable = VK_FALSE;
	RasterCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	RasterCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;		//Poly Line or Fill
	RasterCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;			//VK_CULL_MODE_NONE	VK_CULL_MODE_BACK_BIT
	RasterCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	RasterCreateInfo.depthBiasEnable = VK_FALSE;
	RasterCreateInfo.depthBiasConstantFactor = 0.0f;
	RasterCreateInfo.depthBiasClamp = 0.0f;
	RasterCreateInfo.depthBiasSlopeFactor = 0.0f; 
	RasterCreateInfo.lineWidth = 1.0f;
	
	VkPipelineMultisampleStateCreateInfo MultisampleCreateInfo;
	MultisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	MultisampleCreateInfo.pNext = NULL;
	MultisampleCreateInfo.flags = 0;
	MultisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	MultisampleCreateInfo.sampleShadingEnable = VK_FALSE;					//Enable or Disable Multisample
	MultisampleCreateInfo.minSampleShading = 1.0f;
	MultisampleCreateInfo.pSampleMask = NULL;
	MultisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	MultisampleCreateInfo.alphaToOneEnable = VK_FALSE;
	
	VkPipelineColorBlendAttachmentState ColorBlendAttachment;
	ColorBlendAttachment.blendEnable = VK_TRUE;
	ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	
	VkPipelineColorBlendStateCreateInfo ColorBlendCreateInfo;
	ColorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ColorBlendCreateInfo.pNext = NULL;
	ColorBlendCreateInfo.flags = 0;
	ColorBlendCreateInfo.logicOpEnable = VK_FALSE;
	ColorBlendCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
	ColorBlendCreateInfo.attachmentCount = 1;
	ColorBlendCreateInfo.pAttachments = &ColorBlendAttachment;
	ColorBlendCreateInfo.blendConstants[0] = 0.0f;
	ColorBlendCreateInfo.blendConstants[1] = 0.0f;
	ColorBlendCreateInfo.blendConstants[2] = 0.0f;
	ColorBlendCreateInfo.blendConstants[3] = 0.0f;
	
	VkDynamicState DynamicStates[] = 
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	
	VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo;
	DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	DynamicStateCreateInfo.pNext = NULL;
	DynamicStateCreateInfo.flags = 0;
	DynamicStateCreateInfo.dynamicStateCount = 2;
	DynamicStateCreateInfo.pDynamicStates = DynamicStates;
	
	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo;
	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutCreateInfo.pNext = NULL;
	PipelineLayoutCreateInfo.flags = 0;
	PipelineLayoutCreateInfo.setLayoutCount = 1;
	PipelineLayoutCreateInfo.pSetLayouts = &DescriptorSetLayout;
	PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	PipelineLayoutCreateInfo.pPushConstantRanges = NULL;
	
	VkResult result = vkCreatePipelineLayout(device, &PipelineLayoutCreateInfo, NULL, &PipelineLayout); 
	ASSERT_VULKAN(result);	
	
	VkGraphicsPipelineCreateInfo PipelineCreateInfo;
	PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineCreateInfo.pNext = NULL;
	PipelineCreateInfo.flags = 0;
	PipelineCreateInfo.stageCount = 2;
	PipelineCreateInfo.pStages = ShaderStages;
	PipelineCreateInfo.pVertexInputState = &VertexInputCreateInfo;
	PipelineCreateInfo.pInputAssemblyState = &InputAssemblyCreateInfo;
	PipelineCreateInfo.pTessellationState = NULL;
	PipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
	PipelineCreateInfo.pRasterizationState = &RasterCreateInfo;
	PipelineCreateInfo.pMultisampleState = &MultisampleCreateInfo;
	PipelineCreateInfo.pDepthStencilState = NULL;
	PipelineCreateInfo.pColorBlendState = &ColorBlendCreateInfo;
	PipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
	PipelineCreateInfo.layout = PipelineLayout; 
	PipelineCreateInfo.renderPass = RenderPass;
	PipelineCreateInfo.subpass = 0;
	PipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	PipelineCreateInfo.basePipelineIndex = -1;
	
	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &PipelineCreateInfo, NULL, &Pipeline);
	ASSERT_VULKAN(result);
}

void CreateFramebuffers()
{
	Framebuffers = new VkFramebuffer[amountOfImagesInSwapchain];
	
	for(size_t i = 0; i < amountOfImagesInSwapchain; i++)
	{
		VkFramebufferCreateInfo FramebufferCreateInfo;
		FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferCreateInfo.pNext = NULL;
		FramebufferCreateInfo.flags = 0;
		FramebufferCreateInfo.renderPass = RenderPass;
		FramebufferCreateInfo.attachmentCount = 1;
		FramebufferCreateInfo.pAttachments = &(imageViews[i]);
		FramebufferCreateInfo.width = Width;
		FramebufferCreateInfo.height = Height;
		FramebufferCreateInfo.layers = 1;
		
		VkResult result = vkCreateFramebuffer(device, &FramebufferCreateInfo, NULL, &(Framebuffers[i]));
		ASSERT_VULKAN(result);
	}
}

void CreateCommandPool()
{
	VkCommandPoolCreateInfo CommandPoolCreateInfo;
	CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolCreateInfo.pNext = NULL;
	CommandPoolCreateInfo.flags = 0;
	CommandPoolCreateInfo.queueFamilyIndex = 0;
	
	VkResult result = vkCreateCommandPool(device, &CommandPoolCreateInfo, NULL, &CommandPool);
	ASSERT_VULKAN(result);
}

void CreateCommandBuffers()
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo;
	CommandBufferAllocateInfo.sType  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.pNext = NULL;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = amountOfImagesInSwapchain;
	
	CommandBuffers = new VkCommandBuffer[amountOfImagesInSwapchain];
	VkResult result = vkAllocateCommandBuffers(device, &CommandBufferAllocateInfo, CommandBuffers);
	ASSERT_VULKAN(result);
}

void CopyBuffer(VkBuffer Src, VkBuffer Dest, VkDeviceSize Size)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo;
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.pNext = NULL;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = 1;
	
	VkCommandBuffer CommandBuffer;
	VkResult result = vkAllocateCommandBuffers(device, &CommandBufferAllocateInfo, &CommandBuffer);
	ASSERT_VULKAN(result);
	
	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = NULL;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = NULL;
	
	result = vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
	ASSERT_VULKAN(result);
	
	VkBufferCopy BufferCopy;
	BufferCopy.srcOffset = 0;
	BufferCopy.dstOffset = 0;
	BufferCopy.size = Size;
	vkCmdCopyBuffer(CommandBuffer, Src, Dest, 1, &BufferCopy);
	
	result = vkEndCommandBuffer(CommandBuffer);
	ASSERT_VULKAN(result);
	
	VkSubmitInfo SubmitInfo;
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.waitSemaphoreCount = 0;
	SubmitInfo.pWaitSemaphores = NULL;
	SubmitInfo.pWaitDstStageMask = NULL;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffer;
	SubmitInfo.signalSemaphoreCount = 0;
	SubmitInfo.pSignalSemaphores = NULL;
	
	result = vkQueueSubmit(queue, 1, &SubmitInfo, VK_NULL_HANDLE);
	ASSERT_VULKAN(result);
	
	vkQueueWaitIdle(queue);
	
	vkFreeCommandBuffers(device, CommandPool, 1, &CommandBuffer);
}
template <typename T>
void CreateVertexBuffer(std::vector<T> Vertices)
{
	CreateAndUploadBuffer(device, physicalDevices[0], queue, CommandPool, Vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VertexBuffer, VertexBufferDeviceMemory);
}
template <typename T>
void CreateIndexBuffer(std::vector<T> Indices)
{
	CreateAndUploadBuffer(device, physicalDevices[0], queue, CommandPool, Indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, IndexBuffer, IndexBufferDeviceMemory);
}

void CreateUniformBuffer()
{
	VkDeviceSize BufferSize = sizeof(MVP);
	CreateBuffer(device, physicalDevices[0], BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, UniformBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBufferMemory);
}

void CreateDescriptorPool()
{
	VkDescriptorPoolSize DescriptorPoolSize;
	DescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorPoolSize.descriptorCount = 1;
	
	VkDescriptorPoolSize SamplerPoolSize;
	SamplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	SamplerPoolSize.descriptorCount = 1;
	
	std::vector <VkDescriptorPoolSize> DescriptorPoolSizes;
	DescriptorPoolSizes.push_back(DescriptorPoolSize);
	DescriptorPoolSizes.push_back(SamplerPoolSize);
	
	VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo;
	DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	DescriptorPoolCreateInfo.pNext = NULL;
	DescriptorPoolCreateInfo.flags = 0;
	DescriptorPoolCreateInfo.maxSets = 1;
	DescriptorPoolCreateInfo.poolSizeCount = DescriptorPoolSizes.size();
	DescriptorPoolCreateInfo.pPoolSizes = DescriptorPoolSizes.data();
	
	VkResult result = vkCreateDescriptorPool(device, &DescriptorPoolCreateInfo, NULL, &DescriptorPool);
	ASSERT_VULKAN(result);
}

void CreateDescriptorSet()
{
	VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo;
	DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescriptorSetAllocateInfo.pNext = NULL;
	DescriptorSetAllocateInfo.descriptorPool = DescriptorPool;
	DescriptorSetAllocateInfo.descriptorSetCount = 1;
	DescriptorSetAllocateInfo.pSetLayouts = &DescriptorSetLayout;
	
	VkResult result = vkAllocateDescriptorSets(device, &DescriptorSetAllocateInfo, &DescriptorSet);
	ASSERT_VULKAN(result);
	
	VkDescriptorBufferInfo DescriptorBufferInfo;
	DescriptorBufferInfo.buffer = UniformBuffer;
	DescriptorBufferInfo.offset = 0;
	DescriptorBufferInfo.range = sizeof(MVP);
	
	VkWriteDescriptorSet DescriptorWrite;
	DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	DescriptorWrite.pNext = NULL;
	DescriptorWrite.dstSet = DescriptorSet;
	DescriptorWrite.dstBinding = 0;
	DescriptorWrite.dstArrayElement = 0;
	DescriptorWrite.descriptorCount = 1;
	DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorWrite.pImageInfo = NULL;
	DescriptorWrite.pBufferInfo = &DescriptorBufferInfo;
	DescriptorWrite.pTexelBufferView = NULL;
	
	VkDescriptorImageInfo DescriptorImageInfo;
	DescriptorImageInfo.sampler = Sampler;
	DescriptorImageInfo.imageView = ImageView;
	DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	
	VkWriteDescriptorSet DescriptorSampler;
	DescriptorSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	DescriptorSampler.pNext = NULL;
	DescriptorSampler.dstSet = DescriptorSet;
	DescriptorSampler.dstBinding = 1;
	DescriptorSampler.dstArrayElement = 0;
	DescriptorSampler.descriptorCount = 1;
	DescriptorSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	DescriptorSampler.pImageInfo = &DescriptorImageInfo;
	DescriptorSampler.pBufferInfo = NULL;
	DescriptorSampler.pTexelBufferView = NULL;
	
	std::vector<VkWriteDescriptorSet> WriteDescriptorSets;
	WriteDescriptorSets.push_back(DescriptorWrite);
	WriteDescriptorSets.push_back(DescriptorSampler);
	
	vkUpdateDescriptorSets(device, WriteDescriptorSets.size(), WriteDescriptorSets.data(), 0, NULL);
}

void RecordCommandBuffers()
{
	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = NULL;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = NULL;
	
	for(size_t i = 0; i < amountOfImagesInSwapchain; i++)
	{
		VkResult result = vkBeginCommandBuffer(CommandBuffers[i], &CommandBufferBeginInfo);
		ASSERT_VULKAN(result);
		
		VkRenderPassBeginInfo RenderPassBeginInfo;
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.pNext = NULL;
		RenderPassBeginInfo.renderPass = RenderPass;
		RenderPassBeginInfo.framebuffer = Framebuffers[i];
		RenderPassBeginInfo.renderArea.offset = {0, 0};
		RenderPassBeginInfo.renderArea.extent = {Width, Height};
		VkClearValue ClearValue = {0.0f, 0.0f, 0.0f, 1.0f};				//Window Color
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearValue;
		
		vkCmdBeginRenderPass(CommandBuffers[i], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
		
		VkViewport Viewport;
		Viewport.x = 0.0f;
		Viewport.y = 0.0f;
		Viewport.width = Width;
		Viewport.height = Height;
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 1.0f;
		vkCmdSetViewport(CommandBuffers[i], 0, 1, &Viewport);
		
		VkRect2D Scissor;
		Scissor.offset = {0, 0};
		Scissor.extent = {Width, Height};
		vkCmdSetScissor(CommandBuffers[i], 0, 1, &Scissor);
		
		VkDeviceSize Offsets[] = {0};
		vkCmdBindVertexBuffers(CommandBuffers[i], 0, 1, &VertexBuffer, Offsets);
		vkCmdBindIndexBuffer(CommandBuffers[i], IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		
		vkCmdBindDescriptorSets(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSet, 0, NULL);
		
	//	vkCmdDraw(CommandBuffers[i], vertices1.size(), 1, 0, 0);
		vkCmdDrawIndexed(CommandBuffers[i], Indices.size(), 1, 0, 0, 0);
		
		vkCmdEndRenderPass(CommandBuffers[i]);
		
		result = vkEndCommandBuffer(CommandBuffers[i]);
		ASSERT_VULKAN(result);
	}
}

void CreateSemaphores()
{
	VkSemaphoreCreateInfo SemaphoreCreateInfo;
	SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	SemaphoreCreateInfo.pNext = NULL;
	SemaphoreCreateInfo.flags = 0;
	
	VkResult result = vkCreateSemaphore(device, &SemaphoreCreateInfo, NULL, &SemaphoreImageAvailable);
	ASSERT_VULKAN(result);
	result = vkCreateSemaphore(device, &SemaphoreCreateInfo, NULL, &SemaphoreRenderingDone);
	ASSERT_VULKAN(result);
}

void StartVulkan()
{
	CreateInstance();
	physicalDevices = GetAllPhysicalDevices();
	PrintInstanceLayers();
	PrintInstanceExtensions();																						
	CreateGlfwWindowSurface();
	PrintStatsOfAllPhysicalDevices();
	CreateLogicalDevice();
	InitQueue();
	CheckSurfaceSupport();
	CreateSwapchain();
	CreateImageViews();
	CreateRenderPass();
	CreateDescriptorSetLayout();
	CreatePipeline();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffers();
	
	LoadTexture("texture.jpg");
	Upload(device, physicalDevices[0], CommandPool, queue);
	
	CreateVertexBuffer(Vertices);
	CreateIndexBuffer(Indices);
	CreateUniformBuffer();
	CreateDescriptorPool();
	CreateDescriptorSet();
	RecordCommandBuffers();
	CreateSemaphores();
}

void RecreateSwapchain()
{
	vkDeviceWaitIdle(device);
	
	vkFreeCommandBuffers(device, CommandPool, amountOfImagesInSwapchain, CommandBuffers);
	delete[] CommandBuffers;
	
	vkDestroyCommandPool(device, CommandPool, NULL);
	
	for(size_t i = 0; i < amountOfImagesInSwapchain; i++)
	{
		vkDestroyFramebuffer(device, Framebuffers[i], NULL);
	}
	delete[] Framebuffers;
	
//	vkDestroyPipeline(device, Pipeline, NULL);
	vkDestroyRenderPass(device, RenderPass, NULL);
	for(int i = 0; i < amountOfImagesInSwapchain; i++)
	{
		vkDestroyImageView(device, imageViews[i], NULL);
	}
	delete[] imageViews;
//	vkDestroyPipelineLayout(device, PipelineLayout, NULL);
//	vkDestroyShaderModule(device, ShaderModuleVert, NULL);
//	vkDestroyShaderModule(device, ShaderModuleFrag, NULL);
	
	VkSwapchainKHR oldSwapchain = swapchain;
	
	CreateSwapchain();
	CreateImageViews();
	CreateRenderPass();
//	CreatePipeline();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffers();
	RecordCommandBuffers();
	
	vkDestroySwapchainKHR(device, oldSwapchain, NULL);
}

void DrawFrame()
{
	uint32_t ImageIndex;
	vkAcquireNextImageKHR(device, swapchain, 2000, SemaphoreImageAvailable, VK_NULL_HANDLE, &ImageIndex);
	
	VkSubmitInfo SubmitInfo;
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.waitSemaphoreCount = 1;
	SubmitInfo.pWaitSemaphores = &SemaphoreImageAvailable;
	VkPipelineStageFlags WaitStageMask[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	SubmitInfo.pWaitDstStageMask = WaitStageMask;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &(CommandBuffers[ImageIndex]);
	SubmitInfo.signalSemaphoreCount = 1;
	SubmitInfo.pSignalSemaphores = &SemaphoreRenderingDone;
	
	VkResult result = vkQueueSubmit(queue, 1, &SubmitInfo, VK_NULL_HANDLE);
	ASSERT_VULKAN(result);
	
	VkPresentInfoKHR PresentInfo;
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = &SemaphoreRenderingDone;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &swapchain;
	PresentInfo.pImageIndices = &ImageIndex;
	PresentInfo.pResults = NULL;
	
	result = vkQueuePresentKHR(queue, &PresentInfo);
	ASSERT_VULKAN(result);
}

glm::mat4 PerspectivMatrix( float aspect_ratio, float field_of_view, float near_plane, float far_plane ) 
{
    float f = 1.0f / tan( glm::radians( 0.5f * field_of_view ) );

    glm::mat4 perspective_projection_matrix = 
	{
      f / aspect_ratio,
      0.0f,
      0.0f,
      0.0f,

      0.0f,
      -f,
      0.0f,
      0.0f,

      0.0f,
      0.0f,
      far_plane / (near_plane - far_plane),
      -1.0f,
      
      0.0f,
      0.0f,
      (near_plane * far_plane) / (near_plane - far_plane),
      0.0f
    };
    return perspective_projection_matrix;
  }

auto GameStartTime = std::chrono::high_resolution_clock::now();
void UpdateMVP()
{
	auto FrameTime = std::chrono::high_resolution_clock::now();
	
	float TimeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(FrameTime - GameStartTime).count() / 1000.0f;
	glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			  Model = glm::rotate(Model, TimeSinceStart * glm::radians(00.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			  Model = glm::rotate(Model, TimeSinceStart * glm::radians(000.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			  Model = glm::rotate(Model, TimeSinceStart * glm::radians(000.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			  Model = glm::scale(Model, glm::vec3(1.0, 1.0, 1.0));
	
	float cameraSpeed = 0.001;
	
	cameraPos += (cameraSpeed * cameraFront) * deltaForward;
	cameraPos += (glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed) * deltaSide;
	
	glm::mat4 View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
								 					 
	glm::mat4 Projection = glm::mat4(1.0f); 
	float aspect = Width/Height;
//	Projection = glm::perspective(glm::radians(120.0f), aspect, 0.01f, 100000.0f);
	Projection = PerspectivMatrix(aspect, 75.0f, 0.01f, 1000.0f);
//	Projection[1][1] *= -1;		//Or Projection[1][1] *= +1;	
	
	MVP = Projection * View * Model;
	
	void* Data;
	vkMapMemory(device, UniformBufferMemory, 0, sizeof(MVP), 0, &Data);
	memcpy(Data, &MVP, sizeof(MVP));
	vkUnmapMemory(device, UniformBufferMemory);
}

void Translate(float x, float y, float z)
{
	glm::mat4 Model;
	Model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	MVP = Model;
	
	void* Data;
	vkMapMemory(device, UniformBufferMemory, 0, sizeof(MVP), 0, &Data);
	memcpy(Data, &MVP, sizeof(MVP));
	vkUnmapMemory(device, UniformBufferMemory);
}

void GameLoop()
{
	
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();		
		
		UpdateMVP();
		
	//	Translate(0.0, 0.0, 0.0);
		DrawFrame();
	}
}

void ShutdownVulkan()
{
	vkDeviceWaitIdle(device);
	
	vkDestroyDescriptorSetLayout(device, DescriptorSetLayout, NULL);
	vkDestroyDescriptorPool(device, DescriptorPool, NULL);
	vkFreeMemory(device, UniformBufferMemory, NULL);
	vkDestroyBuffer(device, UniformBuffer, NULL);
	
	vkFreeMemory(device, IndexBufferDeviceMemory, NULL);
	vkDestroyBuffer(device, IndexBuffer, NULL);
	
	vkFreeMemory(device, VertexBufferDeviceMemory, NULL);
	vkDestroyBuffer(device, VertexBuffer, NULL);
	
	DestroyTexture();
	
	vkDestroySemaphore(device, SemaphoreImageAvailable, NULL);
	vkDestroySemaphore(device, SemaphoreRenderingDone, NULL);
	
	vkFreeCommandBuffers(device, CommandPool, amountOfImagesInSwapchain, CommandBuffers);
	delete[] CommandBuffers;
	
	vkDestroyCommandPool(device, CommandPool, NULL);
	
	for(size_t i = 0; i < amountOfImagesInSwapchain; i++)
	{
		vkDestroyFramebuffer(device, Framebuffers[i], NULL);
	}
	delete[] Framebuffers;
	
	vkDestroyPipeline(device, Pipeline, NULL);
	vkDestroyRenderPass(device, RenderPass, NULL);
	for(int i = 0; i < amountOfImagesInSwapchain; i++)
	{
		vkDestroyImageView(device, imageViews[i], NULL);
	}
	delete[] imageViews;
	vkDestroyPipelineLayout(device, PipelineLayout, NULL);
	vkDestroyShaderModule(device, ShaderModuleVert, NULL);
	vkDestroyShaderModule(device, ShaderModuleFrag, NULL);
	vkDestroySwapchainKHR(device, swapchain, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroySurfaceKHR(instance, surface, NULL);
	vkDestroyInstance(instance, NULL);
}

void ShutdownGLFW()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main()
{
	system("runCompiler.bat");
	StartGLFW();
	StartVulkan();
	GameLoop();
	ShutdownVulkan();
	ShutdownGLFW();
	
	return 0;
}
