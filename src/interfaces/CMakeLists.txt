project(interfaces LANGUAGES CXX)

add_library(interfaces
    interfaces.cpp
)

target_include_directories(interfaces PRIVATE
    ${SRCDIR}/public
    ${SRCDIR}/public/tier0
    ${SRCDIR}/public/appframework
    ${SRCDIR}/public/interfaces
)