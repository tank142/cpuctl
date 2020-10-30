pkgname=cpuctl
pkgver=0
pkgrel=1
arch=('any')
license=('GPL3')
depends=('libconfig' 'glibc')
source=($pkgname::git+https://github.com/tank142/cpuctl.git
"cpuctl.conf" "cpuctl.service")
sha256sums=('SKIP' 'SKIP' 'SKIP')
build() {
	mkdir -p "${srcdir}"/cpuctl/build
	cd "${srcdir}"/cpuctl/build
	qmake ..
	make
}
package() {
	install -Dm755 "${srcdir}"/cpuctl.conf "${pkgdir}"/etc/cpuctl.conf
	install -Dm755 "${srcdir}"/cpuctl.service "${pkgdir}"/usr/lib/systemd/system/cpuctl.service
	install -d "${pkgdir}"/usr/bin/
	install -m755 "${srcdir}"/cpuctl/build/cpuctl "${pkgdir}"/usr/bin/
}
