#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Change GLM for Vulkan for Users

// Libraries
#include <GLFW/glfw3.h>
#include <Vulkan/Vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Core
#include "Core/EntryPoint.hpp"
#include "Core/Application.hpp"
#include "Core/Window.hpp"
#include "Core/Logger.hpp"


// Engine
#include "Engine/Engine.hpp"
#include "Engine/Mesh.hpp"
#include "Engine/PipelineBuilder.hpp"
#include "Engine/Shader.hpp"

// Engine / Descriptor Sets
#include "Engine/DescriptorSets/DescriptorAllocator.hpp"
#include "Engine/DescriptorSets/DescriptorLayoutCache.hpp"
#include "Engine/DescriptorSets/DescriptorBuilder.hpp"


// Events
#include "/Events/EventDispatcher.hpp"
#include "/Events/EventCodes.hpp"
#include "/Events/Events.hpp"


// Util
#include "Util/PipelineCreation.hpp"
#include "Util/Types.hpp"
#include "Util/Timer.hpp"

// Util / Cameras
#include "Util/Cameras/BaseCamera.hpp"
#include "Util/Cameras/Orthographic.hpp"
