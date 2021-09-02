if(IRIS_PLATFORM MATCHES "WINDOWS")
  set(CPACK_GENERATOR ZIP)
  set(CPACK_SOURCE_GENERATOR ZIP)
else()
  set(CPACK_GENERATOR TGZ)
  set(CPACK_SOURCE_GENERATOR TGZ)
endif()

set(CPACK_PACKAGE_VENDOR "iris")
set(CPACK_PACKAGE_CONTACT "")
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
string(TOLOWER ${IRIS_PLATFORM} platform)
string(TOLOWER ${IRIS_PLATFORM} platform)
string(TOLOWER ${IRIS_ARCH} arch)
string(TOLOWER ${PROJECT_NAME} project)
set(CPACK_SYSTEM_NAME "${platform}-${arch}")

include(CPack)

