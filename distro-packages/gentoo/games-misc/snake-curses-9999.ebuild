# Copyright 2020 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=7

DESCRIPTION="Snake game written in C with Ncurses"
HOMEPAGE="https://gitlab.com/libre_hackerman/snake_curses"

LICENSE="GPL-3"
SLOT="0"
IUSE=""

DEPEND="sys-libs/ncurses"
RDEPEND="${DEPEND}"
BDEPEND=""


if [[ ${PV} == *9999 ]] ; then
	EGIT_REPO_URI="https://gitlab.com/libre_hackerman/snake_curses.git"
	inherit git-r3
else
	SRC_URI="https://gitlab.com/libre_hackerman/snake_curses/-/archive/v${PV}/snake_curses-v${PV}.tar.gz"
	KEYWORDS="~amd64 ~x86"
	S="${WORKDIR}/snake_curses-v${PV}"
fi

src_install() {
	mkdir -p "${ED}/usr/bin"
	emake PREFIX="${ED}/usr" install
}
