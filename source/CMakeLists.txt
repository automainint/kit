add_subdirectory(kit)
add_subdirectory(kit_test)

if(KIT_ENABLE_TESTING)
  add_subdirectory(test)
endif()
