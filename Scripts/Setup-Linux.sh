#!/bin/bash

pushd ..
Vendor/Binaries/Premake/Linux/premake5 --cc=clang gmake2
popd
