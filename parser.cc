#include "parser.h"
#include "lexer.h"

namespace spring {

/*
 * Parse a normal expression.
 * There are several patterns:
 * 1 + 2 ->             (+)
 *                      / \
 *                     1   2
 *
 * 1 - 2 * 3 ->         (-)
 *                    /     \
 *                   1      (*)
 *                         /   \
 *                        2     3
 *
 * 1 - 2 + 3
 *                       (+)
 *                     /     \
 *                   (-)      3
 *                  /   \
 *                 1     2
 */

}  // namespace spring
