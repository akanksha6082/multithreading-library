#!/bin/bash
current_dir=`pwd`

#execuing test file for one-one
cd $current_dir"/one-one"
./test.sh

#executing test file for many-one
cd $current_dir"/many-one"
./test.sh

