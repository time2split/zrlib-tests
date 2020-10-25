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
	src/base/Graph/GraphTests.c
	src/base/Identifier/IdentifierTests.c
	src/base/Identifier/IdentifierUTests.c
	src/base/Identifier/IDGenerator/IDGeneratorTests.c
	src/base/structTests.c
	src/base/Tree/TreeTests.c
	src/base/Vector/VectorTests.c
	src/benchmark/versus/Address_pow2VsArithmetic.c
	src/benchmark/versus/Bits_naiveVsIntrinsic.c
	src/benchmark/versus/PoolVsMalloc.c
	src/benchmark/versus/Reserve_listVsBits.c
    )
    
set(HEADERS
	src/main.h
	src/base/ArrayOpTests.h
	src/base/BitsTests.h
	src/base/MapTests.h
	src/base/Graph/GraphTests.h
	src/base/Identifier/IdentifierTests.h
	src/base/Identifier/IdentifierUTests.h
	src/base/Identifier/IDGenerator/IDGeneratorTests.h
	src/base/structTests.h
	src/base/Tree/TreeTests.h
	src/base/MemoryOpTests.h
	src/base/MPoolTests.h
	src/base/Vector/VectorTests.h
    )

add_executable(zrlib-tests ${SRCS} ${HEADERS})
set_property(TARGET zrlib-tests PROPERTY C_STANDARD 11)
target_link_libraries(zrlib-tests zrlib)
