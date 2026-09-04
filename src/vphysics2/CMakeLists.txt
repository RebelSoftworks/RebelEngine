project(vphysics2 LANGUAGES CXX)

if(NOT LINUX)
	set(OUTBINDIR ${SRCDIR}/../game/bin/win64)
else()
	set(OUTBINDIR ${SRCDIR}/../game/bin/linuxsteamrt64)
endif()

add_library(vphysics2 SHARED
    vphysics2_wrapper.cpp
)

set_target_properties(vphysics2 PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTBINDIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUTBINDIR}
)

target_link_directories(vphysics2 PRIVATE ${SRCDIR}/lib/public)

target_include_directories(vphysics2 PRIVATE
    ${SRCDIR}/public
    ${SRCDIR}/public/tier0
    ${SRCDIR}/public/appframework
    ${SRCDIR}/public/interfaces
    ${SRCDIR}/public/vphysics
    ${SRCDIR}/public/tier1
)

target_link_libraries(vphysics2 PRIVATE interfaces tier0 vstdlib)