#!/bin/bash

make clean

rm -f kylin-scanner.pro.user \
    .qmake.stash \
    Makefile \
    src/Makefile \
    src/kylin-scanner \
    debian/debhelper-build-stamp \
    debian/files \
    debian/kylin-scanner.substvars \
    debian/README.Debian \
    debian/kylin-scanner.debhelper.log \
    translations/kylin-scanner_bo_CN.*

rm -rf debian/.debhelper/ \
    debian/kylin-scanner \
    ./kylin-scanner \
    ./kylin-scanner.pro.user* \
    ./*.qm
