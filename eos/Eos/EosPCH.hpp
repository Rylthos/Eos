#pragma once

#define EOS_VK_CHECK(x) \
    do \
    { \
        VkResult err = x; \
        if (err) \
        { \
            EOS_CORE_LOG_ERROR("Vulkan Error: {}", err); \
            abort(); \
        } \
    } while (0) \

#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Changes GLM to work with Vulkan for Eos
#define GLM_FORCE_LEFT_HANDED // Change GLM for Vulkan for Users

#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <concepts>
#include <optional>
#include <memory>

#include "Eos/Core/Core.hpp"
#include "Eos/Core/Logger.hpp"

#include "Eos/Engine/Types.hpp"

#include "stbInclude.hpp"
