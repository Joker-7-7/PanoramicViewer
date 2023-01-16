cmake_minimum_required(VERSION 3.5)

project(PanoramicViewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS REQUIRED
        Quick
        Multimedia
        Core
        Gui
        Widgets
        OpenGL
        MultimediaWidgets
        OpenGLWidgets
        )

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find package VTK
find_package(VTK 9.1 COMPONENTS REQUIRED
        IOGeometry
        IOLegacy
        IOPLY
        IOImage
        IOXML
        ImagingCore
        GUISupportQt
        GUISupportQtQuick
        RenderingContextOpenGL2
        RenderingCore
        RenderingFreeType
        RenderingGL2PSOpenGL2
        RenderingOpenGL2
        RenderingVolume
        RenderingVolumeOpenGL2
        CommonCore
        CommonDataModel
        InteractionWidgets
        InteractionStyle
        RenderingAnnotation
        DICOM
        InteractionImage
        )


file (GLOB_RECURSE Sources Src/*.c*)
file (GLOB_RECURSE Headers Src/*.h*)
file (GLOB         QtResources *.qrc)
file (GLOB         Ui Src/*.ui)

# Build executable
add_executable(${PROJECT_NAME} WIN32 ${Sources} ${QtResources} ${Ui} ${Headers})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets
        PRIVATE Qt6::OpenGL
        PRIVATE Qt6::Multimedia
        PRIVATE Qt6::MultimediaWidgets
        PRIVATE Qt6::Quick
        PRIVATE Qt6::OpenGLWidgets
        PRIVATE ${VTK_LIBRARIES}
        )

vtk_module_autoinit(
        TARGETS ${PROJECT_NAME}
        MODULES ${VTK_LIBRARIES})
