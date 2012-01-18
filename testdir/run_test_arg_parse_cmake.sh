#! /bin/sh

#set -e

CMD=$1
fail=no

check() { #args expected
    out=`$CMD $1`
    leftovers=`echo $out | sed "/^$2\$/d"`
    test -z "$leftovers" && return
    fail=yes
}


check ''                   'const_a:0 const_b:0 int_a:0 int_b:0 long_a:0 long_b:0'
check '-const_a'           'const_a:1 const_b:0 int_a:0 int_b:0 long_a:0 long_b:0'
check '-const_b'           'const_a:0 const_b:1 int_a:0 int_b:0 long_a:0 long_b:0'
check '-const_a -const_b'  'const_a:1 const_b:1 int_a:0 int_b:0 long_a:0 long_b:0'
check '-const_b -const_a'  'const_a:1 const_b:1 int_a:0 int_b:0 long_a:0 long_b:0'

check '-int_a 33'          'const_a:0 const_b:0 int_a:33 int_b:0 long_a:0 long_b:0'
check '-int_a -3'          'const_a:0 const_b:0 int_a:-3 int_b:0 long_a:0 long_b:0'
check '-int_b 22'          'const_a:0 const_b:0 int_a:0 int_b:22 long_a:0 long_b:0'
check '-int_b -2'          'const_a:0 const_b:0 int_a:0 int_b:-2 long_a:0 long_b:0'
check '-int_a -1 -int_b 3' 'const_a:0 const_b:0 int_a:-1 int_b:3 long_a:0 long_b:0'
check '-int_b -1 -int_a 3' 'const_a:0 const_b:0 int_a:3 int_b:-1 long_a:0 long_b:0'

check '-long_a 12'            'const_a:0 const_b:0 int_a:0 int_b:0 long_a:12 long_b:0'
check '-long_a -99'           'const_a:0 const_b:0 int_a:0 int_b:0 long_a:-99 long_b:0'
check '-long_b -12'           'const_a:0 const_b:0 int_a:0 int_b:0 long_a:0 long_b:-12'
check '-long_b 99'            'const_a:0 const_b:0 int_a:0 int_b:0 long_a:0 long_b:99'
check '-long_a 3 -long_b -9'  'const_a:0 const_b:0 int_a:0 int_b:0 long_a:3 long_b:-9'
check '-long_b 3 -long_a -9'  'const_a:0 const_b:0 int_a:0 int_b:0 long_a:-9 long_b:3'

check '-long_a -99 -int_b 3 -const_b'  'const_a:0 const_b:1 int_a:0 int_b:3 long_a:-99 long_b:0'

check '-nonsense'          'const_a:0 const_b:0 int_a:0 int_b:0 long_a:0 long_b:0'

test $fail = yes && exit 1
exit 0

