#! /bin/sh

#set -e

fail=no

check() { #args expected
    out=`./test_arg_parse $1`
    echo $out | grep "$2" >/dev/null && return
    echo "Args    : $1"
    echo "Output  : $out"
    echo "Expected: $2"
    fail=yes
}


check ''                   'const_a:0 const_b:0'
check '-const_a'           'const_a:1 const_b:0'
check '-const_b'           'const_a:0 const_b:1'
check '-const_a -const_b'  'const_a:1 const_b:1'
check '-const_b -const_a'  'const_a:1 const_b:1'

test $fail = yes && exit 1
exit 0

