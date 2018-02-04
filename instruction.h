#ifndef SPRING_INSTRUCTION_H
#define SPRING_INSTRUCTION_H
namespace spring {

enum class Instr {
  // names  args   description
  EQ = 0, // a,b    v(a) = 123
  ADD,    // a,b,c  v(a) = v(b) + v(c)
  SUB,    // a,b,c  v(a) = v(b) - v(c)
  MUL,
  DIV,
  MOD,
  // comparation operators.
  EQS, //    a,b,c  v(a) = v(b) == v(c)
  GT,  //    a,b,c  v(a) = v(b) > v(c)
  GE,
  LT,
  LE
};

/*
 * Variable scopes. Each block has a local scope that stores temporary
 * variables, after the block's executation finished, the local scope will be
 * freed.
 */
struct Scope {};

/*
 * A block for if/else/function.
 */
struct Block {};

/*
 * Frame for a function invoke.
 */
struct Frame {
  Block *block;
  Frame *pre;
};

/*
 * Instructions.
 */
struct InstrArchive {
  std::vector<Frame> frame_stack;
};

} // namespace spring
#endif
