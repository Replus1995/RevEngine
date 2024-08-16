#pragma once
#include "Rev/Core/Base.h"

#define REV_VK_CHECK(FUNC_RES, ERR_MSG) \
	if (FUNC_RES != VK_SUCCESS) throw std::runtime_error(ERR_MSG);

#define REV_VK_FRAME_OVERLAP 2