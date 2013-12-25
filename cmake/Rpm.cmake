set(RPMBUILD_DIR ~/rpmbuild CACHE STRING "Rpmbuild directory, for the rpm target")
set(MOCK_COMMAND mock CACHE STRING "Command to use for running mock")
set(MOCK_CHROOT fedora-13-x86_64 CACHE STRING "Chroot to use when building an rpm with mock")
set(RPM_DISTRO fc13 CACHE STRING "Suffix of the rpm file")
set(RPM_ARCH x86_64 CACHE STRING "Architecture of the rpm file")

add_custom_target(rpm
  COMMAND ${CMAKE_SOURCE_DIR}/dist/maketarball.sh
  COMMAND ${CMAKE_COMMAND} -E copy qtmind-${QTMIND_VERSION}.tar.gz ${RPMBUILD_DIR}/SOURCES/
  COMMAND rpmbuild -bs ${CMAKE_SOURCE_DIR}/dist/qtmind.spec
  COMMAND ${MOCK_COMMAND}
      --verbose
      --root=${MOCK_CHROOT}
      --resultdir=${CMAKE_BINARY_DIR}/mock_result/
      ${RPMBUILD_DIR}/SRPMS/qtmind-${QTMIND_VERSION}-${QTMIND_VERSION}.${RPM_DISTRO}.src.rpm
  COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_BINARY_DIR}/mock_result/qtmind-${QTMIND_VERSION}-${QTMIND_VERSION}.${RPM_DISTRO}.${RPM_ARCH}.rpm
      ${CMAKE_BINARY_DIR}/qtmind-${QTMIND_VERSION}-${QTMIND_VERSION}.${RPM_DISTRO}.${RPM_ARCH}.rpm
)
