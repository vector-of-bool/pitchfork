FetchContent_Declare(
    RangeV3
    URL https://hub.aliasa.io/ericniebler/range-v3@4f4beb45c5e56aca4233e4d4c760208e21fff2ec
)

FetchContent_GetProperties(RangeV3)
if(NOT rangev3_POPULATED)
    FetchContent_Populate(RangeV3)
    add_library(range-v3 INTERFACE)
    target_include_directories(range-v3 SYSTEM INTERFACE
        "${rangev3_SOURCE_DIR}/include")
endif()
