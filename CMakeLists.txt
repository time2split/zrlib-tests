cmake_minimum_required(VERSION 3.0)

project(zrlib-tests)

include_directories(libs/ zrlib/)
add_subdirectory(zrlib/)
set(CMAKE_BUILD_TYPE Debug)

set(SRCS
    src/main.c
	src/base/ArrayOpTests.c
	src/base/BitsTests.c
	src/base/MapTests.c
	src/base/MemoryOpTests.c
	src/base/MPoolTests.c
	src/base/Vector/VectorTests.c
    )
    
set(HEADERS
	src/main.h
	src/base/ArrayOpTests.h
	src/base/BitsTests.h
	src/base/MapTests.h
	src/base/MemoryOpTests.h
	src/base/MPoolTests.h
	src/base/Vector/VectorTests.h
    )

add_executable(zrlib-tests ${SRCS} ${HEADERS})
set_property(TARGET zrlib-tests PROPERTY C_STANDARD 11)
target_link_libraries(zrlib-tests zrlib)
