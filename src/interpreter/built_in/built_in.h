#pragma once
#include <expected>
#include <functional>
#include <string>
#include <span>
#include <unordered_map>
#include <variant>

enum TYPES : uint16_t {
    NUM_TYPE,
    STRING_TYPE,
    NIL_TYPE,
    BOOL_TYPE,
    LIST_TYPE,
    FUNC_TYPE,
    NOT_SET_TYPE
};

namespace Memory {
    class StackFrame;

    struct StringHolder;
    struct ListHolder;
    struct BuiltInFuncHolder;
    struct UserSetFuncHolder;

    using Holder = std::variant<
        std::monostate, double, bool,        // integral types
        std::shared_ptr<StringHolder>,       // string type
        std::shared_ptr<ListHolder>,         // list type
        std::shared_ptr<BuiltInFuncHolder>,  // built-in functions
        std::shared_ptr<UserSetFuncHolder>   // user set functions
    >;

    struct BuiltInFuncHolder {
        BuiltInFuncHolder(std::function<Holder(Holder&&)> f) : function(f) {}
        std::function<Holder(Holder&&)> function;
    };
}

extern std::unordered_map<std::string, Memory::Holder> BUILT_IN_FUNCTIONS;
extern std::unordered_map<TYPES, std::string> TYPE_TO_STR;

namespace BuiltIn {

// NUMBER AWARE FUNCTIONS
Memory::Holder abs(Memory::Holder&&);
Memory::Holder ceil(Memory::Holder&&);
Memory::Holder floor(Memory::Holder&&);
Memory::Holder round(Memory::Holder&&);
Memory::Holder sqrt(Memory::Holder&&);
Memory::Holder rnd(Memory::Holder&&);
Memory::Holder parse_num(Memory::Holder&&);
Memory::Holder to_string(Memory::Holder&&);

// STRING AWARE FUNCTIONS
Memory::Holder len(Memory::Holder&&);
Memory::Holder lower(Memory::Holder&&);
Memory::Holder upper(Memory::Holder&&);
Memory::Holder split(Memory::Holder&&);
Memory::Holder join(Memory::Holder&&);
Memory::Holder replace(Memory::Holder&&);

// LIST AWARE FUNCTIONS
Memory::Holder range(Memory::Holder&&);
Memory::Holder len(Memory::Holder&&);
Memory::Holder push(Memory::Holder&&);
Memory::Holder pop(Memory::Holder&&);
Memory::Holder insert(Memory::Holder&&);
Memory::Holder remove(Memory::Holder&&);
Memory::Holder sort(Memory::Holder&&);

// UNIVERSAL FUNCTIONS
Memory::Holder copy(Memory::Holder&&);

// SYSTEM FUNCTIONS
Memory::Holder print(Memory::Holder&&);
Memory::Holder println(Memory::Holder&&);
Memory::Holder read(Memory::Holder&&);
Memory::Holder stacktrace(Memory::Holder&&);

} // BuiltIn