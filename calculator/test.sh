#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "0;"
assert 42 "42;"

assert 21 "5+20-4;"

assert 41 " 12 + 34 - 5 ;"

assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 1 '(-3+5)/2;'

assert 1 '1<2;'
assert 0 '2<2;'
assert 1 '2>1;'
assert 0 '2>2;'
assert 1 '2<=3;'
assert 1 '2<=2;'
assert 0 '2<=1;'
assert 1 '3>=2;'
assert 1 '2>=2;'
assert 0 '1>=2;'

assert 1 'a=1;'
assert 2 'a=b=2;'
assert 3 '1; a=3;'
assert 5 'a=5;a;'

assert 1 'ab=1;'
assert 2 'ab = 1; ab = ab + 1;'
assert 1 'abcdefghijklmnopqrstuvwxyz_ZBCDEFGHIJKLMNOPQRSTUVWXYZ1234567890=1;'

assert 1 'return 1;'
assert 2 'return 2; 1;'

assert 2 'if(0) return 3; 2;'
assert 3 'if(1) return 3; 2;'
assert 1 'a=0; if(1) a=1; a;'
assert 2 'if(0) return 3; else 2;'
assert 21 'if(0) a=10; else a=20; if(1) a = a + 1; else a = a - 1; return a;'

echo OK