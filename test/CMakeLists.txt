# TODO: Separate C++ plugin tests so they can run independently of Python bindings.
if(BUILD_PYTHON_BINDINGS)
    set(plugins_src "${CMAKE_CURRENT_SOURCE_DIR}/plugins")

    pytest_discover_tests(
        TestKind
        LIBRARY_PATH_PREPEND
            $<TARGET_FILE_DIR:pyKind>
            "${onetbb_LIB_DIRS_RELEASE}"
            "${onetbb_BIN_DIRS_RELEASE}"
        PYTHON_PATH_PREPEND
            "$<TARGET_FILE_DIR:pyKind>/../.."
            "$<TARGET_FILE_DIR:pxr::pyPlug>/../.."
            "$<TARGET_FILE_DIR:pxr::pyTf>/../.."
        TRIM_FROM_NAME "^(Test|test_)"
        ENVIRONMENT
            "PLUGINS_PATH=${CMAKE_CURRENT_SOURCE_DIR}/plugins/*/"
        EXTRA_ARGS "-v"
        DEPENDS kind pyKind
    )
endif()
