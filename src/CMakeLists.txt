set(SOURCES
    CuteDock.cpp
    CuteNode.cpp
    main.cpp
    MainWindow.cpp
    NodeScene.cpp
    NodeView.cpp
    )

set(HEADERS
    CuteDock.h
    CuteNode.h
    MainWindow.h
    NodeScene.h
    NodeView.h
    )

set(UI_FILES
    MainWindow.ui
    )

set(QRC_FILES
    )

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES} ${HEADERS} ${UI_FILES} ${QRC_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

# copy dlls next to compiled binary
if (WIN32)
    file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}/bin" QtBinDirectory)

    set(QtDebugLibs Qt5Cored.dll Qt5Guid.dll Qt5Widgetsd.dll)
    set(QtReleaseLibs Qt5Core.dll Qt5Gui.dll Qt5Widgets.dll)

    foreach(file ${QtDebugLibs})
        file(TO_CMAKE_PATH "${QtBinDirectory}/${file}" CuteLib)
        ADD_CUSTOM_COMMAND (TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CuteLib} ${ProjectDebugOut})
    endforeach()

    foreach(file ${QtReleaseLibs})
        file(TO_CMAKE_PATH "${QtBinDirectory}/${file}" CuteLib)
        ADD_CUSTOM_COMMAND (TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CuteLib} ${ProjectReleaseOut})
    endforeach()
endif()
