#!/bin/bash
gcc vm.c
compiled=$?
if [[ $compiled != 0 ]]; then
  echo "does not compile"
  exit 1
fi

echo -n "Testing test1.txt : "
./a.out ./testing/test1.txt <<< '3' > ./testing/output.txt
executed=$?
if [[ $executed !=  0 ]]; then
  echo "crashed"
  exit 1
fi
diff -w -B ./testing/output.txt ./testing/output1.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
  echo "incorrect output"
else
  echo "pass"
fi

echo -n "Testing test2.txt : "
./a.out ./testing/test2.txt <<< '10 7' > ./testing/output.txt
executed=$?
if [[ $executed !=  0 ]]; then
  echo "crashed"
  exit 1
fi
diff -w -B ./testing/output.txt ./testing/output2.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
  echo "incorrect output"
else
  echo "pass"
fi