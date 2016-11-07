#include "../h/Token.h"
#include "../h/ErrorHandler.h"

#include <vector>
using std::vector;

void lexString(string text, vector<Token>& tokens)
{
	tokens.push_back(Token("text", "fsgfd", 54, 44, Token::OPERATOR));
}
