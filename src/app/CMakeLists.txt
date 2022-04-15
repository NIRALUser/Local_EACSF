#cmake_minimum_required(VERSION 2.8.7)
cmake_policy(SET CMP0071  NEW)
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)


SET(APP_NAME Local_EACSF)

set(${APP_NAME}_moc csfwindow.h)
set(${APP_NAME}_ui csfwindow.ui )
set(${APP_NAME}_rcc Local_EACSF_Resources.qrc)

set_property(SOURCE csfwindow.h PROPERTY SKIP_AUTOMOC ON)

qt5_wrap_cpp(${APP_NAME}_generated_moc ${${APP_NAME}_moc})
qt5_wrap_ui(${APP_NAME}_generated_ui ${${APP_NAME}_ui})
qt5_add_resources(${APP_NAME}_generated_rcc ${${APP_NAME}_rcc})

list(APPEND ${APP_NAME}_src
	    csfwindow.cpp
	    extexecutableswidget.cxx
	    csfscripts.cpp
	    model.cpp
	    addWidget.cpp
	    ${${APP_NAME}_generated_moc}
	    ${${APP_NAME}_generated_ui}
	    ${${APP_NAME}_generated_rcc}
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

SEMMacroBuildCLI(
	NAME ${APP_NAME}
	EXECUTABLE_ONLY
	ADDITIONAL_SRCS ${${APP_NAME}_src}
	TARGET_LIBRARIES ${QT_LIBRARIES}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
	INSTALL_RUNTIME_DESTINATION ${INSTALL_RUNTIME_DESTINATION}
	INSTALL_LIBRARY_DESTINATION ${INSTALL_LIBRARY_DESTINATION}
	INSTALL_ARCHIVE_DESTINATION ${INSTALL_ARCHIVE_DESTINATION}
)






option(CREATE_BUNDLE "Create a bundle" OFF)

# Generate a bundle
if(CREATE_BUNDLE)
  
  set(CPACK_PACKAGE_VERSION ${version})

  if(APPLE)
    set(OS_BUNDLE MACOSX_BUNDLE)
  elseif(WIN32)
    set(OS_BUNDLE WIN32)
  endif()

  set(bundle_name Local_EACSF${version})

  #--------------------------------------------------------------------------------
  SET(qtconf_dest_dir bin)
  
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${bundle_name}")

	if(APPLE)
	    SET(qtconf_dest_dir ${bundle_name}.app/Contents/Resources)
	    SET(APPS "\${CMAKE_INSTALL_PREFIX}/${bundle_name}.app")

	    set(MACOSX_BUNDLE_BUNDLE_NAME "${bundle_name}")
	    set(MACOSX_BUNDLE_INFO_STRING "Local_EACSF: Local measurement of extra axial cerebrospinal fluid.")
	    set(MACOSX_BUNDLE_LONG_VERSION_STRING "Local_EACSF version - ${version}")
	    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${version}")
	    set(MACOSX_BUNDLE_BUNDLE_VERSION "${version}")
	    set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2021 University of North Carolina , Chapel Hill.")
	    
	    set_source_files_properties(
	      PROPERTIES
	      MACOSX_PACKAGE_LOCATION Resources
	      )
	    macro(install_qt5_plugin _qt_plugin_name _qt_plugins_var)
	      get_target_property(_qt_plugin_path "${_qt_plugin_name}" LOCATION)
	      if(EXISTS "${_qt_plugin_path}")
	        get_filename_component(_qt_plugin_file "${_qt_plugin_path}" NAME)
	        get_filename_component(_qt_plugin_type "${_qt_plugin_path}" PATH)
	        get_filename_component(_qt_plugin_type "${_qt_plugin_type}" NAME)
	        set(_qt_plugin_dest "${bundle_name}.app/Contents/PlugIns/${_qt_plugin_type}")
	        install(FILES "${_qt_plugin_path}"
	          DESTINATION "${_qt_plugin_dest}"
	          COMPONENT Runtime)
	        set(${_qt_plugins_var}
	          "${${_qt_plugins_var}};\${CMAKE_INSTALL_PREFIX}/${_qt_plugin_dest}/${_qt_plugin_file}")
	      else()
	        message(FATAL_ERROR "QT plugin ${_qt_plugin_name} not found")
	      endif()
	    endmacro()
	    
	    install_qt5_plugin("Qt5::QCocoaIntegrationPlugin" QT_PLUGINS)

	    set(MACOSX_BUNDLE_ICON_FILE Local_EACSF.icns)
	    set(myApp_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Local_EACSF.icns)
	    set_source_files_properties(${myApp_ICON} PROPERTIES
	       MACOSX_PACKAGE_LOCATION "Resources")
	    set(CPACK_PACKAGE_ICON "${myApp_ICON}")

	    include_directories(${CMAKE_CURRENT_BINARY_DIR})
	    add_executable(${bundle_name} ${OS_BUNDLE} ${myApp_ICON} Local_EACSF.cxx ${${APP_NAME}_src})
	    target_link_libraries(${bundle_name} ${QT_LIBRARIES} ${VTK_LIBRARIES})

	    #--------------------------------------------------------------------------------
	    # Install the QtTest application, on Apple, the bundle is at the root of the
	    # install tree, and on other platforms it'll go into the bin directory.
	    INSTALL(TARGETS ${bundle_name}
	      DESTINATION . COMPONENT Runtime
	      RUNTIME DESTINATION bin COMPONENT Runtime
	    )

	    SET(INSTALL_RUNTIME_DESTINATION ${bundle_name}.app/Contents/bin/)
	    

	    set(APP_EXECUTABLES 
	    	AddScalarstoPolyData
			ComputeAverageMesh
			ComputeCSFVolume
			CreateOuterImage
			CreateOuterSurface
			EACSFKlaplace
			EditPolyData
			EstimateCortexStreamlinesDensity
			FitPlane
			HeatKernelSmoothing
			Local_EACSF
			ROImean
			)
			
		foreach(app_exec ${APP_EXECUTABLES})


			find_program(${app_exec}_location
		        ${app_exec}
		        PATHS ${CMAKE_BINARY_DIR}/bin)

	        if(EXISTS "${${app_exec}_location}.xml")
	          install(PROGRAMS ${${app_exec}_location} 
	            DESTINATION ${INSTALL_RUNTIME_DESTINATION}
	            COMPONENT RUNTIME)

	          install(FILES ${${app_exec}_location}.xml
	            DESTINATION ${INSTALL_RUNTIME_DESTINATION}
	            COMPONENT RUNTIME)


	        else()
	          install(PROGRAMS ${${app_exec}_location} 
	            DESTINATION ${INSTALL_RUNTIME_DESTINATION}
	            COMPONENT RUNTIME)      
	        endif()
	      endforeach()

	endif()

	IF(WIN32)
		SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${bundle_name}.exe")
	ENDIF(WIN32)

	#--------------------------------------------------------------------------------
	# install a qt.conf file
	# this inserts some cmake code into the install script to write the file
	INSTALL(CODE "
	  file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"[Paths]\nPlugins = PlugIns\n\")
	  " COMPONENT Runtime)

	# Install the license
	INSTALL(FILES 
	${CMAKE_SOURCE_DIR}/LICENSE
	DESTINATION "${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}"
	COMPONENT Runtime)

	get_target_property(Qt5_location Qt5::Widgets LOCATION)
	string(FIND ${Qt5_location} "/QtWidgets" length)
	string(SUBSTRING ${Qt5_location} 0 ${length} Qt5_location)
	#Fix the bundle
	install(CODE "
	include(BundleUtilities)
	fixup_bundle(\"${APPS}\" \"${QT_PLUGINS};\" \"${Qt5_location}\")
	file(GLOB qt_libs \${CMAKE_INSTALL_PREFIX}/bin/*.so*)
	  foreach(qt_lib \${qt_libs})
	    get_filename_component(qt_file \"\${qt_lib}\" NAME)
	    file(RENAME \${qt_lib} \${CMAKE_INSTALL_PREFIX}/lib/\${qt_file})
	  endforeach()
	"
	COMPONENT Runtime)

	# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
	# where CPackConfig.cmake is created by including CPack
	# And then there's ways to customize this as well
	set(CPACK_BINARY_DRAGNDROP ON)

endif()


if(UNIX AND NOT APPLE)
 # # when building, don't use the install RPATH already
 # # (but later on when installing)
 # SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
 # # the RPATH to be used when installing

  string(REPLACE "." ";" VERSION_LIST ${version})
  list(GET VERSION_LIST 0 CPACK_PACKAGE_VERSION_MAJOR)
  list(GET VERSION_LIST 1 CPACK_PACKAGE_VERSION_MINOR)
  list(GET VERSION_LIST 2 CPACK_PACKAGE_VERSION_PATCH)

  SET(CMAKE_INSTALL_RPATH "../lib")
  
  get_target_property(QtWidgets_location Qt5::Widgets LOCATION)
  get_target_property(QtCore_location Qt5::Core LOCATION)
  get_target_property(QtGui_location Qt5::Gui LOCATION)
  get_target_property(QtX11Extras_location Qt5::X11Extras LOCATION)
  get_target_property(QtDBus_location Qt5::DBus LOCATION)
  
  get_target_property(QtWidgets_SOFT Qt5::Widgets IMPORTED_SONAME_RELEASE)
  get_target_property(QtCore_SOFT Qt5::Core IMPORTED_SONAME_RELEASE)
  get_target_property(QtGui_SOFT Qt5::Gui IMPORTED_SONAME_RELEASE)
  get_target_property(QtX11Extras_SOFT Qt5::X11Extras IMPORTED_SONAME_RELEASE)
  get_target_property(QtDBus_SOFT Qt5::DBus IMPORTED_SONAME_RELEASE)

  get_target_property(Qt5_location Qt5::Widgets LOCATION)  
  string(FIND ${Qt5_location} "libQt5Widgets" length)
  string(SUBSTRING ${Qt5_location} 0 ${length} Qt5_location)
  
  #install(FILES ${QtWidgets_location} ${QtOpenGL_location} ${Qt5_location}/${QtWidgets_SOFT} ${Qt5_location}/${QtOpenGL_SOFT}
  #    DESTINATION lib
  #    COMPONENT Runtime)  

  install(FILES ${QtWidgets_location} ${QtCore_location} ${QtGui_location} ${QtDBus_location} ${QtX11Extras_location}
    ${Qt5_location}/${QtWidgets_SOFT} ${Qt5_location}/${QtCore_SOFT} ${Qt5_location}/${QtGui_SOFT} ${Qt5_location}/${QtX11Extras_SOFT} ${Qt5_location}/${QtDBus_SOFT}
    ${Qt5_location}/libQt5XcbQpa.so.5.11.2 ${Qt5_location}/libQt5XcbQpa.so.5
    ${Qt5_location}/libicui18n.so.56.1 ${Qt5_location}/libicuuc.so.56.1 ${Qt5_location}/libicudata.so.56.1 ${Qt5_location}/libicui18n.so.56 ${Qt5_location}/libicuuc.so.56 ${Qt5_location}/libicudata.so.56
      DESTINATION lib
      COMPONENT Runtime)

  set(QT_PLUGINS "")
  set(qt_plugins Qt5::QXcbIntegrationPlugin)
  foreach(qt_plugin ${qt_plugins})

    get_target_property(_qt_plugin_path "${qt_plugin}" LOCATION)
    get_filename_component(_qt_plugin_file "${_qt_plugin_path}" NAME)
    get_filename_component(_qt_plugin_type "${_qt_plugin_path}" PATH)
    get_filename_component(_qt_plugin_type "${_qt_plugin_type}" NAME)

    install(PROGRAMS "${_qt_plugin_path}"
          DESTINATION "bin/${_qt_plugin_type}"
          COMPONENT RuntimePlugins)
  endforeach()

  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Auto_EACSF.sh "\#!/bin/bash\nSCRIPT=$(readlink -f $(realpath -s $0))\nSCRIPTPATH=$(dirname $SCRIPT)\nexport LD_LIBRARY_PATH=$SCRIPTPATH/lib:$LD_LIBRARY_PATH\n$SCRIPTPATH/bin/Auto_EACSF $@\n")

  install(CODE "
    file(
      COPY ${CMAKE_CURRENT_BINARY_DIR}/Auto_EACSF.sh
      DESTINATION \${CMAKE_INSTALL_PREFIX}
      FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )
    " COMPONENT Runtime)

endif()


# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well  
include(InstallRequiredSystemLibraries)
include(CPack) 


