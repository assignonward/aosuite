#!/bin/bash

set -e

pushd npth
git checkout npth-1.5
popd

pushd libgpg-error
git checkout libgpg-error-1.31
popd

pushd libgcrypt
git checkout libgcrypt-1.8.2
popd

pushd libksba
git checkout libksba-1.3.5
popd

pushd libassuan
git checkout libassuan-2.5.1
popd

pushd gnupg
git checkout gnupg-2.2.8
popd

pushd gpgme
git checkout gpgme-1.11.1
popd
