message(STATUS "WITH_EVENTS     \t= ${WITH_EVENTS}")

if (WITH_EVENTS)
    add_definitions(-DXMRIG_FEATURE_EVENTS)
endif()
