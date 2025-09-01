# CMakeList.txt : CMake project for contador, include source and define
# project specific logic here.
#
function(load_gtkmm)

  if(LINUX)
    find_package(CURL REQUIRED)
  endif()

  if (NOT VCPKG_TOOLCHAIN)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTKMM gtkmm-3.0)
    include_directories(${GTKMM_INCLUDE_DIRS})
    link_directories(${GTKMM_LIBRARY_DIRS})
    link_libraries(${GTKMM_LIBRARIES})

    pkg_check_modules(GLIB REQUIRED glib-2.0)
    link_libraries(${GLIB_LIBRARIES})
  else()
    find_package(gtkmm CONFIG REQUIRED)
    target_link_libraries(WeimachClassificadora PRIVATE gtkmm::gtkmm)
    target_link_libraries(WeimachClassificadora PRIVATE gtk3::gtk3)
  endif()
endfunction(load_gtkmm)
function(load_system)
  if (MSVC)
    set(GUI_OPTION WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4250")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
  elseif (APPLE)
    set(GUI_OPTION MACOSX_BUNDLE)
  else()
    add_compile_options(-O2 -Werror)
  endif ()
endfunction(load_system)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("contador")

load_gtkmm()
load_system()

# Add source to this project's executable.
add_executable (contador "contador.cpp" "contador.h" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET contador PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
