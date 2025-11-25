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
        MIPS32_add.c
        microui.c
        ../include/microui.h
)

add_executable(mips ${SOURCES})
target_include_directories(mips PRIVATE ${PROJECT_SOURCE_DIR}/include)


if(NOT EMSCRIPTEN)
    find_package(SDL3 REQUIRED CONFIG)

    add_executable(mips_native ${SOURCES})
    target_link_libraries(mips_native PRIVATE SDL3::SDL3)
    target_include_directories(mips_native PRIVATE ${PROJECT_SOURCE_DIR}/include)
endif()

if(EMSCRIPTEN)
    include(FetchContent)

    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON  CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL_VIDEO_DRIVER_X11     OFF CACHE BOOL "" FORCE)
    set(SDL_VIDEO_DRIVER_WAYLAND OFF CACHE BOOL "" FORCE)
    set(SDL_RENDER_VULKAN        OFF CACHE BOOL "" FORCE)
    set(SDL_VIDEO_VULKAN         OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG main
    )
    FetchContent_MakeAvailable(SDL3)

    add_executable(mips_web ${SOURCES})
    set_target_properties(mips_web PROPERTIES SUFFIX ".html")

    target_link_options(mips_web PRIVATE
            "-sUSE_SDL=0"
            "-sASYNCIFY"
            "-sSINGLE_FILE=1"
            "-sSTACK_SIZE=524288"     # 512 КБ
    )

    target_link_libraries(mips_web PRIVATE SDL3::SDL3-static)
    target_include_directories(mips_web PRIVATE ${PROJECT_SOURCE_DIR}/include)
endif()
