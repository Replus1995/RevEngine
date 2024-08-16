#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"

#define REV_VK_CHECK(FUNC_RES) REV_CORE_ASSERT(FUNC_RES == VK_SUCCESS)
#define REV_VK_CHECK_THROW(FUNC_RES, ERR_MSG) if (FUNC_RES != VK_SUCCESS) throw std::runtime_error(ERR_MSG);

#define REV_VK_FRAME_OVERLAP 2