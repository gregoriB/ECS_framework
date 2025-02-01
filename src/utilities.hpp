#pragma once

#include <cmath>
#include <iostream>
#include <optional>
#include <ostream>

template <typename T> [[nodiscard]] auto getTypeName()
{
    return typeid(T).name();
}

template <typename T, typename Base> [[nodiscard]] bool isBase()
{
    if (std::is_base_of_v<Base, T>)
        return true;

    return false;
}

template <typename EnumValue, int Count = 0> class EnumStringConverter final
{
  public:
    template <auto Value> constexpr std::optional<std::string_view> generateStringFromValue() const
    {
        std::string_view pretty{__PRETTY_FUNCTION__};
        for (int i = pretty.size(); i > 2; --i)
        {
            if (pretty[i - 1] == ':' && pretty[i - 2] == ':')
            {
                pretty.remove_prefix(i);
                break;
            }
        };

        for (int i = 0; i < pretty.size(); ++i)
        {
            if (pretty[i] == ';' || pretty[i] == ']')
            {
                pretty.remove_suffix(pretty.size() - i);
                break;
            }
        }

        if (pretty.size() >= 25)
        {
            std::string_view slice = pretty.substr(0, 25);
            if (slice == "generateStringFromValue()")
                return std::nullopt;
        }

        return pretty;
    }

    constexpr std::optional<std::string_view> convert(EnumValue value) const
    {
        return EnumStringConverter<EnumValue, Count>{}(static_cast<int>(value));
    };

    constexpr std::optional<std::string_view> operator()(int value) const
    {
        if (!(value - Count))
            return generateStringFromValue<static_cast<EnumValue>(Count)>();

        return EnumStringConverter<EnumValue, Count + 1>{}(value);
    }
};

inline constexpr int ENUM_STRING_MAX_SIZE = 128;

template <typename EnumValue> struct EnumStringConverter<EnumValue, ENUM_STRING_MAX_SIZE>
{
    constexpr std::optional<std::string_view> operator()(int) const
    {
        return std::nullopt;
    }
};

/**
 * A convenience that assumes the string definitely exists. BE CAREFUL!
 */
template <typename Enum> [[nodiscard]] inline constexpr std::string_view getEnumString(Enum value)
{
    return EnumStringConverter<Enum>{}.convert(value).value();
}

template <typename Enum>
[[nodiscard]] inline constexpr std::optional<std::string_view> getOptionalEnumString(Enum value)
{
    return EnumStringConverter<Enum>{}.convert(value);
}

template <typename Enum> [[nodiscard]] inline constexpr size_t getEnumSize()
{
    size_t enumSize{};
    for (int i = 0; i < ENUM_STRING_MAX_SIZE; ++i)
    {
        auto value = getOptionalEnumString(static_cast<Enum>(i));
        if (value.has_value())
            enumSize++;
    }

    return enumSize;
}

template <typename Enum> [[nodiscard]] inline constexpr std::array<Enum, getEnumSize<Enum>()> getEnumArray()
{
    return std::array<Enum, getEnumSize<Enum>()>{};
}

template <typename Func, typename... Args>
concept ReturnsBool = std::is_invocable_r_v<bool, Func, Args...>;
