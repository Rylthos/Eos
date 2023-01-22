#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Change GLM for Vulkan for Users
#define GLM_FORCE_LEFT_HANDED // Change GLM for Vulkan for Users

// Libraries
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Core
#include "Core/EntryPoint.hpp"
#include "Core/Application.hpp"
#include "Core/Window.hpp"
#include "Core/Logger.hpp"
#include "Core/DeletionQueue.hpp"
#include "Core/Timer.hpp"

// Core / Cameras
#include "Core/Cameras/Orthographic.hpp"
#include "Core/Cameras/Perspective.hpp"


// Engine
#include "Engine/Buffer.hpp"
#include "Engine/ComputeShader.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GlobalData.hpp"
#include "Engine/Initializers.hpp"
#include "Engine/Mesh.hpp"
#include "Engine/RenderPassBuilder.hpp"
#include "Engine/Shader.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Types.hpp"

// Engine / Descriptor Sets
#include "Engine/DescriptorSets/DescriptorAllocator.hpp"
#include "Engine/DescriptorSets/DescriptorLayoutCache.hpp"
#include "Engine/DescriptorSets/DescriptorBuilder.hpp"

// Engine / Pipelines
#include "Engine/Pipelines/ComputePipelineBuilder.hpp"
#include "Engine/Pipelines/PipelineBuilder.hpp"
#include "Engine/Pipelines/PipelineCreationInfo.hpp"

// Engine / Submits
#include "Engine/Submits/TransferSubmit.hpp"
#include "Engine/Submits/GraphicsSubmit.hpp"


// Events
#include "Events/EventDispatcher.hpp"
#include "Events/EventCodes.hpp"
#include "Events/Events.hpp"


#include "stbInclude.hpp"
