

add_executable(testEval test.cpp ../ENLink.h)
TARGET_LINK_LIBRARIES(testEval ENLink ${Boost_LIBRARIES})

install(TARGETS testEval
		DESTINATION ${CMAKE_BINARY_DIR}/bin
		CONFIGURATIONS Release RelWithDebInfo Debug)