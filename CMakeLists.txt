cmake_minimum_required(VERSION 2.8)
project(ltest)

#test

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")

set (CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -D_POSIX_C_SOURCE=200809L -Wno-override-init ${CMAKE_C_FLAGS}")

endif()

set_source_files_properties(tags PROPERTIES GENERATED true)

add_custom_target(tags
        COMMAND ctags -R .
	        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable (ltest
		src/armor.c
		src/cpairs.c
		src/entity.c
		src/entity_name.c
		src/entity_types.c
		src/fight.c
		src/globals.c
		src/item.c
		src/item_ui.c
		src/location.c
		src/log.c
		src/main.c
		src/map_actions.c
		src/map_ai.c
		src/map_ai_hm.c
		src/map_fov.c
		src/map_path.c
		src/map_ui.c
		src/mapgen.c
		src/mapmode.c
		src/pqueue.c
		src/queue.c
		src/random.c
		src/squad.c
		src/stealth.c
		src/ui.c
		src/weapon.c
	       )

find_package(PkgConfig)
pkg_check_modules(NCURSESW "ncursesw" REQUIRED)

include_directories(${NCURSESW_INCLUDE_DIRS})

target_link_libraries(ltest ncursesw m)

install(TARGETS ltest
	RUNTIME DESTINATION bin
	)
