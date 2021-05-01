#!/bin/bash
echo ""
echo "==============="
echo -e "\033[0;31mMANY-ONE TESTING\033[0m"
echo "==============="

echo ""
echo ""

echo -e "\033[0;31mInterface/API Testing\033[0m"
echo ""
echo ""
./bin/api_test

echo ""
echo ""

echo -e "\033[0;31mSpinlock Synchronization Testing\033[0m"
echo ""
echo ""
./bin/spin_test 5

echo ""
echo ""

echo -e "\033[0;31mMatrix Multiplication Test\033[0m"

echo ""
echo ""

./bin/matrix_test data1.txt 5

echo ""
echo ""

