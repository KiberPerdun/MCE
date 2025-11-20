set(SOURCES main.c
        ../include/MIPS32.h
        ../include/types.h
        ../include/MIPS32_math.h
        ../include/MIPS32_branch.h
        ../include/MIPS32_control.h
        ../include/MIPS32_mem.h
        ../include/MIPS32_logic.h
        ../include/MIPS32_move.h
        ../include/MIPS32_shift.h
        ../include/MIPS32_trap.h
        MIPS32_unknown_opcode.c
        ../include/debug.h
        MIPS32_init.c
        MIPS32_free.c
        MIPS32_addi.c
        MIPS32_run.c
)

add_executable(mips ${SOURCES})
target_include_directories(mips PRIVATE ${PROJECT_SOURCE_DIR}/include)
