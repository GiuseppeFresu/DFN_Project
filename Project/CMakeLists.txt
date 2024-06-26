cmake_minimum_required(VERSION 3.9)

project(DFN LANGUAGES CXX C VERSION 1.0.0)

# Set C++ Standard
################################################################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# IMPOSE WARNINGS ON DEBUG
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic-errors")

# IMPOSE CXX FLAGS FOR WINDOWS
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
endif (WIN32)

# Add dependencies
################################################################################
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../externals/Main_Install/eigen3)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../externals/Main_Install/googletest)

## Eigen3
find_package(Eigen3 CONFIG REQUIRED)
list(APPEND DFN_LINKED_LIBRARIES PUBLIC Eigen3::Eigen)

## Threads
find_package(Threads REQUIRED)
list(APPEND DFN_LINKED_LIBRARIES PRIVATE Threads::Threads)

## GTest
find_package(GTest REQUIRED)

list(APPEND DFN_LINKED_LIBRARIES PRIVATE GTest::gtest)
list(APPEND DFN_LINKED_LIBRARIES PRIVATE GTest::gtest_main)
list(APPEND DFN_LINKED_LIBRARIES PRIVATE GTest::gmock)
list(APPEND DFN_LINKED_LIBRARIES PRIVATE GTest::gmock_main)

# Insert Sources
################################################################################

add_subdirectory(src)
list(APPEND DFN_SOURCES ${src_sources})
list(APPEND DFN_HEADERS ${src_headers})
list(APPEND DFN_INCLUDE ${src_includes})

add_subdirectory(TestingParaview/Code/src)
list(APPEND DFN_SOURCES ${src_paraview_sources})
list(APPEND DFN_HEADERS ${src_paraview_headers})
list(APPEND DFN_INCLUDE ${src_paraview_includes})

add_subdirectory(src_test)
list(APPEND DFN_TEST_SOURCES ${src_test_sources})
list(APPEND DFN_TEST_HEADERS ${src_test_headers})
list(APPEND DFN_TEST_INCLUDE ${src_test_includes})

# Add file
###############################################################################
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/DFN DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


# Create executable
################################################################################
add_executable(${PROJECT_NAME}
        main.cpp
        ${DFN_SOURCES}
    ${DFN_HEADERS})

add_executable(${PROJECT_NAME}_TEST
    main_test.cpp
    ${DFN_SOURCES}
    ${DFN_HEADERS}
    ${DFN_TEST_SOURCES}
    ${DFN_TEST_HEADERS})

target_link_libraries(${PROJECT_NAME} ${DFN_LINKED_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${DFN_INCLUDE})
target_compile_options(${PROJECT_NAME} PUBLIC -fPIC)


target_include_directories(${PROJECT_NAME}_TEST PUBLIC ${DFN_PARAVIEW_INCLUDES} ${DFN_TEST_INCLUDES})
target_link_libraries(${PROJECT_NAME}_TEST PUBLIC ${DFN_LINKED_LIBRARIES})
target_compile_options(${PROJECT_NAME}_TEST PUBLIC -fPIC)
