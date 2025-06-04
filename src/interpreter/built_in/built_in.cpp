#include "built_in.h"

std::unordered_map<TYPES, std::string> TYPE_TO_STR = {
    {TYPES::NUM_TYPE, "number"},
    {TYPES::STRING_TYPE, "string"},
    {TYPES::NIL_TYPE, "nil"},
    {TYPES::BOOL_TYPE, "bool"},
    {TYPES::LIST_TYPE, "list"},
    {TYPES::FUNC_TYPE, "func"},
    {TYPES::NOT_SET_TYPE, "(not set type)"}
};


namespace BuiltIn {

// NUMBER AWARE FUNCTIONS
/// @brief  absolute value of num (abs)
/// @return single number if one argument is addressed
/// @return list of abs values otherwise
HolderPack abs = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() == 0) { throw Errors::RunTime::ExpectedAtLeastOneArg(); }
            for (auto& holderpack : params) {
                if (holderpack->type != TYPES::NUM_TYPE) { throw Errors::TypeErrors::TypeErrorNum(); }
            }
            if (params.size() == 1) {
                return {std::abs(std::get<double>(params[0]->holder)), TYPES::NUM_TYPE};
            }
            std::vector<HolderPack> calcs;
            for (auto& hp : params) {
                calcs.emplace_back(std::abs(std::get<double>(hp->holder)), TYPES::NUM_TYPE);
            }
            return HolderPack(MakeListHolder(std::move(calcs)), TYPES::LIST_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  ceil value of num
/// @return single number if one argument is addressed
/// @return list of ceil values otherwise
HolderPack ceil = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() == 0) { throw Errors::RunTime::ExpectedAtLeastOneArg(); }
            for (auto& holderpack : params) {
                if (holderpack->type != TYPES::NUM_TYPE) { throw Errors::TypeErrors::TypeErrorNum(); }
            }
            if (params.size() == 1) {
                return {std::ceil(std::get<double>(params[0]->holder)), TYPES::NUM_TYPE};
            }
            std::vector<HolderPack> calcs;
            for (auto& hp : params) {
                calcs.emplace_back(std::ceil(std::get<double>(hp->holder)), TYPES::NUM_TYPE);
            }
            return HolderPack(MakeListHolder(std::move(calcs)), TYPES::LIST_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  floor value of num
/// @return single number if one argument is addressed
/// @return list of floor values otherwise
HolderPack floor = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() == 0) { throw Errors::RunTime::ExpectedAtLeastOneArg(); }
            for (auto& holderpack : params) {
                if (holderpack->type != TYPES::NUM_TYPE) { throw Errors::TypeErrors::TypeErrorNum(); }
            }
            if (params.size() == 1) {
                return {std::floor(std::get<double>(params[0]->holder)), TYPES::NUM_TYPE};
            }
            std::vector<HolderPack> calcs;
            for (auto& hp : params) {
                calcs.emplace_back(std::floor(std::get<double>(hp->holder)), TYPES::NUM_TYPE);
            }
            return HolderPack(MakeListHolder(std::move(calcs)), TYPES::LIST_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  round value of num
/// @return single number if one argument is addressed
/// @return list of round values otherwise
HolderPack round = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() == 0) { throw Errors::RunTime::ExpectedAtLeastOneArg(); }
            for (auto& holderpack : params) {
                if (holderpack->type != TYPES::NUM_TYPE) { throw Errors::TypeErrors::TypeErrorNum(); }
            }
            if (params.size() == 1) {
                return {std::round(std::get<double>(params[0]->holder)), TYPES::NUM_TYPE};
            }
            std::vector<HolderPack> calcs;
            for (auto& hp : params) {
                calcs.emplace_back(std::round(std::get<double>(hp->holder)), TYPES::NUM_TYPE);
            }
            return HolderPack(MakeListHolder(std::move(calcs)), TYPES::LIST_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  sqrt value of num
/// @return single number if one argument is addressed
/// @return list of sqrt values otherwise
HolderPack sqrt = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() == 0) { throw Errors::RunTime::ExpectedAtLeastOneArg(); }
            for (auto& holderpack : params) {
                if (holderpack->type != TYPES::NUM_TYPE) { throw Errors::TypeErrors::TypeErrorNum(); }
            }
            if (params.size() == 1) {
                return {std::sqrt(std::get<double>(params[0]->holder)), TYPES::NUM_TYPE};
            }
            std::vector<HolderPack> calcs;
            for (auto& hp : params) {
                calcs.emplace_back(std::sqrt(std::get<double>(hp->holder)), TYPES::NUM_TYPE);
            }
            return HolderPack(MakeListHolder(std::move(calcs)), TYPES::LIST_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  rnd(n)
/// @brief  generate a random number from 0 to n - 1
/// @return single number if one argument is addressed
/// @return list of sqrt values otherwise
HolderPack rnd = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 1) { throw Errors::RunTime::ExpectedOneArg(); }
            if (params[0]->type != TYPES::NUM_TYPE) { throw Errors::TypeErrors::TypeErrorNum(); }
            if (std::get<double>(params[0]->holder) <= 0) { throw Errors::TypeErrors::NonPositiveNumber(); }
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, static_cast<int64_t>(std::get<double>(params[0]->holder)));
            return {static_cast<double>(distrib(gen)), TYPES::NUM_TYPE};
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  parse_num(str)
/// @return convert str to number - if it possible
/// @return nil - otherwise
HolderPack parse_num = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 1) { throw Errors::RunTime::ExpectedOneArg(); }
            if (params[0]->type != TYPES::STRING_TYPE) { throw Errors::TypeErrors::TypeErrorString(); }
            try {
                double value = std::stod(std::get<std::string>(params[0]->holder));
                return HolderPack(value, TYPES::NUM_TYPE);
            } catch (...) {
                return HolderPack(TYPES::NIL_TYPE);
            }
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  to_string(n)
/// @return convert number to str
HolderPack to_string = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 1) { throw Errors::RunTime::ExpectedOneArg(); }
            if (params[0]->type != TYPES::NUM_TYPE) { throw Errors::TypeErrors::TypeErrorNum(); }
            return HolderPack(std::to_string(std::get<double>(params[0]->holder)), TYPES::STRING_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);

// STRING AWARE FUNCTIONS
/// @brief  lower(str)
/// @return create a lowercase-ed str
HolderPack lower = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 1) { throw Errors::RunTime::ExpectedOneArg(); }
            if (params[0]->type != TYPES::STRING_TYPE) { throw Errors::TypeErrors::TypeErrorString(); }
            auto range = std::get<std::string>(params[0]->holder)
            | std::views::transform([](unsigned char s) { return std::tolower(s); });

            return HolderPack(std::string(range.begin(), range.end()), TYPES::STRING_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  upper(str)
/// @return create a uppercase-ed str
HolderPack upper = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 1) { throw Errors::RunTime::ExpectedOneArg(); }
            if (params[0]->type != TYPES::STRING_TYPE) { throw Errors::TypeErrors::TypeErrorString(); }
            auto range = std::get<std::string>(params[0]->holder)
            | std::views::transform([](unsigned char s) { return std::toupper(s); });
            
            return HolderPack(std::string(range.begin(), range.end()), TYPES::STRING_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  split(str)
/// @return split a string by blank characters
HolderPack split = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 1) { throw Errors::RunTime::ExpectedOneArg(); }
            if (params[0]->type != TYPES::STRING_TYPE) { throw Errors::TypeErrors::TypeErrorString(); }
            std::vector<std::string> splitted = Utils::Split(std::get<std::string>(params[0]->holder), &std::isblank);
            auto range = splitted | std::views::transform([](std::string s) {
                return HolderPack(std::move(s), TYPES::STRING_TYPE); });
            
            return HolderPack(MakeListHolder(std::vector(range.begin(), range.end())), TYPES::LIST_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief  join(list, delim[default=""])
/// @return assemble a string by giving parts in list by delim
HolderPack join = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() < 1 || params.size() > 2) { throw Errors::RunTime::ExpectedFromOneOrTwoArgs(); }
            if (params[0]->type != TYPES::LIST_TYPE) { throw Errors::TypeErrors::TypeErrorList(); }
            std::string delim;
            if (params.size() > 1) {
                if (params[1]->type != TYPES::STRING_TYPE) { throw Errors::TypeErrors::TypeErrorString(); }
                delim = std::get<std::string>(params[1]->holder);
            }
            std::string result;
            bool first = true;
            for (auto& hp : std::get<ListHolderPtr>(params[0]->holder)->data) {
                if (!first) { result += delim; }
                if (hp->type != TYPES::STRING_TYPE) { throw Errors::TypeErrors::TypeErrorString(); }
                result += std::get<std::string>(hp->holder);
                first = false;
            }
            return HolderPack(std::move(result), TYPES::STRING_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
/// @brief   replace(str, old, new)
/// @return  finds the closest to left str and changes it to a new (or do not change otherwise)
/// @details return new string
HolderPack replace = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 3) { throw Errors::RunTime::ExpectedThreeArgs(); }
            for (auto& hp : params) {
                if (hp->type != TYPES::STRING_TYPE) { throw Errors::TypeErrors::TypeErrorString(); }
            }
            auto& str = std::get<std::string>(params[0]->holder);
            auto& to_delete = std::get<std::string>(params[1]->holder);
            auto& to_replace = std::get<std::string>(params[2]->holder);
            if (size_t pos = str.find(to_delete); pos != std::string::npos) {
                return HolderPack(
                    str.substr(0, pos) + to_replace + str.substr(pos + to_delete.size(), str.size()),
                    TYPES::STRING_TYPE
                );
            }
            return HolderPack(str, TYPES::STRING_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);

// LIST AWARE FUNCTIONS
/// @brief  range(start[default = 0], step, step[default = 1])
/// @return list of numbers (start, start + step, start + 2 * step, ...)
HolderPack range = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() < 1 || params.size() > 3) { throw Errors::RunTime::ExpectedFromOneToThreeArgs(); }
            if (params[0]->type != NUM_TYPE || params.size() > 1 && params[1]->type != NUM_TYPE) { throw Errors::TypeErrors::TypeErrorNum(); }
            double start = (params.size() > 1) ? std::get<double>(params[0]->holder) : 0;
            double end = (params.size() > 1) ? std::get<double>(params[1]->holder) : std::get<double>(params[0]->holder);
            double step = 1;
            if (params.size() > 2) {
                if (params[2]->type != NUM_TYPE) { throw Errors::TypeErrors::TypeErrorNum(); }
                step = std::get<double>(params[2]->holder);
            }
            if (step == 0) { throw Errors::RunTime::ZeroStep(); }
            std::vector<HolderPack> range;
            if (start < end) {
                for (double i = start; i < end; i += step) {
                    range.push_back(HolderPack(i, TYPES::NUM_TYPE));
                }
            } else {
                for (double i = start; i > end; i += step) {
                    range.push_back(HolderPack(i, TYPES::NUM_TYPE));
                }
            }
            return {MakeListHolder(std::move(range)), TYPES::LIST_TYPE};
        }
    )),
    TYPES::FUNC_TYPE
);
/// @brief  push(list, elem)
/// @return modified list with added elem
HolderPack push = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 2) { throw Errors::RunTime::ExpectedTwoArgs(); }
            if (params[0]->type != TYPES::LIST_TYPE) { throw Errors::TypeErrors::TypeErrorList(); }
            if (params[1]->type == TYPES::NOT_SET_TYPE) { throw Errors::MemoryErrors::NotFound(); }

            std::get<ListHolderPtr>(params[0]->holder)->data.push_back(std::move(params[1]));
            return params[0];
        }
    )),
    TYPES::FUNC_TYPE
);
/// @brief  pop(list)
/// @brief  modified list by delete last elem
/// @return deleted element if list is not empty, nil otherwise
HolderPack pop = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 1) { throw Errors::RunTime::ExpectedOneArg(); }
            if (params[0]->type != TYPES::LIST_TYPE) { throw Errors::TypeErrors::TypeErrorList(); }
            
            auto& list = std::get<ListHolderPtr>(params[0]->holder)->data;
            if (list.size() == 0) { return {TYPES::NIL_TYPE}; }
            auto last = list.back(); list.pop_back();
            return last;
        }
    )),
    TYPES::FUNC_TYPE
);
HolderPack insert;
HolderPack remove;
/// @brief  sort(list)
/// @brief  sort the given list by comparators defined in operators
/// @return the sorted given list (modify)
HolderPack sort;

// UNIVERSAL FUNCTIONS STRING / LIST
HolderPack copy;
HolderPack len = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 1) { throw Errors::RunTime::ExpectedOneArg(); }
            if (params[0]->type == TYPES::STRING_TYPE) {
                return HolderPack(
                    static_cast<double>(std::get<std::string>(params[0]->holder).size()),
                    TYPES::NUM_TYPE);
            }
            if (params[0]->type == TYPES::LIST_TYPE) {
                return HolderPack(
                    static_cast<double>(std::get<Memory::ListHolderPtr>(params[0]->holder)->data.size()),
                    TYPES::NUM_TYPE);
            }
            throw Errors::TypeErrors::TypeErrorStringOrList();
        }
    )),
    TYPES::FUNC_TYPE
);

// SYSTEM FUNCTIONS
HolderPack print = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            bool need_space = false;
            for (auto& holderpack : params) {
                if (need_space) { *Interpreter::out << ' '; }
                switch (holderpack->type) {
                    case TYPES::NUM_TYPE:
                        *Interpreter::out << std::get<double>(holderpack->holder); break;
                    case TYPES::STRING_TYPE:
                        *Interpreter::out << '"' << std::get<std::string>(holderpack->holder) << '"'; break;
                    case TYPES::BOOL_TYPE:
                        if (std::get<bool>(holderpack->holder)) { *Interpreter::out << "true"; }
                        else { *Interpreter::out << "false"; }
                        break;
                    case TYPES::NIL_TYPE:
                        *Interpreter::out << "nil"; break;
                    case TYPES::FUNC_TYPE:
                        *Interpreter::out << "<function>"; break;
                    case TYPES::LIST_TYPE:
                    { *Interpreter::out << '['; bool not_first = false;
                        for (auto hp : std::get<ListHolderPtr>(holderpack->holder)->data) {
                            if (not_first) { *Interpreter::out << ", "; } not_first = true;
                            std::get<BuiltInFunction>(
                                std::get<FuncHolderPtr>(print->holder)->function
                            )({std::move(hp)});
                        } *Interpreter::out << ']'; }
                        break;
                    case TYPES::NOT_SET_TYPE:
                        *Interpreter::out << "(not set type)"; break;
                } need_space = true;
            }
            return HolderPack(TYPES::NIL_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
HolderPack println = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            for (auto& holderpack : params) {
                std::get<BuiltInFunction>(
                    std::get<FuncHolderPtr>(print->holder)->function
                )({std::move(holderpack)});
                *Interpreter::out << std::endl;
            }
            return HolderPack(TYPES::NIL_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
HolderPack read = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() > 0) { throw Errors::RunTime::ExpectedZeroArgs(); }
            std::string str;
            std::getline(*Interpreter::in, str);
            return HolderPack(std::move(str), TYPES::STRING_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
HolderPack stacktrace = HolderPack(
    MakeFuncHolder(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            *Interpreter::out << Memory::StackFrame::PrintStack(*Interpreter::stack_frame);
            return HolderPack(TYPES::NIL_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);

}

std::unordered_map<std::string, Memory::HolderPack> BUILT_IN_FUNCTIONS;

void BuiltIn::InitializeBuilInFunctions() {
    BUILT_IN_FUNCTIONS = {
        // NUMBER AWARE FUNCTIONS
        {"abs", BuiltIn::abs},
        {"ceil", BuiltIn::ceil},
        {"floor", BuiltIn::floor},
        {"round", BuiltIn::round},
        {"sqrt", BuiltIn::sqrt},
        {"rnd", BuiltIn::rnd},
        {"parse_num", BuiltIn::parse_num},
        {"to_string", BuiltIn::to_string},

        // STRING AWARE FUNCTIONS
        {"len", BuiltIn::len},
        {"lower", BuiltIn::lower},
        {"upper", BuiltIn::upper},
        {"split", BuiltIn::split},
        {"join", BuiltIn::join},
        {"replace", BuiltIn::replace},

        // LIST AWARE FUNCTIONS
        {"range", BuiltIn::range},
        {"len", BuiltIn::len},
        {"push", BuiltIn::push},
        {"pop", BuiltIn::pop},
        {"insert", BuiltIn::insert},
        {"remove", BuiltIn::remove},
        {"sort", BuiltIn::sort},

        // UNIVERSAL FUNCTIONS
        {"copy", BuiltIn::copy},

        // SYSTEM FUNCTIONS
        {"print", BuiltIn::print},
        {"println", BuiltIn::println},
        {"read", BuiltIn::read},
        {"stacktrace", BuiltIn::stacktrace}
    };
}