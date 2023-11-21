file(GLOB CXX_SOURCES *.cpp)
file(GLOB CXX_HEADERS *.h)

add_library(lab_verifier STATIC ${CXX_SOURCES} ${CXX_HEADERS})
target_link_libraries(lab_verifier lc3core)