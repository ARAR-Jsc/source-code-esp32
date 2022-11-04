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
