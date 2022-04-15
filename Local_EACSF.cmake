cmake_minimum_required (VERSION 2.8.11)
project (Local_EACSF)

#Qt example------------------------------------

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/lib)


if(NOT INSTALL_RUNTIME_DESTINATION)
	set(INSTALL_RUNTIME_DESTINATION bin)
endif(NOT INSTALL_RUNTIME_DESTINATION)

if(NOT INSTALL_LIBRARY_DESTINATION)
	set(INSTALL_LIBRARY_DESTINATION lib)
endif(NOT INSTALL_LIBRARY_DESTINATION)

if(NOT INSTALL_ARCHIVE_DESTINATION)
	set(INSTALL_ARCHIVE_DESTINATION lib)
endif(NOT INSTALL_ARCHIVE_DESTINATION)


# Find the QtWidgets library

# find Qt5 headers
if(UNIX AND NOT APPLE)
  find_package(Qt5 COMPONENTS Widgets Gui X11Extras DBus REQUIRED)
else(UNIX AND NOT APPLE)
  find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif(UNIX AND NOT APPLE)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})

set(QT_LIBRARIES ${Qt5Widgets_LIBRARIES})

# get Local_EACSF info
FILE(READ src/app/Local_EACSF.xml var)

string(REGEX MATCH "<title>.*</title>" ext "${var}")
string(REPLACE "<title>" "" title ${ext})
string(REPLACE "</title>" "" title ${title})
add_definitions(-DLocal_EACSF_TITLE="${title}")

string(REGEX MATCH "<version>.*</version>" ext "${var}")
string(REPLACE "<version>" "" version ${ext})
string(REPLACE "</version>" "" version ${version})
add_definitions(-DLocal_EACSF_VERSION="${version}")

string(REGEX MATCH "<contributor>.*</contributor>" ext "${var}")
string(REPLACE "<contributor>" "" contributors ${ext})
string(REPLACE "</contributor>" "" contributors ${contributors})
add_definitions(-DLocal_EACSF_CONTRIBUTORS="${contributors}")

add_subdirectory(src)

find_program(Local_EACSF_LOCATION 
      Local_EACSF
      HINTS ${CMAKE_CURRENT_BINARY_DIR}/bin)
    if(Local_EACSF_LOCATION)
      install(PROGRAMS ${Local_EACSF_LOCATION}
        DESTINATION ${INSTALL_RUNTIME_DESTINATION}
        COMPONENT RUNTIME)
    endif()

configure_file(CMake/Local_EACSFConfig.cmake.in
  "${PROJECT_BINARY_DIR}/Local_EACSFConfig.cmake" @ONLY)

if(WIN32 AND NOT CYGWIN)
  set(INSTALL_CMAKE_DIR cmake)
else()
  set(INSTALL_CMAKE_DIR lib/cmake/Local_EACSF)
endif()

install(FILES
  "${PROJECT_BINARY_DIR}/Local_EACSFConfig.cmake"  
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)


file(GLOB Local_EACSF_HEADERS "*.h")
install(FILES ${Local_EACSF_HEADERS} 
DESTINATION include)

#option(BUILD_TESTING "Build the testing tree" ON)

#IF(BUILD_TESTING)
#  include(CTest)
#  ADD_SUBDIRECTORY(Testing)
#ENDIF(BUILD_TESTING)
