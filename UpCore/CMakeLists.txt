add_custom_command(OUTPUT TMidasEventDict.cxx TMidasEventDict_rdict.pcm TMidasEvent.rootmap COMMAND ../scripts/build_dict.sh TMidasEvent.h TMidasEventDict.cxx TMidasEvent.rootmap libUnpackerCore.dylib DEPENDS TMidasEvent.h)
add_library(UnpackerCore SHARED MidasBank.cxx MidasEventFragment.cxx MidasFile.cxx TMidasEvent.cxx TMidasEventDict.cxx UnpackerOptionManager.cxx VUserPoint.cxx UserPoint.cxx Sharc.cxx Tigress.cxx Trifoil.cxx)
target_link_libraries(UnpackerCore ${ROOT_LIBRARIES} ${NPLib_LIBRARIES} -lNPSharc -lNPTigress -lNPTrifoil)

