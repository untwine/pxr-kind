# TODO: Separate C++ plugin tests so they can run independently of Python bindings.
if(BUILD_PYTHON_BINDINGS)
    set(plugins_src "${CMAKE_CURRENT_SOURCE_DIR}/plugins")

    pytest_discover_tests(
        TestKind
        LIBRARY_PATH_PREPEND
            $<TARGET_FILE_DIR:kind>
            $<TARGET_FILE_DIR:pyKind>
            $<TARGET_FILE_DIR:pxr::plug>
            $<TARGET_FILE_DIR:pxr::pyPlug>
            $<TARGET_FILE_DIR:pxr::tf>
            $<TARGET_FILE_DIR:pxr::pyTf>
        PYTHON_PATH_PREPEND
            "$<TARGET_FILE_DIR:pyKind>/../.."
            "$<TARGET_FILE_DIR:pxr::pyPlug>/../.."
            "$<TARGET_FILE_DIR:pxr::pyTf>/../.."
#            "${CMAKE_CURRENT_SOURCE_DIR}/plugins/"
        TRIM_FROM_NAME "^(Test|test_)"
        ENVIRONMENT
            "PLUGINS_PATH=${CMAKE_CURRENT_SOURCE_DIR}/plugins/*/"
        EXTRA_ARGS "-v"
        DEPENDS kind pyKind
    )
endif()
