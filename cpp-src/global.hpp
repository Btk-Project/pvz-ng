#pragma once

#include <array>
#include <string>
#include <typeinfo>

#if __cplusplus >= 201703L || _MSVC_LANG > 201402L
/// ====================== enum string =========================
#ifndef NEKO_ENUM_SEARCH_DEPTH
#define NEKO_ENUM_SEARCH_DEPTH 60
#endif

// NOLINTBEGIN
#ifdef __GNUC__
#define NEKO_STRINGIFY_TYPE_RAW(x) NEKO_STRINGIFY_TYPEINFO(typeid(x))
#define NEKO_FUNCTION              __PRETTY_FUNCTION__

namespace detail {
template <typename T, T Value>
constexpr auto _Neko_GetEnumName() noexcept {
    // constexpr auto _Neko_GetEnumName() [with T = MyEnum; T Value = MyValues]
    // constexpr auto _Neko_GetEnumName() [with T = MyEnum; T Value =
    // (MyEnum)114514]"
    std::string_view name(__PRETTY_FUNCTION__);
    size_t eqBegin        = name.find_last_of(' ');
    size_t end            = name.find_last_of(']');
    std::string_view body = name.substr(eqBegin + 1, end - eqBegin - 1);
    if (body[0] == '(') {
        // Failed
        return std::string_view();
    }
    return body;
}
inline std::string NEKO_STRINGIFY_TYPEINFO(const std::type_info& info) {
    int status;
    auto str = ::abi::__cxa_demangle(info.name(), nullptr, nullptr, &status);
    if (str) {
        std::string ret(str);
        ::free(str);
        return ret;
    }
    return info.name();
}
#elif defined(_MSC_VER)
#define NEKO_STRINGIFY_TYPE_RAW(type) NEKO_STRINGIFY_TYPEINFO(typeid(type))
#define NEKO_ENUM_TO_NAME(enumType)
#define NEKO_FUNCTION __FUNCTION__
namespace detail {
inline const char* NEKO_STRINGIFY_TYPEINFO(const std::type_info& info) {
    // Skip struct class prefix
    auto name = info.name();
    if (::strncmp(name, "class ", 6) == 0) {
        return name + 6;
    }
    if (::strncmp(name, "struct ", 7) == 0) {
        return name + 7;
    }
    if (::strncmp(name, "enum ", 5) == 0) {
        return name + 5;
    }
    return name;
}
template <typename T, T Value>
constexpr auto _Neko_GetEnumName() noexcept {
    // auto __cdecl _Neko_GetEnumName<enum main::MyEnum,(enum
    // main::MyEnum)0x2>(void) auto __cdecl _Neko_GetEnumName<enum
    // main::MyEnum,main::MyEnum::Wtf>(void)
    std::string_view name(__FUNCSIG__);
    size_t dotBegin       = name.find_first_of(',');
    size_t end            = name.find_last_of('>');
    std::string_view body = name.substr(dotBegin + 1, end - dotBegin - 1);
    if (body[0] == '(') {
        // Failed
        return std::string_view();
    }
    return body;
}
#else
namespace detail {
#define NEKO_STRINGIFY_TYPE_RAW(type) typeid(type).name()
template <typename T, T Value>
constexpr auto _Neko_GetEnumName() noexcept {
    // Unsupported
    return std::string_view();
}
#endif
template <typename T, T Value>
constexpr bool _Neko_IsValidEnum() noexcept {
    return !_Neko_GetEnumName<T, Value>().empty();
}
template <typename T, size_t... N>
constexpr size_t _Neko_GetValidEnumCount(std::index_sequence<N...> seq) noexcept {
    return (... + _Neko_IsValidEnum<T, T(N)>());
}
template <typename T, size_t... N>
constexpr size_t _Neko_GetValidBitEnumCount(std::index_sequence<N...> seq) noexcept {
    return (... + _Neko_IsValidEnum<T, T((uint64_t)1 << N)>());
}
template <typename T, size_t... N>
constexpr auto _Neko_GetValidEnumNames(std::index_sequence<N...> seq) noexcept {
    constexpr auto validCount = _Neko_GetValidEnumCount<T>(seq);

    std::array<std::pair<T, std::string_view>, validCount> arr;
    std::string_view vstr[sizeof...(N)]{_Neko_GetEnumName<T, T(N)>()...};

    size_t n    = 0;
    size_t left = validCount;
    auto iter   = arr.begin();

    for (auto i : vstr) {
        if (!i.empty()) {
            // Valid name
            iter->first  = T(n);
            iter->second = i;
            ++iter;
        }
        if (left == 0) {
            break;
        }

        n += 1;
    }

    return arr;
}
template <typename T, size_t... N>
constexpr auto _Neko_GetValidBitEnumNames(std::index_sequence<N...> seq) noexcept {
    constexpr auto validCount = _Neko_GetValidBitEnumCount<T>(seq);

    std::array<std::pair<T, std::string_view>, validCount> arr;
    std::string_view vstr[sizeof...(N)]{_Neko_GetEnumName<T, T((uint64_t)1 << N)>()...};

    size_t n    = 0;
    size_t left = validCount;
    auto iter   = arr.begin();

    for (auto i : vstr) {
        if (!i.empty()) {
            // Valid name
            iter->first  = T(n);
            iter->second = i;
            ++iter;
        }
        if (left == 0) {
            break;
        }

        n += 1;
    }

    return arr;
}
// NOLINTEND
template <typename T>
std::string enum_to_string(const T& value) {
    constexpr static auto KEnumArr = _Neko_GetValidEnumNames<T>(std::make_index_sequence<NEKO_ENUM_SEARCH_DEPTH>());
    std::string ret;
    for (int i = 0; i < KEnumArr.size(); ++i) {
        if (KEnumArr[i].first == value) {
            ret = std::string(KEnumArr[i].second);
        }
    }
    return ret;
}

template <typename T>
constexpr std::string make_enum_string(const std::string& fmt) {
    constexpr auto KEnumArr = _Neko_GetValidEnumNames<T>(std::make_index_sequence<NEKO_ENUM_SEARCH_DEPTH>());
    std::string ret;
    for (int i = 0; i < KEnumArr.size(); ++i) {
        if (KEnumArr[i].second.size() > 0) {
            std::string tfmt = fmt;
            auto enumName    = KEnumArr[i].second;
            auto colon       = enumName.find_last_of(':');
            if (colon != std::string::npos) {
                enumName = enumName.substr(colon + 1);
            }
            tfmt.replace(tfmt.find("{enum}"), 6, enumName);
            tfmt.replace(tfmt.find("{num}"), 5, std::to_string(i));
            ret += tfmt;
        }
    }
    return ret;
}

template <typename T>
constexpr std::string make_bit_enum_string(const std::string& fmt) {
    constexpr auto KEnumArr = _Neko_GetValidBitEnumNames<T>(std::make_index_sequence<NEKO_ENUM_SEARCH_DEPTH>());
    std::string ret;
    for (int i = 0; i < KEnumArr.size(); ++i) {
        if (KEnumArr[i].second.size() > 0) {
            std::string tfmt = fmt;
            auto enumName    = KEnumArr[i].second;
            auto colon       = enumName.find_last_of(':');
            if (colon != std::string::npos) {
                enumName = enumName.substr(colon + 1);
            }
            tfmt.replace(tfmt.find("{enum}"), 6, enumName);
            tfmt.replace(tfmt.find("{num}"), 5, std::to_string((uint64_t)1 << i));
            ret += tfmt;
        }
    }
    return ret;
}
}
#else
static_assert(false, "please compiler in c++17 or above")
#endif

template <typename T>
constexpr std::string make_property_hint_flags();

#define PVZ_NG_ENUM_OPERATOR(EnumType, IntType)                                                                        \
    inline IntType operator|(EnumType t1, EnumType t2) { return static_cast<IntType>(t1) | static_cast<IntType>(t2); } \
    inline IntType operator&(EnumType t1, EnumType t2) { return static_cast<IntType>(t1) & static_cast<IntType>(t2); } \
    inline IntType operator^(EnumType t1, EnumType t2) { return static_cast<IntType>(t1) ^ static_cast<IntType>(t2); } \
    inline IntType operator~(EnumType t1) { return ~static_cast<IntType>(t1); }                                        \
    inline bool operator!(EnumType t1) { return !static_cast<bool>(t1); }                                              \
    inline bool operator==(EnumType t1, EnumType t2) { return static_cast<IntType>(t1) == static_cast<IntType>(t2); }  \
    inline IntType operator|(EnumType t1, IntType t2) { return static_cast<IntType>(t1) | t2; }                        \
    inline IntType operator&(EnumType t1, IntType t2) { return static_cast<IntType>(t1) & t2; }                        \
    inline IntType operator^(EnumType t1, IntType t2) { return static_cast<IntType>(t1) ^ t2; }                        \
    inline bool operator==(EnumType t1, IntType t2) { return static_cast<IntType>(t1) == t2; }                         \
    inline IntType operator|(IntType t1, EnumType t2) { return t1 | static_cast<IntType>(t2); }                        \
    inline IntType operator&(IntType t1, EnumType t2) { return t1 & static_cast<IntType>(t2); }                        \
    inline IntType operator^(IntType t1, EnumType t2) { return t1 ^ static_cast<IntType>(t2); }                        \
    inline bool operator==(IntType t1, EnumType t2) { return t1 == static_cast<IntType>(t2); }                         \
    template <typename T>                                                                                              \
    constexpr std::string make_property_hint_flags() {                                                                 \
        auto str = detail::make_bit_enum_string<EnumType>("{enum}:{num},");                                            \
        str.pop_back();                                                                                                \
        return str;                                                                                                    \
    }

namespace godot {

enum class GlobalType : uint32_t {
    // None = 0, // 不知道为啥如果定义0，在模板自动生成的枚举中会将未定义的值全部视为0。

    // height
    NormalHPlant = 1 << 0, // normal plant
    HighHPlant   = 1 << 1, // high plant
    LowHPlant    = 1 << 2, // low plant

    // gridSize
    NormalGPlant = 1 << 3, // normal plant, gridSize = 1
    SmallGPlant  = 1 << 4, // small plant, gridSize = 1 / 3
    LargeGPlant  = 1 << 5, // large plant, gridSize = 2

    // terrain
    Soil         = 1 << 6,
    Sand         = 1 << 7,
    Rock         = 1 << 8,
    Water        = 1 << 9,
    Flowerpot    = 1 << 10,
    LotusLeaf    = 1 << 11,
    ExplosionPit = 1 << 12,

    // other
};
PVZ_NG_ENUM_OPERATOR(GlobalType, uint32_t)
} // namespace godot

#undef PVZ_NG_ENUM_OPERATOR