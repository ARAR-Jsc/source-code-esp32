SUMMARY = "test application helloworld"
DESCRIPTION = "Custom recipe to build helloworld.c application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# add lib for dump message (use bbdebug)
inherit logging 
#inherit autotools-brokensep
#inherit cmake pkgconfig
inherit pkgconfig

#DEPENDS = "alsa-lib dlt-daemon sqlite3"
#DEPENDS = "alsa-lib sqlite3"

#EXTRA_OECMAKE += "\
#    -DUSE_DLT=ON \
#"


RDEPENDS_${PN} += "glibc gtk+ libgcc"
DEPENDS += " \
    gtk+ \
    libusb1 \
    libgcc \
"

RDEPENDS_${PN} += "libstdc++ libstdc++-dev "
DEPENDS += " \
    gcc-runtime \
"


#INSANE_SKIP_${PN} += " \
#dev-so \
#already-stripped \
#"

# Where to find source files (can be local, GitHub, etc.)
#SRC_URI = "file://src"
#SRC_URI = "file://xboard"
SRC_URI = "file://xboard/build"

# Where to keep downloaded source files (in tmp/work/...)
#S = "${WORKDIR}/src"
#S = "${WORKDIR}/xboard"
S = "${WORKDIR}/xboard/build"

# Pass arguments to linker
TARGET_CC_ARCH += "${LDFLAGS}"

#INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

#do_package_qa[noexec] = "1"

#INHIBIT_PACKAGE_STRIP = "1"
# Cross-compile source code
do_compile() {
    #${CC} -o helloworld helloworld.c
    #${CXX} -o hello_world hello_world.cpp
    make clean
    make all
}

# Create /usr/bin in rootfs and copy program to it
do_install() {
    bbwarn "warn_xinc hao  D -- bindir ${D}   ${bindir} ------ ${B}"
    install -d ${D}${bindir}
    #install -m 0755 ${B}/waterbot ${D}${bindir}
    #install -m 0755 helloworld ${D}${bindir}
}
