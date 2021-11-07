project(OrgCpp)

cmake_minimum_required(VERSION 3.18)

add_library(orgcpp SHARED OrgFile.cpp)
set_target_properties(orgcpp PROPERTIES PUBLIC_HEADER OrgFile.hpp)
install(TARGETS orgcpp PUBLIC_HEADER)

# Do not build on Android
# Triggers relocation issues with Android toolchain
if(NOT ANDROID)
    add_executable(parse-org-file parse-org-file.cpp)

    target_link_libraries(parse-org-file orgcpp)
endif()
