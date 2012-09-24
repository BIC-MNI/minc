LINK_LIBRARIES(minc_io ${LIBMINC_LIBRARIES})

ADD_EXECUTABLE(ezminc_rw_test ezminc_rw_test.cpp)
#ADD_EXECUTABLE(ezminc_stats   ezminc_stats.cpp)

ADD_TEST(ezminc_rw_test ezminc_rw_test ${CMAKE_CURRENT_BINARY_DIR})