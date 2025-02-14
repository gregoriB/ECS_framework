#pragma once

#include "../core.hpp"

enum class TestEnum
{
    NONE = 0,
    ONE,
    TWO,
};

inline void test_enum_string_converter(ECM &ecm)
{
    PRINT("TEST ENUM STRING CONVERTER")

    ECS::Utilities::EnumStringConverter<TestEnum> converter{};
    constexpr std::optional<std::string_view> withValue = converter.convert(TestEnum::ONE);

    assert(withValue.has_value());

    constexpr std::optional<std::string_view> withoutValue = converter.convert(static_cast<TestEnum>(3));

    static_assert(!withoutValue.has_value());
}

inline void test_get_enum_string(ECM &ecm)
{
    PRINT("TEST ENUM GET STRING CONVERTER")

    constexpr std::string_view withValue = ECS::Utilities::getEnumString(TestEnum::ONE);

    static_assert(withValue == "ONE");
}

inline void test_get_optional_enum_string(ECM &ecm)
{
    PRINT("TEST ENUM GET STRING CONVERTER")

    constexpr std::optional<std::string_view> withValue = ECS::Utilities::getOptionalEnumString(TestEnum::ONE);

    static_assert(withValue.has_value());

    constexpr std::optional<std::string_view> withoutValue = ECS::Utilities::getOptionalEnumString(static_cast<TestEnum>(3));

    static_assert(!withoutValue.has_value());
}

inline void test_get_enum_size_count(ECM &ecm)
{
    PRINT("TEST ENUM SIZE COUNT")

    static_assert(ECS::Utilities::getEnumSize<TestEnum>() == 3);
}

inline void test_get_enum_array(ECM &ecm)
{
    PRINT("TEST ENUM GET ARRAY")

    auto arr = ECS::Utilities::getEnumArray<TestEnum>();

    static_assert(arr.size() == 3);

    // verify default values
    for (int i = 0; i < arr.size(); ++i)
    {
        auto actual = ECS::Utilities::getEnumString(arr[i]);
        auto expected = ECS::Utilities::getEnumString(TestEnum::NONE);
        assert(actual == expected);
    }
}
