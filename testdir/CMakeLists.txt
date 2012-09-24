
LINK_LIBRARIES(${MINC2_LIBRARIES})

ADD_EXECUTABLE(minc_tst minc.c)
ADD_EXECUTABLE(icv icv.c)

ADD_EXECUTABLE(icv_dim1 icv_dim1.c)
ADD_EXECUTABLE(icv_dim icv_dim.c)
ADD_EXECUTABLE(icv_fillvalue icv_fillvalue.c)
ADD_EXECUTABLE(icv_range icv_range.c)
ADD_EXECUTABLE(mincapi mincapi.c)
ADD_EXECUTABLE(minc_types minc_types.c)
ADD_EXECUTABLE(test_arg_parse test_arg_parse.c)
ADD_EXECUTABLE(test_mconv test_mconv.c)
ADD_EXECUTABLE(test_speed test_speed.c)
ADD_EXECUTABLE(test_xfm test_xfm.c)

ADD_EXECUTABLE(create_grid_xfm create_grid_xfm.c)
TARGET_LINK_LIBRARIES(create_grid_xfm ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)

MACRO(minc_test cmd)
  # minc 1 version
  ADD_TEST(${cmd}-1 ${CMAKE_CURRENT_SOURCE_DIR}/run_test_cmake.sh ${CMAKE_CURRENT_BINARY_DIR}/${cmd} ${CMAKE_CURRENT_SOURCE_DIR}/${cmd}.out)
  
  IF(MINC2_BUILD_V2)
  # minc 2 version
    ADD_TEST(${cmd}-2 ${CMAKE_CURRENT_SOURCE_DIR}/run_test2_cmake.sh ${CMAKE_CURRENT_BINARY_DIR}/${cmd} ${CMAKE_CURRENT_SOURCE_DIR}/${cmd}.out)
  ENDIF(MINC2_BUILD_V2)
ENDMACRO(minc_test)


# running tests
minc_test(minc_types)
minc_test(icv_dim1)
minc_test(icv_dim)
minc_test(icv_fillvalue)
minc_test(icv_range)


ADD_TEST(arg_parse ${CMAKE_CURRENT_SOURCE_DIR}/run_test_arg_parse_cmake.sh ${CMAKE_CURRENT_BINARY_DIR}/test_arg_parse)
ADD_TEST(icv icv)
ADD_TEST(minc minc_tst)

ADD_TEST(mincapi mincapi)
ADD_TEST(test_arg_parse test_arg_parse)
ADD_TEST(test_mconv test_mconv)

# TODO port these test to cmake
#ADD_TEST(create_grid_xfm create_grid_xfm)
#ADD_TEST(test_speed test_speed)
#ADD_TEST(test_xfm test_xfm)

TARGET_LINK_LIBRARIES(test_xfm ${VOLUME_IO_LIBRARY} ${MINC2_LIBRARIES} m)
