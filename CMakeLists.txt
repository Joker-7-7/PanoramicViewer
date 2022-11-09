cmake_minimum_required(VERSION 3.5)

project(PanoramivViewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Core REQUIRED)
find_package(Qt6Gui REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6OpenGL REQUIRED)
find_package(Qt6Multimedia REQUIRED)
find_package(Qt6MultimediaWidgets REQUIRED)
find_package(Qt6OpenGLWidgets REQUIRED)

# List source files & resources
file (GLOB Sources *.cpp *.cxx)
file (GLOB Headers *.h)
file (GLOB Resources *.qrc)
file (GLOB UIs *.ui)

# Find package VTK
find_package(VTK
    9.1
    COMPONENTS
    IOGeometry
    IOLegacy
    IOPLY
    IOImage
    IOXML
    ImagingCore
    GUISupportQt
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
    CommonComputationalGeometry
REQUIRED
)




# Build executable
add_executable(PanoramivViewer WIN32 ${Sources} ${Headers} ${Resources} ${UIs})

target_link_libraries(PanoramivViewer PRIVATE Qt6::Widgets Qt6::OpenGL Qt6::Multimedia Qt6::MultimediaWidgets Qt6::OpenGLWidgets ${VTK_LIBRARIES} )

vtk_module_autoinit(
    TARGETS PanoramivViewer
    MODULES ${VTK_LIBRARIES})
