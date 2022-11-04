1. Prepare source code:

	- Clone source code from https://github.com/Halolo/orange-pi-distro using git command:
	git clone --recursive-submodule https://github.com/Halolo/orange-pi-distro (this command will clone submodule in this repository)
	- Clone https://github.com/linux-sunxi/meta-sunxi.git to get sdcard_image-sunxi.bbclass (in classs folder)
	- Copy folder class in meta-sunxi to meta-opi
	- Modify line in poky/meta/conf/bitbake.conf : IMAGE_NAME = "${IMAGE_BASENAME}-${MACHINE}${IMAGE_VERSION_SUFFIX}" to IMAGE_NAME = "${IMAGE_BASENAME}-${MACHINE}" due to ${IMAGE_VERSION_SUFFIX} cause error when create image file
	- Add line in poky/meta/classes/core-image.bbclass : inherit ../../meta-opi/classes/sdcard_image-sunxi to create image file
	- Remove all sync command in sdcard_image-sunxi.bbclass
	- Add SPL_BINARY="./u-boot-sunxi-with-spl.bin" in meta-opi/class/sdcard_image-sunxi.bbclass or modify below line in source-me:
		DISTRO = "opi"' >> "$BUILD_DIR/conf/local.conf"
	to 
		DISTRO = "opi"
 		SPL_BINARY = "./u-boot-sunxi-with-spl.bin"' >> "$BUILD_DIR/conf/local.conf"
	- Add line in source-me file: "$DIR/meta-sunxi \\" right below  "$DIR/meta-opi \\"
	- Copy meta-sunxi folder to orange-pi-distro (where has meta-opi folder)
2. Build the source code
	- source environment: . source orange-pi-pc-plus
	- Start build: bitbake opipcplus-minimal

3. Flash image file to sdcard:
	- Download etcher from balena.io/etcher
	- flash image file, which file has sunfix: sunxi-sdimg (build/tmp-glibc/deploy/images/orange-pi-pc-plus)
	
4. Add application into image file
- In meta-, which we working on it now, add recipes to contain source code: recipes-xxx/helloworld/file/src   (file/src due to in .bb file:://src <--> /file/src/)
create 1 .bb file inside helloworld with content:

	SUMMARY = "test application helloworld"
	DESCRIPTION = "Custom recipe to build helloworld.c application"
	LICENSE = "MIT"
	LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

	# Where to find source files (can be local, GitHub, etc.)
	SRC_URI = "file://src"

	# Where to keep downloaded source files (in tmp/work/...)
	S = "${WORKDIR}/src"

	# Pass arguments to linker
	TARGET_CC_ARCH += "${LDFLAGS}"

	# Cross-compile source code
	do_compile() {
	    ${CC} -o helloworld helloworld.c (for compiling c application, optional)
	    ${CXX} -o hello_world hello_world.cpp (forcompiling cpp application, optional)
	}

	# Create /usr/bin in rootfs and copy program to it
	do_install() {
	    install -d ${D}${bindir}
	    install -m 0755 helloworld ${D}${bindir} (modify permission for application)
	}

- configure for running application as services (in image file):
    + create file recipes-xxx/systemd/helloworldsystemd_1.00.bb with below content:

	LICENSE = "CLOSED"
	inherit systemd

	SYSTEMD_AUTO_ENABLE = "enable"
	SYSTEMD_SERVICE_${PN} += "helloworld.service"
	SYSTEMD_SERVICE_${PN} += "hello_world.service"

	SRC_URI_append += " file://helloworld.service "
	SRC_URI_append += " file://hello_world.service "
	FILES_${PN} += "${systemd_unitdir}/system/helloworld.service"
	FILES_${PN} += "${systemd_unitdir}/system/hello_world.service"

	do_install_append() {
	  install -d ${D}/${systemd_unitdir}/system
	  install -m 0644 ${WORKDIR}/helloworld.service ${D}/${systemd_unitdir}/system
	  install -m 0644 ${WORKDIR}/hello_world.service ${D}/${systemd_unitdir}/system
	}
    + create server file for autoboot:
	
	create file recipes-xxx/systemd/files/helloworld.service with below content:
	
	[Unit]
	Description=GNU Hello World startup script for KOAN training course

	[Service]
	ExecStart=/usr/bin/helloworld

	[Install]
	WantedBy=multi-user.target

