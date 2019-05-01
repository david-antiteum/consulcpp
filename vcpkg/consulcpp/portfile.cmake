include(vcpkg_common_functions)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO david-antiteum/consulcpp
    REF 0.1.0-1
    SHA512 024aa88ece7e3e61024d0a51fb9cb1abed5f33e1abde11a2b53c5d3437206c8a2da5db525a473a9a35b90cfa83a078e2f092f419c27b4d1019cdb7d08dd27c00
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
	OPTIONS
		-DBUILD_EXAMPLES=OFF
)

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH "share/consulcpp/cmake")
vcpkg_copy_pdbs()

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/consulcpp RENAME copyright)

# Remove duplicate info
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/share)
