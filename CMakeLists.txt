cmake_minimum_required(VERSION 3.20)

project(find-and-replace)

set(CMAKE_CXX_STANDARD 17)

find_package(FLTK CONFIG REQUIRED)

add_executable(find-and-replace
	src/main.cpp
	src/logic/logic.cpp
)

target_include_directories(find-and-replace PRIVATE
	src/logic
)

target_link_libraries(find-and-replace PRIVATE
	fltk
	fltk_gl
	fltk_forms
	fltk_images
)