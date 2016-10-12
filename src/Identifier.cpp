#include "../h/Identifier.h"

Identifier::~Identifier()
{
	for (auto i=actions.begin(); i!=actions.end(); ++i)
		delete *i;
}

