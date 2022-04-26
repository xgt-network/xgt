# Automated Testing Documentation

## To Create Test Environment Container

From the root of the repository:

    docker build --rm=false \
        -t xgtitinc/ci-test-environment:latest \
        -f tests/scripts/Dockerfile.testenv .

## To Run The Tests

(Also in the root of the repository.)

    docker build --rm=false \
        -t xgtitinc/xgt-test \
        -f Dockerfile.test .

## To Troubleshoot Failing Tests

    docker run -ti \
        xgtitinc/ci-test-environment:latest \
        /bin/bash

Then, inside the container:

(These steps are taken from `/Dockerfile.test` in the
repository root.)

    git clone https://github.com/xgtit/xgt.git \
        /usr/local/src/xgt
    cd /usr/local/src/xgt
    git checkout <branch> # e.g. 123-feature
    git submodule update --init --recursive
    mkdir build
    cd build
    cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DBUILD_XGT_TESTNET=ON \
        ..
    make -j$(nproc) chain_test
    ./tests/chain_test
    cd /usr/local/src/xgt
    doxygen
    programs/build_helpers/check_reflect.py
