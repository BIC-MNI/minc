$! --------------------------------------------------------------------------
$! For making proglib.olb on vms
$! --------------------------------------------------------------------------
$!
$! $Id: Make.com,v 1.2 1994-09-28 10:34:22 neelin Exp $
$
$ ccc := cc /opt/nodebug/nolist/include=[]
$
$ ccc parseargv.c
$ ccc time_stamp.c
$ ccc vax_conversions.c
$ ccc tag_io.c
$
$ library/create mincprog.olb
$ library/replace mincprog parseargv, time_stamp, vax_conversions, tag_io
