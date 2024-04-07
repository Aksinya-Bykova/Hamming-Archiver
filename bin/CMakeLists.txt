add_executable(${PROJECT_NAME} main.cpp
        ../tests/archiver_test.cpp
        ../lib/parser.cpp
        ../lib/parser.h
        ../lib/archive.cpp
        ../lib/archive.h
        ../lib/file.h
        ../lib/file.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE archiver)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
