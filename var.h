#ifndef SPRING_VARIABLE_H
#define SPRING_VARIABLE_H

#include <string>
#include <cstdint>
#include <vector>

namespace spring {

struct Variable;

struct Object {
  std::string name;
  int id;
  std::vector<Variable> static_members;
  vector<uint32_t> funcs;
};

struct ObjectV {
  uint32_t obj;
  std::vector<Variable> members;
};

enum class VarType {
  kInt = 0,
  kFloat,
  kString,
  kBool,
  kObject
};

union VarData {
  int32_t intv;
  float floatv;
  bool boolv;
  std::string* stringv;
  ObjectV* object;
};

struct Variable {
  VarType type;
  VarData data;
};

}  // namespace spring

#endif //SPRING_VARIABLE_H