# find directories with includes
include_directories(../backend)
include_directories(../common)


add_subdirectory(lab_verifier)

add_executable(assembler asm_main.cpp $<TARGET_OBJECTS:common>)
target_link_libraries(assembler lc3core)
add_executable(simulator sim_main.cpp $<TARGET_OBJECTS:common>)
target_link_libraries(simulator lc3core)
add_executable(labtest labtest_main.cpp $<TARGET_OBJECTS:common>)
target_link_libraries(labtest lc3core lab_verifier)