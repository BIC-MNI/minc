$! --------------------------------------------------------------------------
$! For making proglib.olb on vms
$! --------------------------------------------------------------------------
$!
$! $Id: Make.com,v 3.0 1995-05-15 19:31:35 neelin Rel $
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
