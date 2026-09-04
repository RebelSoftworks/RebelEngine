project(engine2 LANGUAGES CXX)

add_library(engine2 SHARED 
    bitbuf_errorhandler.cpp
    camerarenderer.cpp
    engine2.cpp
    engineservicemgr.cpp
    engineserviceregistry.cpp
    inputservice.cpp
    pvs.cpp
    renderservice.cpp
    sceneutils.cpp
    ${SRCDIR}/game/client/modelloader.cpp
)

set(OUTBINDIR ${SRCDIR}/../game/bin/win64)

set_target_properties(engine2 PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUTBINDIR}
)

target_include_directories(engine2 PRIVATE
    ${SRCDIR}/public
    ${SRCDIR}/public/tier0
    ${SRCDIR}/public/appframework
    ${SRCDIR}/public/interfaces
)

target_link_directories(engine2 PRIVATE ${SRCDIR}/lib/public)

target_compile_options(engine2 PRIVATE /W0)

target_link_libraries(engine2 PRIVATE interfaces tier0 vstdlib)
