#pragma once

#include "core.hpp"
#include "utilities.hpp"

#ifdef ecs_disable_asserts
#define ECS_ASSERT(condition, message) ECS::internal::Utilities::_assert(condition, message);
#else
#define ECS_ASSERT(...) ;
#endif

#ifdef ecs_show_warnings
#define ECS_LOG_WARNING(...) ECS::internal::Utilities::debugWarningPrint(__VA_ARGS__);
#else
#define ECS_LOG_WARNING(...) ;
#endif
