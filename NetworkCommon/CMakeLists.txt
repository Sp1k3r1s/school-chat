cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NetworkCommon")

add_library (NetworkCommon "src/Reader.cpp" "src/Serializer.cpp" "src/Writer.cpp" "include/NetworkCommon/Deserializer.h" "src/Deserializer.cpp")

target_include_directories(NetworkCommon
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(NetworkCommon PUBLIC cxx_std_23)