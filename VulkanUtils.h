#define ASSERT_VULKAN(val)		\
		if(val != VK_SUCCESS)	\
		{						\
			printf("Error\n");	\
			exit(0);			\
		}	
		
VkInstance instance;
std::vector<VkPhysicalDevice> physicalDevices;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain = VK_NULL_HANDLE;
VkImageView *imageViews;
VkFramebuffer *Framebuffers;
uint32_t amountOfImagesInSwapchain = 0;
VkShaderModule ShaderModuleVert;
VkShaderModule ShaderModuleFrag;
VkPipelineLayout PipelineLayout;
VkRenderPass RenderPass;
VkPipeline Pipeline;
VkCommandPool CommandPool;
VkCommandBuffer *CommandBuffers;
VkSemaphore SemaphoreImageAvailable;
VkSemaphore SemaphoreRenderingDone;
VkQueue queue;
VkBuffer VertexBuffer;
VkDeviceMemory VertexBufferDeviceMemory;
VkBuffer IndexBuffer;
VkDeviceMemory IndexBufferDeviceMemory;
VkBuffer UniformBuffer;
VkDeviceMemory UniformBufferMemory;

GLFWwindow *window;

uint32_t Width = 800;
uint32_t Height = 800;
const VkFormat BGRA = VK_FORMAT_B8G8R8A8_UNORM;

glm::mat4 MVP;
VkDescriptorSetLayout DescriptorSetLayout;
VkDescriptorPool DescriptorPool;
VkDescriptorSet DescriptorSet;


uint32_t FindMemoryTypeIndex(VkPhysicalDevice PhysicalDevice, uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
{
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);
	for(uint32_t i = 0; i < PhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if((TypeFilter & (1 << i)) && (PhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
		{
			return i;
		}
	}
	
	printf("Found no correct Memory Type");
}


void CreateBuffer(VkDevice Device, VkPhysicalDevice PhysicalDevice, VkDeviceSize DeviceSize, VkBufferUsageFlags BufferUsageFlags, VkBuffer &Buffer, VkMemoryPropertyFlags MemoryPropertyFlags, VkDeviceMemory &DeviceMemory)
{
	VkBufferCreateInfo BufferCreateInfo;
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = NULL;
	BufferCreateInfo.flags = 0;
	BufferCreateInfo.size = DeviceSize;
	BufferCreateInfo.usage = BufferUsageFlags;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferCreateInfo.queueFamilyIndexCount = 0;
	BufferCreateInfo.pQueueFamilyIndices = NULL;
	
	VkResult result = vkCreateBuffer(device, &BufferCreateInfo, NULL, &Buffer);
	ASSERT_VULKAN(result);
	
	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(device, Buffer, &MemoryRequirements);
	
	VkMemoryAllocateInfo MemoryAllocateInfo;
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = NULL;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(PhysicalDevice, MemoryRequirements.memoryTypeBits, MemoryPropertyFlags);

	result = vkAllocateMemory(device, &MemoryAllocateInfo, NULL, &DeviceMemory);
	ASSERT_VULKAN(result);
	
	vkBindBufferMemory(device, Buffer, DeviceMemory, 0);
}

void CopyBuffer(VkBuffer Src, VkBuffer Dest, VkDeviceSize Size);

template <typename T>
void CreateAndUploadBuffer(VkDevice Device, VkPhysicalDevice PhysicalDevice, VkQueue Queue, VkCommandPool CommandPool, std::vector<T> Data, VkBufferUsageFlags Usage, VkBuffer &Buffer, VkDeviceMemory &DeviceMemory)
{
	VkDeviceSize BufferSize = sizeof(T) * Data.size();
	
	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;
	CreateBuffer(Device, PhysicalDevice, BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, StagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBufferMemory);
	
	
	void *RawData;
	vkMapMemory(device, StagingBufferMemory, 0, BufferSize, 0, &RawData);
	memcpy(RawData, Data.data(), BufferSize);
	vkUnmapMemory(device, StagingBufferMemory);
	
	CreateBuffer(Device, PhysicalDevice, BufferSize, Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, Buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, DeviceMemory);
	
	CopyBuffer(StagingBuffer, Buffer, BufferSize);
	
	vkDestroyBuffer(device, StagingBuffer, NULL);
	vkFreeMemory(device, StagingBufferMemory, NULL);
}
