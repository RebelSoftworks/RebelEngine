project(hammer LANGUAGES CXX)

add_executable(hammer
    main.cpp
    hammer.rc
)

set(OUTBINDIR ${SRCDIR}/../game) 

set_target_properties(hammer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUTBINDIR}
    OUTPUT_NAME "editor"
)

target_include_directories(hammer PRIVATE
    ${SRCDIR}/public
    ${SRCDIR}/public/tier0
    ${SRCDIR}/public/appframework
    ${SRCDIR}/public/interfaces
)

target_compile_options(hammer PRIVATE /W0)

target_link_libraries(hammer PRIVATE interfaces tier0 vstdlib)


target_link_directories(hammer PRIVATE
    ${SRCDIR}/lib/public
)