#pragma once

#include "Action.h"

ActionPtr varGetAction(size_t in, Type typeIn, string textIn);
ActionPtr varSetAction(size_t in, Type typeIn, string textIn);
ActionPtr constGetAction(void* in, Type typeIn, string textIn);
