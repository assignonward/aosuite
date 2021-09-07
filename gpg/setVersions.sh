#!/bin/bash

set -e

pushd npth
git checkout npth-1.6
popd

pushd libgpg-error
git checkout libgpg-error-1.42
popd

pushd libgcrypt
git checkout libgcrypt-1.9.4
popd

pushd libksba
git checkout libksba-1.6.0
popd

pushd libassuan
git checkout libassuan-2.5.5
popd

pushd gnupg
git checkout gnupg-2.2.29
popd

pushd gpgme
git checkout gpgme-1.16.0
popd
