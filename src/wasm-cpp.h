#ifndef WASM_CPP
#define WASM_CPP

#include "wasm.h"
#include "wasm-ast.h"
#include "wasm-parse.h"

#include <string>
#include <unordered_map>
#include <cassert>
#include <cstring>

namespace wasm {

#define EACH_CALLBACK0 \
  CALLBACK(after_nop, void) \
  CALLBACK(before_block, WasmParserCookie)

#define EACH_CALLBACK1 \
  CALLBACK(before_module, void, WasmModule*)

#define EACH_CALLBACK2 \
  CALLBACK(before_function, void, WasmModule*, WasmFunction*) \
  CALLBACK(after_block, void, int, WasmParserCookie) \
  CALLBACK(after_export, void, WasmModule*, WasmExport*)

#define EACH_CALLBACK3 \
  CALLBACK(after_function, void, WasmModule*, WasmFunction*, int)   \

class Parser {
public:
  Parser(const char* start, const char* end) {
    source_.start = start;
    source_.end = end;
    tokenizer_.source = source_;
    tokenizer_.loc.pos = source_.start;
    tokenizer_.loc.line = 1;
    tokenizer_.loc.col = 1;

    memset(&parser, 0x0, sizeof(parser));
    parser.user_data = this;

#define CALLBACK(name, retty, ...)                  \
    parser.name = wrapper_##name;

    EACH_CALLBACK0
    EACH_CALLBACK1
    EACH_CALLBACK2
    EACH_CALLBACK3
#undef CALLBACK

  }
  void Parse() {
    wasm_parse_file(&parser, &tokenizer_);
  }

  Module module;

 private:
  virtual void Unimplemented(const char* name);

#define CALLBACK(name, retty, ...) \
  retty name(__VA_ARGS__);

  EACH_CALLBACK0
  EACH_CALLBACK1
  EACH_CALLBACK2
  EACH_CALLBACK3
#undef CALLBACK

  WasmParser parser = {};
  WasmSource source_;
  WasmTokenizer tokenizer_;

  std::unordered_map<WasmFunction*, Function*> functions_;

  std::vector<std::unique_ptr<Expression>>* insertion_point_;
  void insert(Expression* ex) {
    assert(insertion_point_);
    insertion_point_->emplace_back(ex);
  }


  // The callbacks unfortunately are split by arity because we need to
  // interleave the va-args (which are the arg types) with arg names, and
  // variadic macros don't have a way to do that.
#define CALLBACK(name, retty) \
  static retty wrapper_##name(void* user) {\
    return static_cast<Parser*>(user)->name();\
  }
  EACH_CALLBACK0
#undef CALLBACK

#define CALLBACK(name, retty, arg1ty) \
  static retty wrapper_##name(arg1ty arg1, void* user) {           \
    return static_cast<Parser*>(user)->name(arg1);\
  }
  EACH_CALLBACK1
#undef CALLBACK

#define CALLBACK(name, retty, arg1ty, arg2ty)                             \
  static retty wrapper_##name(arg1ty arg1, arg2ty arg2, void* user) {    \
    return static_cast<Parser*>(user)->name(arg1, arg2);                     \
  }
  EACH_CALLBACK2
#undef CALLBACK

#define CALLBACK(name, retty, arg1ty, arg2ty, arg3ty)                          \
  static retty wrapper_##name(arg1ty arg1, arg2ty arg2, arg3ty arg3, void* user) { \
    return static_cast<Parser*>(user)->name(arg1, arg2, arg3);                 \
  }
  EACH_CALLBACK3
#undef CALLBACK

};

} // namespace wasm
#endif // WASM_CPP
