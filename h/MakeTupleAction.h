#pragma once

#include "Action.h"
#include <vector>

ActionPtr makeTupleAction(std::vector<ActionPtr>& sourceActionsIn);

