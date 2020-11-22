#define STB_IMAGE_IMPLEMENTATION
#include <images/stb_image.h>

unsigned int texture[512];

struct
{
	int width = 0;
	int height = 0;
	int bitdepth = 0;
	int SizeInByte = 0;
}ImageInfo;


stbi_uc *Data = NULL;

bool Uploaded = false;
VkImage Image;
VkDeviceMemory ImageMemory;
VkImageView ImageView;
VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
VkDevice ImageDevice;
VkSampler Sampler;

//m_image = Image
//m_imageMemory = ImageMemory
//m_ppixels = Data
//getSampler = Sampler
//getImageView = ImageView

void ChangeLayout(VkDevice Device, VkCommandPool CommandPool, VkQueue Queue, VkImageLayout Layout);
void WriteBufferToImage(VkDevice Device, VkCommandPool CommandPool, VkQueue Queue, VkBuffer Buffer);

void LoadTexture(const char *Path)
{
	stbi_set_flip_vertically_on_load(true);
	
	Data = stbi_load(Path, &ImageInfo.width, &ImageInfo.height, &ImageInfo.bitdepth, STBI_rgb_alpha);
	ImageInfo.SizeInByte = ImageInfo.width * ImageInfo.height * 4;
	printf("SizeInByte: %d\n", ImageInfo.SizeInByte);
	printf("Width: %d\n", ImageInfo.width);
	printf("Height: %d\n", ImageInfo.height);
	printf("Bitdepth: %d\n", ImageInfo.bitdepth);
	
	if(Data == NULL) printf("Cannot Open Texture\n");
}

void Upload(VkDevice Device, VkPhysicalDevice PhysicalDevice, VkCommandPool CommandPool, VkQueue Queue)
{	
	ImageDevice = Device;
	
	VkDeviceSize ImageSize = ImageInfo.SizeInByte;
	
	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory = 0;
	
	CreateBuffer(Device, PhysicalDevice, ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, StagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBufferMemory);
	
	void *data = NULL;
	vkMapMemory(Device, StagingBufferMemory, 0, ImageSize, 0, &data);
	memcpy(data, Data, ImageSize);
	
	vkUnmapMemory(Device, StagingBufferMemory);
	
	VkImageCreateInfo ImageCreateInfo;
	ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageCreateInfo.pNext = NULL;
	ImageCreateInfo.flags = 0;
	ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	ImageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	ImageCreateInfo.extent.width = ImageInfo.width;
	ImageCreateInfo.extent.height = ImageInfo.height;
	ImageCreateInfo.extent.depth = 1;
	ImageCreateInfo.mipLevels = 1;
	ImageCreateInfo.arrayLayers = 1;
	ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	ImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ImageCreateInfo.queueFamilyIndexCount = 0;
	ImageCreateInfo.pQueueFamilyIndices = NULL;
	ImageCreateInfo.initialLayout = ImageLayout;
	
	VkResult result = vkCreateImage(Device, &ImageCreateInfo, NULL, &Image);
	ASSERT_VULKAN(result);
	
	VkMemoryRequirements MemoryRequirements;
	vkGetImageMemoryRequirements(Device, Image, &MemoryRequirements);
	
	VkMemoryAllocateInfo MemoryAllocateInfo;
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.pNext = NULL;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(PhysicalDevice, MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	
	result = vkAllocateMemory(Device, &MemoryAllocateInfo, NULL, &ImageMemory);
	ASSERT_VULKAN(result);
	
	vkBindImageMemory(Device, Image, ImageMemory, 0);
	
	ChangeLayout(Device, CommandPool, Queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	WriteBufferToImage(Device, CommandPool, Queue, StagingBuffer);
	ChangeLayout(Device, CommandPool, Queue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	
	
	vkDestroyBuffer(Device, StagingBuffer, NULL);
	vkFreeMemory(Device, StagingBufferMemory, VK_NULL_HANDLE);

	VkImageViewCreateInfo ImageViewCreateInfo;
	ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ImageViewCreateInfo.pNext = NULL;
	ImageViewCreateInfo.flags = 0;
	ImageViewCreateInfo.image = Image;
	ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ImageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;		//If Not working VK_COMPONENT_SWIZZLE_IDENTITY
	ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	ImageViewCreateInfo.subresourceRange.levelCount = 1;
	ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	ImageViewCreateInfo.subresourceRange.layerCount = 1;
	
	result = vkCreateImageView(Device, &ImageViewCreateInfo, NULL, &ImageView);
	ASSERT_VULKAN(result);
	
	VkSamplerCreateInfo SamplerCreateInfo;
	SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	SamplerCreateInfo.pNext = NULL;
	SamplerCreateInfo.flags = 0;
	SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerCreateInfo.mipLodBias = 0.0f;
	SamplerCreateInfo.anisotropyEnable = VK_TRUE;
	SamplerCreateInfo.maxAnisotropy = 16;
	SamplerCreateInfo.compareEnable = VK_FALSE;
	SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	SamplerCreateInfo.minLod = 0.0f;
	SamplerCreateInfo.maxLod = 0.0f;
	SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	
	result = vkCreateSampler(Device, &SamplerCreateInfo, NULL, &Sampler);
	ASSERT_VULKAN(result);
	
	Uploaded = true;
}

void ChangeLayout(VkDevice Device, VkCommandPool CommandPool, VkQueue Queue, VkImageLayout Layout)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo;
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.pNext = NULL;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = 1;
	
	VkCommandBuffer CommandBuffer;
	VkResult result = vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, &CommandBuffer);
	ASSERT_VULKAN(result);
	
	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = NULL;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = NULL;
	
	result = vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
	ASSERT_VULKAN(result);
	
	
	VkImageMemoryBarrier ImageMemoryBarrier;
	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier.pNext = NULL;
	if(ImageLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && Layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if(ImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && Layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else
	{
		printf("Layout transition not supported");
	}
	
	ImageMemoryBarrier.oldLayout = ImageLayout;
	ImageMemoryBarrier.newLayout = Layout;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.image = Image;
	ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	ImageMemoryBarrier.subresourceRange.levelCount = 1;
	ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	ImageMemoryBarrier.subresourceRange.layerCount = 1;
	
	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &ImageMemoryBarrier);
	
	vkEndCommandBuffer(CommandBuffer);
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
	
	vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Queue);
	
	vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
	
	ImageLayout = Layout;
}

void WriteBufferToImage(VkDevice Device, VkCommandPool CommandPool, VkQueue Queue, VkBuffer Buffer)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo;								/*Begin Put into helper*/
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.pNext = NULL;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = 1;
	
	VkCommandBuffer CommandBuffer;
	VkResult result = vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, &CommandBuffer);
	ASSERT_VULKAN(result);
	
	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = NULL;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = NULL;
	
	result = vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
	ASSERT_VULKAN(result);															/*End Put into helper*/
	
	
	VkBufferImageCopy BufferImageCopy;
	BufferImageCopy.bufferOffset = 0;
	BufferImageCopy.bufferRowLength = 0;
	BufferImageCopy.bufferImageHeight = 0;
	BufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	BufferImageCopy.imageSubresource.mipLevel = 0;
	BufferImageCopy.imageSubresource.baseArrayLayer = 0;
	BufferImageCopy.imageSubresource.layerCount = 1;
	BufferImageCopy.imageOffset = {0, 0, 0};
	BufferImageCopy.imageExtent = {(uint32_t)ImageInfo.width, (uint32_t)ImageInfo.height, 1};
	
	vkCmdCopyBufferToImage(CommandBuffer, Buffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &BufferImageCopy);
	
	
	vkEndCommandBuffer(CommandBuffer);												/*Begin Put into helper*/
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
	
	vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Queue);
	
	vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);					/*End Put into helper*/
}

void DestroyTexture()
{
	stbi_image_free(Data);
	vkDestroySampler(ImageDevice, Sampler, NULL);
	vkDestroyImageView(ImageDevice, ImageView, NULL);
	
	vkDestroyImage(ImageDevice, Image, NULL);
	vkFreeMemory(ImageDevice, ImageMemory, NULL);
}
