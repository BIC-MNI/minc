$! To compile minc programs for vms
$!
$! Get arguments
$ if (p1.eqs."")
$ then
$    write sys$output "Usage: @make objects
$    exit
$ endif
$ objects = p1
$!
$ ccc := cc /opt/nodebug/nolist -
   /include=([],[-.proglib],[--.src],[---.netcdf.include])
$
$ on error then goto exit
$ on control_y then goto exit
$ set ver
$ ccc 'objects'
$ link/nodebug 'objects', -
      [-.proglib]mincprog/lib, -
      [--.src]minc/lib, -
      [---.netcdf.lib]netcdf/lib, -
      sys$input/opt
         sys$library:vaxcrtl.exe/share
$exit:
$ set nover
