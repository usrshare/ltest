cmake_minimum_required(VERSION 2.8)
project(ltest)

#test

set (CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -D_POSIX_C_SOURCE=2 -Wno-override-init")

set_source_files_properties(tags PROPERTIES GENERATED true)

add_custom_target(tags
        COMMAND ctags -R .
	        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories (${CMAKE_SOURCE_DIR}/inc)

add_executable (ltest
		src/armor.c
		src/cpairs.c
		src/entity.c
		src/entity_name.c
		src/entity_types.c
		src/fight.c
		src/globals.c
		src/item.c
		src/location.c
		src/main.c
		src/map_actions.c
		src/map_ai.c
		src/map_fov.c
		src/map_path.c
		src/map_ui.c
		src/mapgen.c
		src/mapmode.c
		src/pqueue.c
		src/random.c
		src/stealth.c
		src/ui.c
		src/weapon.c
	       )

find_package(PkgConfig)
pkg_check_modules(NCURSESW "ncursesw" REQUIRED)

include_directories(${NCURSESW_INCLUDE_DIRS})

target_link_libraries(ltest ncursesw menu m)

install(TARGETS ltest
	RUNTIME DESTINATION bin
	)
