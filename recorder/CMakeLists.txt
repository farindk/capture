cmake_minimum_required(VERSION 3.17)
project(recorder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets Multimedia MultimediaWidgets)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_executable(${PROJECT_NAME} main.cpp
        camera/CameraCapture.cpp camera/CameraCapture.h
        camera/UICameraSettings.cpp camera/UICameraSettings.h
        camera/UICameraSettings.ui Settings.cpp Settings.h
        MainWindow.cpp MainWindow.h MainWindow.ui
        VideoWidget.cpp VideoWidget.h
        Error.cpp Error.h)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE .)

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED} pthread)
target_compile_definitions(${PROJECT_NAME} PRIVATE BUILDDATE=${TODAY} EXPIRATION_DURATION_MONTHS=${EXPIRATION_DURATION_MONTHS})
