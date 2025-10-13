set(MSVC_LIBS glfw3.lib)
set(UNIX_LIBS glfw3)

if (MSVC)

	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		# 64-bit
		target_link_directories(Engine PUBLIC
				extern/glfw/lib-vc2010-64
		)
	else ()
		# 32 bit
		target_link_directories(Engine PUBLIC
				extern/glfw/lib-vc2010-32
		)
	endif ()

	target_link_libraries(Engine PUBLIC ${MSVC_LIBS})

else ()

	target_link_directories(Engine PUBLIC
			extern/glfw/lib-mingw
	)

	target_link_libraries(Engine PUBLIC ${UNIX_LIBS})

endif ()

target_compile_definitions(Engine PRIVATE "HELIOS_API_GL46=1")