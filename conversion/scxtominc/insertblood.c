#include <stdio.h>
#include <stdlib.h>
#include <minc.h>

#define MIbloodroot "blood_analysis"


/* ----------------------------- MNI Header -----------------------------------
@NAME       : CreateBloodStructures
@INPUT      : mincHandle  -> a handle for an open MINC file.  This file should
                             be open for writing, but not in redefinition
			     mode.
              bloodHandle -> a handle for an open BNC file.  This file should
                             be open for reading.
@OUTPUT     : none
@RETURNS    : void
@DESCRIPTION: Copies all variable definitions (with attributes) from the BNC
              file to the MINC file.  The appropriate dimensions are also
              copied.
@METHOD     : none.  Just muddled through.
@GLOBALS    : none
@CALLS      : micopy_all_var_defs (MINC library)
              miadd_child (MINC library)
@CREATED    : May 30, 1994 by MW
@MODIFIED   : 
---------------------------------------------------------------------------- */
void CreateBloodStructures (int mincHandle, int bloodHandle)
{
    int mincRoot;
    int bloodRoot;

    /*
     * Copy all the variables with their attributes.
     */

    (void) micopy_all_var_defs (bloodHandle, mincHandle, 0, NULL);

    /*
     * Make the blood analysis root variable a child of
     * the MINC root variable.
     */

    mincRoot = ncvarid (mincHandle, MIrootvariable);
    bloodRoot = ncvarid (mincHandle, MIbloodroot);
    (void) miadd_child (mincHandle, mincRoot, bloodRoot);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : FillBloodStructures
@INPUT      : mincHandle  -> a handle for an open MINC file.  This file should
                             be open for writing, but not in redefinition
			     mode.
              bloodHandle -> a handle for an open BNC file.  This file should
                             be open for reading.
@OUTPUT     : none
@RETURNS    : void
@DESCRIPTION: Copies all variable values from the BNC file to the MINC file.
              The variable themselves should already exist in the MINC file
              (see CreateBloodStructures).
@METHOD     : none.  Just muddled through.
@GLOBALS    : none
@CALLS      : micopy_all_var_values (MINC library)
@CREATED    : May 30, 1994 by MW
@MODIFIED   : 
---------------------------------------------------------------------------- */
void FillBloodStructures (int mincHandle, int bloodHandle)
{
    (void) micopy_all_var_values (bloodHandle, mincHandle, 0, NULL);
}
