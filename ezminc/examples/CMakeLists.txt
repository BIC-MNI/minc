LINK_LIBRARIES( minc_io )

ADD_EXECUTABLE(fuzzy_volume_similarity fuzzy_volume_similarity.cpp)
ADD_EXECUTABLE(trilinear_resample trilinear_resample.cpp)
ADD_EXECUTABLE(volume_avg volume_avg.cpp)
ADD_EXECUTABLE(volume_msq_dist volume_msq_dist.cpp)
ADD_EXECUTABLE(volume_similarity volume_similarity.cpp)

INSTALL(TARGETS
   fuzzy_volume_similarity
   volume_similarity
   DESTINATION bin)

