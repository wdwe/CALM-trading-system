add_library(tz
        src/tz.cpp
)

target_include_directories(tz
    PUBLIC ./include
)

target_compile_definitions(tz
        PRIVATE INSTALL=.
)