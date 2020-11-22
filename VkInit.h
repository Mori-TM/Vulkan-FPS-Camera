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
	
	VkPipelineVertexInputStateCreateInfo VertexInputCreateInfo;
	VertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputCreateInfo.pNext = NULL;
	VertexInputCreateInfo.flags = 0;
	VertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	VertexInputCreateInfo.pVertexBindingDescriptions = NULL;
	VertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
	VertexInputCreateInfo.pVertexAttributeDescriptions = NULL;
	
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
	RasterCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	RasterCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	RasterCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;	//Or VK_FRONT_FACE_COUNTER_CLOCKWISE
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
	
	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo;
	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutCreateInfo.pNext = NULL;
	PipelineLayoutCreateInfo.flags = 0;
	PipelineLayoutCreateInfo.setLayoutCount = 0;
	PipelineLayoutCreateInfo.pSetLayouts = NULL;
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
	PipelineCreateInfo.pDynamicState = NULL;
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
		
		vkCmdDraw(CommandBuffers[i], 3, 1, 0, 0);
		
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
