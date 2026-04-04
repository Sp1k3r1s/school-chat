cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Server")

include(FetchContent)

FetchContent_Declare(
	asio
	GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
	GIT_TAG asio-1-32-0
)

FetchContent_MakeAvailable(asio)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable (Server "main.cpp" "Sessions/BaseSession.cpp" "Server.cpp" "Sessions/AdminSession.h")

target_compile_definitions(Server PRIVATE ASIO_STANDALONE)
target_link_libraries(Server PRIVATE Threads::Threads OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(Server PRIVATE ${asio_SOURCE_DIR}/asio/include)
