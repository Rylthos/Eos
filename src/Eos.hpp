#pragma once

// Libraries
#include <GLFW/glfw3.h>
#include <Vulkan/Vulkan.h>
#include <glm/glm.hpp>

// Core
#include "Eos/Core/EntryPoint.hpp"
#include "Eos/Core/Application.hpp"
#include "Eos/Core/Window.hpp"
#include "Eos/Core/Logger.hpp"

// Engine
#include "Eos/Engine/Engine.hpp"
#include "Eos/Engine/Mesh.hpp"
#include "Eos/Engine/PipelineBuilder.hpp"
#include "Eos/Engine/Shader.hpp"

#include "Eos/Engine/DescriptorSets/DescriptorAllocator.hpp"
#include "Eos/Engine/DescriptorSets/DescriptorLayoutCache.hpp"
#include "Eos/Engine/DescriptorSets/DescriptorBuilder.hpp"

// Util
#include "Eos/Util/PipelineCreation.hpp"
#include "Eos/Util/Types.hpp"
