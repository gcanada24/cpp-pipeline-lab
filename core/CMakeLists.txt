# capture_core — static library. Public headers in include/capture/..., implementation in src/...
# Anything that links capture_core can #include <capture/...> (PUBLIC include dir).
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(capture_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(capture_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
