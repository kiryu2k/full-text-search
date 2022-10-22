include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
)

FetchContent_MakeAvailable(nlohmann_json)