if (XMRIG_OS_WIN)
    message(STATUS "WITH_COM        \t= ${WITH_COM}")

    if (WITH_COM)
        add_definitions(-DXMRIG_FEATURE_COM)
    endif()
endif()
