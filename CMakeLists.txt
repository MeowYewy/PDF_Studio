cmake_minimum_required(VERSION 3.21)
project(PageCase VERSION 0.2.4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(DEFINED ENV{QT_DIR})
    set(QT_ROOT "$ENV{QT_DIR}" CACHE PATH "Qt installation path")
elseif(NOT DEFINED QT_ROOT)
    set(QT_ROOT "D:/Qt/6.11.1/mingw_64" CACHE PATH "Qt installation path")
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

# Prefer Qt 6 default QML resource prefix :/qt/qml/
qt_policy(SET QTP0001 NEW)

set(SOURCES
    src/main.cpp
    src/appsettings.cpp
    src/appcontroller.cpp
    src/filelistmodel.cpp
    src/pdfpreviewmodel.cpp
    src/officeconverter.cpp
    src/officetextextractor.cpp
    src/pdfengine.cpp
    src/pdfpagerenderer.cpp
    src/previewimageprovider.cpp
    src/updatechecker.cpp
    src/watermarklayout.cpp
    src/filepicker.cpp
)

set(HEADERS
    src/appsettings.h
    src/appcontroller.h
    src/filelistmodel.h
    src/pdfpreviewmodel.h
    src/officeconverter.h
    src/officetextextractor.h
    src/pdfengine.h
    src/pdfpagerenderer.h
    src/previewimageprovider.h
    src/updatechecker.h
    src/watermarklayout.h
    src/filepicker.h
)

qt_add_executable(PageCase ${SOURCES} ${HEADERS})

set_source_files_properties(qml/style/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

set(PAGECASE_RESOURCES
    resources/logo.svg
    resources/changelog.json
    resources/update.json
    resources/icons/loader-circle.svg
    resources/icons/chevron-left.svg
    resources/icons/menu.svg
    resources/icons/hard-drive.svg
    resources/icons/folder.svg
    resources/icons/file-text.svg
    resources/icons/file-type.svg
    resources/icons/file-video.svg
    resources/icons/file-volume.svg
    resources/icons/file-braces.svg
    resources/icons/pdf.svg
    resources/icons/copy.svg
    resources/icons/scissors.svg
    resources/icons/clipboard-paste.svg
    resources/icons/trash-2.svg
    resources/icons/pencil.svg
    resources/icons/file-image.svg
    resources/icons/file.svg
)
if(EXISTS "${CMAKE_SOURCE_DIR}/resources/app-icon.png")
    list(APPEND PAGECASE_RESOURCES resources/app-icon.png)
endif()

qt_add_qml_module(PageCase
    URI PageCase
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/style/Theme.qml
        qml/components/AppHeader.qml
        qml/components/FeatureTabs.qml
        qml/components/SettingsDropdown.qml
        qml/components/StyledComboBox.qml
        qml/components/RotateComboBox.qml
        qml/components/CompactComboBox.qml
        qml/components/ConvertComboBox.qml
        qml/components/CompressComboBox.qml
        qml/components/WatermarkColorPicker.qml
        qml/components/WatermarkCountCombo.qml
        qml/components/WatermarkInputField.qml
        qml/components/DragGrip.qml
        qml/components/GlobalProgressBar.qml
        qml/components/IconDeleteButton.qml
        qml/components/StatusToast.qml
        qml/components/SwitchToggle.qml
        qml/components/StyledCheckBox.qml
        qml/components/StyledButton.qml
        qml/components/MenuOption.qml
        qml/components/ThemeToggleButton.qml
        qml/components/ShadowCard.qml
        qml/components/FileDropZone.qml
        qml/components/FileKindIcon.qml
        qml/components/LucideIcon.qml
        qml/components/PickerFilterCombo.qml
        qml/components/PathBreadcrumbBar.qml
        qml/components/FilePickerIconButton.qml
        qml/components/FileListView.qml
        qml/components/ScrollPreview.qml
        qml/components/ActionBar.qml
        qml/components/AboutDialog.qml
        qml/components/AppLogo.qml
        qml/components/ChangelogDialog.qml
        qml/components/FilePickerDialog.qml
        qml/components/UpdateStatusIcon.qml
        qml/components/UpdateNewVersionLabel.qml
        qml/pages/PageBase.qml
        qml/pages/SplitPage.qml
        qml/pages/MergePage.qml
        qml/pages/RotatePage.qml
        qml/pages/ConvertPage.qml
        qml/pages/CompressPage.qml
        qml/pages/WatermarkPage.qml
    RESOURCES
        ${PAGECASE_RESOURCES}
)

target_link_libraries(PageCase PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets
    Qt6::Network
    Qt6::Concurrent
)

if(TARGET Qt6::Pdf)
    target_link_libraries(PageCase PRIVATE Qt6::Pdf)
    target_compile_definitions(PageCase PRIVATE HAS_QT_PDF=1)
    message(STATUS "Qt6 Pdf found — using built-in PDF preview")
else()
    message(STATUS "Qt6 Pdf not found — using poppler pdftoppm fallback")
endif()

if(WIN32)
    set_target_properties(PageCase PROPERTIES WIN32_EXECUTABLE TRUE)
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/app-icon.ico")
        target_sources(PageCase PRIVATE "${CMAKE_SOURCE_DIR}/resources/app-icon.rc")
    endif()
endif()

add_custom_command(TARGET PageCase POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/tools/qpdf"
        "$<TARGET_FILE_DIR:PageCase>/tools/qpdf"
    COMMENT "Copying qpdf tools to build output"
)

if(EXISTS "${CMAKE_SOURCE_DIR}/tools/poppler/pdftoppm.exe")
    add_custom_command(TARGET PageCase POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/tools/poppler"
            "$<TARGET_FILE_DIR:PageCase>/tools/poppler"
        COMMENT "Copying poppler tools to build output"
    )
endif()
