$! --------------------------------------------------------------------------
$! For making proglib.olb on vms
$! --------------------------------------------------------------------------
$!
$! $Id: Make.com,v 6.0 1997-09-12 13:23:41 neelin Rel $
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
