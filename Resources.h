
#pragma once

#include "Version.h"

// Macros to stringify preprocessor literals.  This is compatible with both C++ and Microsoft's RC compiler.
#define STRINGIFY(text)         STRINGIFY_IMPL_1((text))
#define STRINGIFY_IMPL_1(arg)   STRINGIFY_IMPL_2 arg
#define STRINGIFY_IMPL_2(text)  #text
#define EXPAND(...)             __VA_ARGS__


// Outpost 2 version triple.  Expands to major.minor.step
#define OP2_VERSION_TRIPLE      EXPAND(OP2_MAJOR_VERSION).EXPAND(OP2_MINOR_VERSION).EXPAND(OP2_STEPPING_VERSION)
#define OP2_VERSION_TRIPLE_STR  STRINGIFY(EXPAND(OP2_VERSION_TRIPLE))
// Outpost 2 version quad.  Expands to major.minor.0.step
#define OP2_VERSION_QUAD        EXPAND(OP2_MAJOR_VERSION).EXPAND(OP2_MINOR_VERSION).EXPAND(0).EXPAND(OP2_STEPPING_VERSION)
#define OP2_VERSION_QUAD_STR    STRINGIFY(EXPAND(OP2_VERSION_QUAD))


// Macro to define or match resource replacement template names.
#define RESOURCE_REPLACE(moduleName, resourceId)      __RESOURCE_REPLACE__(moduleName!resourceId)
#define RESOURCE_REPLACE_NAME(moduleName, resourceId)  STRINGIFY(RESOURCE_REPLACE(moduleName, resourceId))

// Macro to define raw string resources.  Can accept multi-byte or wide strings.
#define RESOURCE_STRING(text) 6 { text, L"\0" }
