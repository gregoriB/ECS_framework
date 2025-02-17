#pragma once

#include "../core.hpp"

enum class TestEnum
{
    NONE = 0,
    ONE,
    TWO,
};

inline void test_enum_string_converter(CM &cm)
{
    PRINT("TEST ENUM STRING CONVERTER")

    ECS::internal::Utilities::EnumStringConverter<TestEnum> converter{};
    constexpr std::optional<std::string_view> withValue = converter.convert(TestEnum::ONE);

    assert(withValue.has_value());

    constexpr std::optional<std::string_view> withoutValue = converter.convert(static_cast<TestEnum>(3));

    static_assert(!withoutValue.has_value());
}

inline void test_get_enum_string(CM &cm)
{
    PRINT("TEST ENUM GET STRING CONVERTER")

    constexpr std::string_view withValue = ECS::internal::Utilities::getEnumString(TestEnum::ONE);

    static_assert(withValue == "ONE");
}

inline void test_get_optional_enum_string(CM &cm)
{
    PRINT("TEST ENUM GET STRING CONVERTER")

    constexpr std::optional<std::string_view> withValue =
        ECS::internal::Utilities::getOptionalEnumString(TestEnum::ONE);

    static_assert(withValue.has_value());

    constexpr std::optional<std::string_view> withoutValue =
        ECS::internal::Utilities::getOptionalEnumString(static_cast<TestEnum>(3));

    static_assert(!withoutValue.has_value());
}

inline void test_get_enum_size_count(CM &cm)
{
    PRINT("TEST ENUM SIZE COUNT")

    static_assert(ECS::internal::Utilities::getEnumSize<TestEnum>() == 3);
}

inline void test_get_enum_array(CM &cm)
{
    PRINT("TEST ENUM GET ARRAY")

    auto arr = ECS::internal::Utilities::getEnumArray<TestEnum>();

    static_assert(arr.size() == 3);

    // verify default values
    for (int i = 0; i < arr.size(); ++i)
    {
        auto actual = ECS::internal::Utilities::getEnumString(arr[i]);
        auto expected = ECS::internal::Utilities::getEnumString(TestEnum::NONE);
        assert(actual == expected);
    }
}
