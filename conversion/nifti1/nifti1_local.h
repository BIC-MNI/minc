/* Additional things we define for the NIfTI-1 format.  Some of these
 * should probably be included in the header files.
 */

/* NIfTI-1 (Analyze 7.5) files can have at most 8 dimensions. This should
 * be defined in nifti1.h
 */
#define MAX_NII_DIMS 8

/* Length of NIfTI-1 description field. Should be defined in nifti1.h
 */
#define MAX_NII_DESCRIP 80

/* File types.  These should be part of the nifti1_io.h header.
 */
#define FT_UNSPECIFIED (-1)
#define FT_ANALYZE 0
#define FT_NIFTI_SINGLE 1
#define FT_NIFTI_DUAL 2
#define FT_NIFTI_ASCII 3

/******
 * Private stuff we use for mapping NIfTI-1 dimensions onto MINC dimensions.
 **/
/* # spatial dimensions */
#define MAX_SPACE_DIMS 3
/* World coordinates */
#define DIM_X 0
#define DIM_Y 1
#define DIM_Z 2
/* Voxel coordinates */
#define DIM_I 0
#define DIM_J 1
#define DIM_K 2

#define DIMORDER_ZYX 0
#define DIMORDER_YZX 1
#define DIMORDER_XZY 2
#define DIMORDER_XYZ 3
#define DIMORDER_ZXY 4
#define DIMORDER_YXZ 5

/* This list is in the order in which dimension lengths and sample
 * widths are stored in the NIfTI-1 structure.
 */
static const char *dimnames[MAX_NII_DIMS] = {
    MIvector_dimension,
    MItime,
    MIzspace,
    MIyspace,
    MIxspace,
    NULL,
    NULL,
    NULL
};

/* Map dimension index from the actual mapping of the data array to the 
 * "internal header array order".
 *
 * In other words, NIfTI-1 seems to store the lengths of dimensions in this
 * order: X, Y, Z, T, V in the dim[8] entry.
 * But data is actually stored with the vector dimension varying _slowest_,
 * with the X dimension varying _fastest_, i.e. as if it were a  C array
 * declared array[V][T][Z][Y][X];
 */

static const int dimmap[MAX_NII_DIMS] = {
    4,
    3,
    2,
    1,
    0,
    -1,
    -1,
    -1
};

/* Names of MINC spatial dimensions, in our "standard" world ordering.
 */
static const char *mnc_spatial_names[MAX_SPACE_DIMS] = {
    MIxspace,
    MIyspace,
    MIzspace
};
