add_library(libs INTERFACE)

target_sources(libs
    INTERFACE
    FILE_SET HEADERS
    FILES
    vector.h
    body.h
    constants.h
    forcefields.h
    integrators.h
    simulation.h
)
