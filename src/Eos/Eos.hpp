#pragma once

// Libraries
#include <GLFW/glfw3.h>
#include <Vulkan/Vulkan.h>
#include <glm/glm.hpp>

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
