cmake_minimum_required(VERSION 3.20)
project(mips_dbt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS} include)
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

llvm_map_components_to_libnames(llvm_libs Core OrcJIT Support Target X86)

set(JIT_SOURCES
        src/core.cpp
        src/ops/add.cpp src/ops/addi.cpp src/ops/ori.cpp
        src/ops/bne.cpp src/ops/j.cpp src/ops/jr.cpp
        src/ops/addiu.cpp src/ops/lw.cpp src/ops/sw.cpp
        src/ops/or.cpp src/ops/sll.cpp src/ops/slt.cpp
        src/ops/sub.cpp src/ops/xor.cpp src/ops/srl.cpp
        src/ops/lui.cpp
)

add_executable(mips_dbt src/main.cpp ${JIT_SOURCES})
target_link_libraries(mips_dbt PRIVATE ${llvm_libs} z)

# === ТЕСТОВЫЙ ПАЙПЛАЙН ===
enable_testing()

set(TEST_C_DIR ${CMAKE_SOURCE_DIR}/tests/c_sources)
set(TEST_BIN_DIR ${CMAKE_BINARY_DIR}/tests_bin)
file(MAKE_DIRECTORY ${TEST_BIN_DIR})

set(TEST_SOURCES ${TEST_C_DIR}/sort.c ${TEST_C_DIR}/tea.c)

# Жесткие флаги для генерации bare-metal бинарников
foreach(SRC_FILE ${TEST_SOURCES})
    get_filename_component(BASENAME ${SRC_FILE} NAME_WE)
    set(OBJ_FILE ${TEST_BIN_DIR}/${BASENAME}.o)
    set(BIN_FILE ${TEST_BIN_DIR}/${BASENAME}.bin)

    add_custom_command(
            OUTPUT ${BIN_FILE}
            COMMAND mipsel-unknown-linux-gnu-gcc -EL -Os -mips32r2 -mno-abicalls -fno-pic -nostdlib -ffreestanding -fno-delayed-branch -c ${SRC_FILE} -o ${OBJ_FILE}
            COMMAND mipsel-unknown-linux-gnu-objcopy -O binary -j .text ${OBJ_FILE} ${BIN_FILE}
            DEPENDS ${SRC_FILE}
            COMMENT "Cross-compiling ${BASENAME}.c to pure MIPS Little-Endian binary..."
    )
    list(APPEND TEST_BINS ${BIN_FILE})
endforeach()

add_custom_target(build_mips_tests ALL DEPENDS ${TEST_BINS})

add_executable(mips_dbt_test tests/test_runner.cpp ${JIT_SOURCES})
target_link_libraries(mips_dbt_test PRIVATE ${llvm_libs} z)
add_dependencies(mips_dbt_test build_mips_tests)

add_test(NAME IntegrationTests COMMAND mips_dbt_test ${TEST_BIN_DIR})