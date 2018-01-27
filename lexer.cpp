#include "lexer.h"

namespace spring {

std::vector<std::string> Token::typenames;
std::vector<std::regex> Token::rules;
bool Token::inited{false};

} // namespace spring
