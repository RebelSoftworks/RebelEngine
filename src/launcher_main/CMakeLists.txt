project(launcher_main LANGUAGES CXX)

add_executable(launcher_main
    main.cpp
    launcher.rc
)

set(OUTBINDIR ${SRCDIR}/../game) 

set_target_properties(launcher_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUTBINDIR}
    OUTPUT_NAME "dispersion"
)

target_include_directories(launcher_main PRIVATE
    ${SRCDIR}/public
    ${SRCDIR}/public/tier0
    ${SRCDIR}/public/appframework
    ${SRCDIR}/public/interfaces
)

target_link_directories(launcher_main PRIVATE
    ${SRCDIR}/lib/public
)

target_link_libraries(launcher_main PRIVATE
    interfaces
    tier0
    vstdlib
    steam_api64
)