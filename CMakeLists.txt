cmake_minimum_required(VERSION 3.21)
project(ProjectO VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(DEFINED ENV{QT_DIR})
    set(QT_ROOT "$ENV{QT_DIR}" CACHE PATH "Qt installation path")
elseif(NOT DEFINED QT_ROOT)
    set(QT_ROOT "C:/Qt/6.11.1/mingw_64" CACHE PATH "Qt installation path")
endif()
list(APPEND CMAKE_PREFIX_PATH "${QT_ROOT}")

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    QuickControls2
    Widgets
    Network
    Concurrent
)

find_package(Qt6 COMPONENTS Pdf QUIET)

qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/appsettings.cpp
    src/appcontroller.cpp
    src/filelistmodel.cpp
    src/pdfpreviewmodel.cpp
    src/officetextextractor.cpp
    src/pdfpagerenderer.cpp
    src/previewimageprovider.cpp
    src/updatechecker.cpp
    src/redactionregionmodel.cpp
    src/piidetector.cpp
    src/mosaicengine.cpp
    src/desensitizeengine.cpp
)

set(HEADERS
    src/appsettings.h
    src/appcontroller.h
    src/filelistmodel.h
    src/pdfpreviewmodel.h
    src/officetextextractor.h
    src/pdfpagerenderer.h
    src/previewimageprovider.h
    src/updatechecker.h
    src/redactionregionmodel.h
    src/piidetector.h
    src/mosaicengine.h
    src/desensitizeengine.h
)

qt_add_executable(ProjectO ${SOURCES} ${HEADERS})

set_source_files_properties(qml/style/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

set(PROJECTO_RESOURCES
    resources/logo.svg
    resources/changelog.json
    resources/update.json
)
if(EXISTS "${CMAKE_SOURCE_DIR}/resources/app-icon.png")
    list(APPEND PROJECTO_RESOURCES resources/app-icon.png)
endif()

qt_add_qml_module(ProjectO
    URI ProjectO
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/style/Theme.qml
        qml/components/AppHeader.qml
        qml/components/SettingsDropdown.qml
        qml/components/StyledComboBox.qml
        qml/components/CompactComboBox.qml
        qml/components/DragGrip.qml
        qml/components/GlobalProgressBar.qml
        qml/components/IconDeleteButton.qml
        qml/components/StatusToast.qml
        qml/components/StyledButton.qml
        qml/components/MenuOption.qml
        qml/components/ThemeToggleButton.qml
        qml/components/ShadowCard.qml
        qml/components/FileDropZone.qml
        qml/components/FileListView.qml
        qml/components/ScrollPreview.qml
        qml/components/DesensitizeActionBar.qml
        qml/components/RedactionOverlay.qml
        qml/components/AboutDialog.qml
        qml/components/AppLogo.qml
        qml/components/ChangelogDialog.qml
        qml/components/UpdateStatusIcon.qml
        qml/pages/WorkspacePage.qml
    RESOURCES
        ${PROJECTO_RESOURCES}
)

target_link_libraries(ProjectO PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets
    Qt6::Network
    Qt6::Concurrent
)

if(TARGET Qt6::Pdf)
    target_link_libraries(ProjectO PRIVATE Qt6::Pdf)
    target_compile_definitions(ProjectO PRIVATE HAS_QT_PDF=1)
    message(STATUS "Qt6 Pdf found — using built-in PDF preview / text extract")
else()
    message(STATUS "Qt6 Pdf not found — using poppler pdftoppm fallback")
endif()

if(WIN32)
    set_target_properties(ProjectO PROPERTIES WIN32_EXECUTABLE TRUE)
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/app-icon.ico")
        target_sources(ProjectO PRIVATE "${CMAKE_SOURCE_DIR}/resources/app-icon.rc")
    endif()
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/tools/poppler/pdftoppm.exe")
    add_custom_command(TARGET ProjectO POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/tools/poppler"
            "$<TARGET_FILE_DIR:ProjectO>/tools/poppler"
        COMMENT "Copying poppler tools to build output"
    )
endif()
