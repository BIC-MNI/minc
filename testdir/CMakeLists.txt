
LINK_LIBRARIES( ${MINC_LIBS} )

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
TARGET_LINK_LIBRARIES(create_grid_xfm ${VOLUME_IO_LIB} ${MINC_LIBS})

MACRO(minc_test cmd)
  # minc 1 version
  add_test(${cmd}-1 ${CMAKE_CURRENT_SOURCE_DIR}/run_test_cmake.sh ${CMAKE_CURRENT_BINARY_DIR}/${cmd} ${CMAKE_CURRENT_SOURCE_DIR}/${cmd}.out)
  # minc 2 version
  add_test(${cmd}-2 ${CMAKE_CURRENT_SOURCE_DIR}/run_test2_cmake.sh ${CMAKE_CURRENT_BINARY_DIR}/${cmd} ${CMAKE_CURRENT_SOURCE_DIR}/${cmd}.out)
ENDMACRO(minc_test)



# running tests
minc_test(minc_types)
minc_test(icv_dim1)
minc_test(icv_dim)
minc_test(icv_fillvalue)
minc_test(icv_range)


add_test(arg_parse ${CMAKE_CURRENT_SOURCE_DIR}/run_test_arg_parse_cmake.sh ${CMAKE_CURRENT_BINARY_DIR}/test_arg_parse)
add_test(icv icv)
add_test(minc minc_tst)

add_test(mincapi mincapi)
add_test(test_arg_parse test_arg_parse)
add_test(test_mconv test_mconv)

# TODO port these test to cmake
#add_test(create_grid_xfm create_grid_xfm)
#add_test(test_speed test_speed)
#add_test(test_xfm test_xfm)

TARGET_LINK_LIBRARIES(test_xfm ${VOLUME_IO_LIB} ${MINC_LIBS})
