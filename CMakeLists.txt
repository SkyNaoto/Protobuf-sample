cmake_minimum_required(VERSION 3.15)
project(MenuTreeProtobufSocket LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)
find_package(absl REQUIRED COMPONENTS base strings log log_internal_check_op)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS menu.proto)

# add_executable(menu_app main.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_executable(server server.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(server PRIVATE protobuf::libprotobuf absl::base absl::strings absl::log absl::log_internal_check_op)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_executable(client client.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(client PRIVATE protobuf::libprotobuf absl::base absl::strings absl::log absl::log_internal_check_op)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})