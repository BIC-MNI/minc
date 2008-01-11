#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <float.h>
#include <minc.h>
#include <ParseArgv.h>
#include <volume_io.h>

#undef X                        /* These are used in nifti1_io */
#undef Y
#undef Z

#include <time_stamp.h>
#include "nifti1_io.h"
#include "analyze75.h"
#include "nifti1_local.h"


void test_xform(mat44 m, int i, int j, int k)
{
    double x, y, z;
    
    x = m.m[DIM_X][DIM_I] * i + m.m[DIM_X][DIM_J] * j + m.m[DIM_X][DIM_K] * k 
        + m.m[DIM_X][3];
    y = m.m[DIM_Y][DIM_I] * i + m.m[DIM_Y][DIM_J] * j + m.m[DIM_Y][DIM_K] * k 
        + m.m[DIM_Y][3];
    z = m.m[DIM_Z][DIM_I] * i + m.m[DIM_Z][DIM_J] * j + m.m[DIM_Z][DIM_K] * k 
        + m.m[DIM_Z][3];

    printf("%d %d %d => ", i, j, k);
    printf("%f %f %f\n", x, y, z);
}

static int usage(void)
{
    static const char msg[] = {
        "nii2mnc: Convert NIfTI-1 files to MINC format\n"
        "usage: nii2mnc [options] filename.nii [filename.mnc]\n"
    };
    fprintf(stderr, "%s", msg);
    return (-1);
}

static void find_data_range(int datatype,
                            long nvox,
                            void *data,
                            double range[2])
{
    int i;

    range[0] = DBL_MAX;
    range[1] = -DBL_MAX;

    for (i = 0; i < nvox; i++) {
        double tmp;

        switch (datatype) {
        case DT_INT8:
            tmp = (double) ((char *)data)[i];
            break;
        case DT_UINT8:
            tmp = (double) ((unsigned char *)data)[i];
            break;
        case DT_INT16:
            tmp = (double) ((short *)data)[i];
            break;
        case DT_UINT16:
            tmp = (double) ((unsigned short *)data)[i];
            break;
        case DT_INT32:
            tmp = (double) ((int *)data)[i];
            break;
        case DT_UINT32:
            tmp = (double) ((unsigned int *)data)[i];
            break;
        case DT_FLOAT32:
            tmp = (double) ((float *)data)[i];
            break;
        case DT_FLOAT64:
            tmp = (double) ((double *)data)[i];
            break;
        default:
            fprintf(stderr, "Data type %d not handled\n", datatype);
            break;
        }
        if (tmp < range[0]) {
            range[0] = tmp;
        }
        if (tmp > range[1]) {
            range[1] = tmp;
        }
    }
}

int
main(int argc, char **argv)
{
    /* NIFTI stuff */
    nifti_image *nii_ptr;

    /* MINC stuff */
    int mnc_fd;                 /* MINC file descriptor */
    nc_type mnc_mtype;          /* MINC memory data type */
    int mnc_msign;              /* MINC !0 if signed data */
    static nc_type mnc_vtype;   /* MINC voxel data type */
    static int mnc_vsign;       /* MINC !0 if signed data */
    int mnc_ndims;              /* MINC image dimension count */
    int mnc_dimids[MAX_VAR_DIMS]; /* MINC image dimension identifiers */
    int mnc_iid;                /* MINC Image variable ID */
    long mnc_start[MAX_VAR_DIMS]; /* MINC data starts */
    long mnc_count[MAX_VAR_DIMS]; /* MINC data counts */
    char *mnc_hist;             /* MINC history */
    double mnc_vrange[2];       /* MINC valid min/max */
    double mnc_srange[2];       /* MINC image min/max */
    double mnc_time_step;
    double mnc_time_start;
    int mnc_spatial_axes[MAX_NII_DIMS];
    double mnc_starts[MAX_SPACE_DIMS];
    double mnc_steps[MAX_SPACE_DIMS];
    double mnc_dircos[MAX_SPACE_DIMS][MAX_SPACE_DIMS];
    Transform mnc_xform;
    General_transform mnc_linear_xform;
    int mnc_vid;                /* Dimension variable id */
    struct analyze75_hdr ana_hdr;

    /* Other stuff */
    char out_str[1024];         /* Big string for filename */
    int i;                      /* Generic loop counter the first */
    int j;                      /* Generic loop counter the second */
    char *str_ptr;              /* Generic ASCIZ string pointer */
    int r;                      /* Result code. */
    static int qflag = 0;       /* Quiet flag (default is non-quiet) */
    static int rflag = 1;       /* Scan range flag */
    static int oflag = DIMORDER_ZYX;
    static int flip[MAX_SPACE_DIMS];

    static char *mnc_ordered_dim_names[MAX_SPACE_DIMS];

    static ArgvInfo argTable[] = {
        {"-byte", ARGV_CONSTANT, (char *) NC_BYTE, (char *)&mnc_vtype,
         "Write voxel data in 8-bit integer format."},
        {"-short", ARGV_CONSTANT, (char *) NC_SHORT, (char *)&mnc_vtype,
         "Write voxel data in 16-bit integer format."},
        {"-int", ARGV_CONSTANT, (char *) NC_INT, (char *)&mnc_vtype,
         "Write voxel data in 32-bit integer format."},
        {"-float", ARGV_CONSTANT, (char *) NC_FLOAT, (char *)&mnc_vtype,
         "Write voxel data in 32-bit floating point format."},
        {"-double", ARGV_CONSTANT, (char *) NC_DOUBLE, (char *)&mnc_vtype,
         "Write voxel data in 64-bit floating point format."},
        {"-signed", ARGV_CONSTANT, (char *) 1, (char *)&mnc_vsign, 
         "Write integer voxel data in signed format."},
        {"-unsigned", ARGV_CONSTANT, (char *) 0, (char *)&mnc_vsign, 
         "Write integer voxel data in unsigned format."},
        {"-noscanrange", ARGV_CONSTANT, (char *) 0, (char *)&rflag,
         "Do not scan data range."},
        {"-quiet", ARGV_CONSTANT, (char *) 0, (char *)&qflag,
         "Quiet operation"},
        {"-transverse", ARGV_CONSTANT, (char *) DIMORDER_ZYX, (char *) &oflag, 
         "Assume transverse (ZYX) ordering for spatial dimensions"},
        {"-sagittal", ARGV_CONSTANT, (char *) DIMORDER_XZY, (char *) &oflag, 
         "Assume sagittal (XZY) ordering for spatial dimensions"},
        {"-coronal", ARGV_CONSTANT, (char *) DIMORDER_YZX, (char *) &oflag, 
         "Assume coronal (YZX) ordering for spatial dimensions"},
        {"-xyz", ARGV_CONSTANT, (char *) DIMORDER_XYZ, (char *) &oflag,
         "Assume XYZ ordering for spatial dimensions"},
        {"-zxy", ARGV_CONSTANT, (char *) DIMORDER_ZXY, (char *) &oflag,
         "Assume ZXY ordering for spatial dimensions"},
        {"-yxz", ARGV_CONSTANT, (char *) DIMORDER_YXZ, (char *) &oflag,
         "Assume YXZ ordering for spatial dimensions"},
        {"-flipx", ARGV_CONSTANT, (char *) 1, (char *)&flip[DIM_X], 
         "Invert direction of X (left-right) axis"},
        {"-flipy", ARGV_CONSTANT, (char *) 1, (char *)&flip[DIM_Y], 
         "Invert direction of Y (posterior-anterior) axis"},
        {"-flipz", ARGV_CONSTANT, (char *) 1, (char *)&flip[DIM_Z], 
         "Invert direction of Z (inferior-superior) axis"},
        {NULL, ARGV_END, NULL, NULL, NULL}
    };


    ncopts = 0;                 /* Clear global netCDF error reporting flag */

    mnc_hist = time_stamp(argc, argv);
    mnc_vtype = NC_NAT;

    if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
        fprintf(stderr, "Too few arguments\n");
        return usage();
    }

    if (argc == 2) {
        strcpy(out_str, argv[1]);
        str_ptr = strrchr(out_str, '.');
        if (str_ptr != NULL) {
            if (!strcmp(str_ptr, ".nii") || !strcmp(str_ptr, ".hdr")) {
                *str_ptr = '\0';
                strcat(out_str, ".mnc");
            }
        }
    }
    else if (argc == 3) {
        strcpy(out_str, argv[2]);
    }
    else {
        fprintf(stderr, "Filename argument required\n");
        return usage();
    }

    /* Read in the entire NIfTI file. */
    nii_ptr = nifti_image_read(argv[1], 1);

    if (nii_ptr->nifti_type == 0) { /* Analyze file!!! */
        FILE *fp;
        int ss;
        int must_swap;

        fp = fopen(argv[1], "rb");
        if (fp != NULL) {
            fread(&ana_hdr, sizeof (ana_hdr), 1, fp);
            fclose(fp);

            must_swap = 0;
            ss = ana_hdr.dime.dim[0];
            if (ss != 0) {
                if (ss < 0 || ss > 7) {
                    nifti_swap_2bytes(1, &(ss));
                    if (ss < 0 || ss > 7) {
                        /* We should never get here!! */
                        fprintf(stderr, "Bad dimension count!!\n");
                    }
                    else {
                        must_swap = 1;
                    }
                }
            }
            else {
                ss = ana_hdr.hk.sizeof_hdr;
                if (ss != sizeof(ana_hdr)) {
                    nifti_swap_4bytes(1, &(ss));
                    if (ss != sizeof(ana_hdr)) {
                        /* We should never get here!! */
                        fprintf(stderr, "Bad header size!!\n");
                    }
                    else {
                        must_swap = 1;
                    }
                }
            }

            if (must_swap) {
                nifti_swap_4bytes(1, &(ana_hdr.hk.sizeof_hdr));
                nifti_swap_4bytes(1, &(ana_hdr.hk.extents));
                nifti_swap_2bytes(1, &(ana_hdr.hk.session_error));

                nifti_swap_4bytes(1, &(ana_hdr.dime.compressed));
                nifti_swap_4bytes(1, &(ana_hdr.dime.verified));
                nifti_swap_4bytes(1, &(ana_hdr.dime.glmax)); 
                nifti_swap_4bytes(1, &(ana_hdr.dime.glmin));
                nifti_swap_2bytes(8, ana_hdr.dime.dim);
                nifti_swap_4bytes(8, ana_hdr.dime.pixdim);
                nifti_swap_2bytes(1, &(ana_hdr.dime.datatype));
                nifti_swap_2bytes(1, &(ana_hdr.dime.bitpix));
                nifti_swap_4bytes(1, &(ana_hdr.dime.vox_offset));
                nifti_swap_4bytes(1, &(ana_hdr.dime.cal_max)); 
                nifti_swap_4bytes(1, &(ana_hdr.dime.cal_min));
            }

            if (!qflag) {
                printf("orient = %d\n", ana_hdr.hist.orient);
            }
        }
    }
                
    if (!qflag) {
        nifti_image_infodump(nii_ptr);
    }

    /* Set up the spatial axis correspondence for the call to 
     * convert_transform_to_starts_and_steps()
     */
    switch (oflag) {
    default:
    case DIMORDER_ZYX:
        mnc_ordered_dim_names[DIM_X] = MIxspace;
        mnc_ordered_dim_names[DIM_Y] = MIyspace;
        mnc_ordered_dim_names[DIM_Z] = MIzspace;
        mnc_spatial_axes[DIM_X] = DIM_X;
        mnc_spatial_axes[DIM_Y] = DIM_Y;
        mnc_spatial_axes[DIM_Z] = DIM_Z;
        break;
    case DIMORDER_ZXY:
        mnc_ordered_dim_names[DIM_X] = MIyspace;
        mnc_ordered_dim_names[DIM_Y] = MIxspace;
        mnc_ordered_dim_names[DIM_Z] = MIzspace;
        mnc_spatial_axes[DIM_X] = DIM_Y;
        mnc_spatial_axes[DIM_Y] = DIM_X;
        mnc_spatial_axes[DIM_Z] = DIM_Z;
        break;
    case DIMORDER_XYZ:
        mnc_ordered_dim_names[DIM_X] = MIzspace;
        mnc_ordered_dim_names[DIM_Y] = MIyspace;
        mnc_ordered_dim_names[DIM_Z] = MIxspace;
        mnc_spatial_axes[DIM_X] = DIM_Z;
        mnc_spatial_axes[DIM_Y] = DIM_Y;
        mnc_spatial_axes[DIM_Z] = DIM_X;
        break;
    case DIMORDER_XZY:
        mnc_ordered_dim_names[DIM_X] = MIyspace;
        mnc_ordered_dim_names[DIM_Y] = MIzspace;
        mnc_ordered_dim_names[DIM_Z] = MIxspace;
        mnc_spatial_axes[DIM_X] = DIM_Y;
        mnc_spatial_axes[DIM_Y] = DIM_Z;
        mnc_spatial_axes[DIM_Z] = DIM_X;
        break;
    case DIMORDER_YZX:
        mnc_ordered_dim_names[DIM_X] = MIxspace;
        mnc_ordered_dim_names[DIM_Y] = MIzspace;
        mnc_ordered_dim_names[DIM_Z] = MIyspace;
        mnc_spatial_axes[DIM_X] = DIM_X;
        mnc_spatial_axes[DIM_Y] = DIM_Z;
        mnc_spatial_axes[DIM_Z] = DIM_Y;
        break;
    case DIMORDER_YXZ:
        mnc_ordered_dim_names[DIM_X] = MIzspace;
        mnc_ordered_dim_names[DIM_Y] = MIxspace;
        mnc_ordered_dim_names[DIM_Z] = MIyspace;
        mnc_spatial_axes[DIM_X] = DIM_Z;
        mnc_spatial_axes[DIM_Y] = DIM_X;
        mnc_spatial_axes[DIM_Z] = DIM_Y;
        break;
    }

    switch (nii_ptr->datatype) {
    case DT_INT8:
        mnc_msign = 1;
        mnc_mtype = NC_BYTE;
        mnc_vrange[0] = CHAR_MIN;
        mnc_vrange[1] = CHAR_MAX;
        break;
    case DT_UINT8:
        mnc_msign = 0;
        mnc_mtype = NC_BYTE;
        mnc_vrange[0] = 0;
        mnc_vrange[1] = UCHAR_MAX;
        break;
    case DT_INT16:
        mnc_msign = 1;
        mnc_mtype = NC_SHORT;
        mnc_vrange[0] = SHRT_MIN;
        mnc_vrange[1] = SHRT_MAX;
        break;
    case DT_UINT16:
        mnc_msign = 0;
        mnc_mtype = NC_SHORT;
        mnc_vrange[0] = 0;
        mnc_vrange[1] = USHRT_MAX;
        break;
    case DT_INT32:
        mnc_msign = 1;
        mnc_mtype = NC_INT;
        mnc_vrange[0] = INT_MIN;
        mnc_vrange[1] = INT_MAX;
        break;
    case DT_UINT32:
        mnc_msign = 0;
        mnc_mtype = NC_INT;
        mnc_vrange[0] = 0;
        mnc_vrange[1] = UINT_MAX;
        break;
    case DT_FLOAT32:
        mnc_msign = 1;
        mnc_mtype = NC_FLOAT;
        mnc_vrange[0] = -FLT_MAX;
        mnc_vrange[1] = FLT_MAX;
        break;
    case DT_FLOAT64:
        mnc_msign = 1;
        mnc_mtype = NC_DOUBLE;
        mnc_vrange[0] = -DBL_MAX;
        mnc_vrange[1] = DBL_MAX;
        break;
    default:
        fprintf(stderr, "Data type %d not handled\n", nii_ptr->datatype);
        break;
    }

    if (mnc_vtype == NC_NAT) {
        mnc_vsign = mnc_msign;
        mnc_vtype = mnc_mtype;
    }

    /* Open the MINC file.  It should not already exist.
     */
    mnc_fd = micreate(out_str, NC_NOCLOBBER);
    if (mnc_fd < 0) {
        fprintf(stderr, "Can't create output file '%s'\n", out_str);
        return (-1);
    }

    /* Create the necessary dimensions in the minc file
     */

    mnc_ndims = 0;

    if (nii_ptr->nt > 1) {
        mnc_dimids[mnc_ndims] = ncdimdef(mnc_fd, MItime, nii_ptr->nt);
        mnc_start[mnc_ndims] = 0;
        mnc_count[mnc_ndims] = nii_ptr->nt;
        mnc_ndims++;

        r = micreate_std_variable(mnc_fd, MItime, NC_INT, 0, NULL);
        switch (nii_ptr->time_units) {
        case NIFTI_UNITS_UNKNOWN:
        case NIFTI_UNITS_SEC:
            mnc_time_step = nii_ptr->dt;
            mnc_time_start = nii_ptr->toffset;
            break;
        case NIFTI_UNITS_MSEC:
            mnc_time_step = nii_ptr->dt / 1000;
            mnc_time_start = nii_ptr->toffset / 1000;
            break;
        case NIFTI_UNITS_USEC:
            mnc_time_step = nii_ptr->dt / 1000000;
            mnc_time_start = nii_ptr->toffset / 1000000;
            break;
        default:
            fprintf(stderr, "Unknown time units value %d\n", 
                    nii_ptr->time_units);
            break;
        }
        miattputdbl(mnc_fd, r, MIstart, mnc_time_start);
        miattputdbl(mnc_fd, r, MIstep, mnc_time_step);
        miattputstr(mnc_fd, r, MIunits, "s");
    }

    if (nii_ptr->nz > 1) {
        mnc_dimids[mnc_ndims] = ncdimdef(mnc_fd, mnc_ordered_dim_names[DIM_Z], 
                                         nii_ptr->nz);
        mnc_start[mnc_ndims] = 0;
        mnc_count[mnc_ndims] = nii_ptr->nz;
        mnc_ndims++;

        r = micreate_std_variable(mnc_fd, mnc_ordered_dim_names[DIM_Z], NC_INT, 
                                  0, NULL);
        miattputdbl(mnc_fd, r, MIstep, nii_ptr->dz);
        miattputstr(mnc_fd, r, MIunits, "mm");
    }

    if (nii_ptr->ny > 1) {
        mnc_dimids[mnc_ndims] = ncdimdef(mnc_fd, mnc_ordered_dim_names[DIM_Y], 
                                         nii_ptr->ny);
        mnc_start[mnc_ndims] = 0;
        mnc_count[mnc_ndims] = nii_ptr->ny;
        mnc_ndims++;

        r = micreate_std_variable(mnc_fd, mnc_ordered_dim_names[DIM_Y], NC_INT, 
                                  0, NULL);
        miattputdbl(mnc_fd, r, MIstep, nii_ptr->dy);
        miattputstr(mnc_fd, r, MIunits, "mm");
    }

    if (nii_ptr->nx > 1) {
        mnc_dimids[mnc_ndims] = ncdimdef(mnc_fd, mnc_ordered_dim_names[DIM_X], 
                                         nii_ptr->nx);
        mnc_start[mnc_ndims] = 0;
        mnc_count[mnc_ndims] = nii_ptr->nx;
        mnc_ndims++;

        r = micreate_std_variable(mnc_fd, mnc_ordered_dim_names[DIM_X], NC_INT, 
                                  0, NULL);
        miattputdbl(mnc_fd, r, MIstep, nii_ptr->dx);
        miattputstr(mnc_fd, r, MIunits, "mm");
    }

    if (nii_ptr->nu > 1) {
        mnc_dimids[mnc_ndims] = ncdimdef(mnc_fd, MIvector_dimension, 
                                         nii_ptr->nu);
        mnc_start[mnc_ndims] = 0;
        mnc_count[mnc_ndims] = nii_ptr->nu;
        mnc_ndims++;
    }

    /* Create scalar image-min and image-max variables.
     */
    micreate_std_variable(mnc_fd, MIimagemax, NC_DOUBLE, 0, NULL);
    micreate_std_variable(mnc_fd, MIimagemin, NC_DOUBLE, 0, NULL);

    /* Create the group variables.
     */
    micreate_std_variable(mnc_fd, MIstudy, NC_INT, 0, NULL);
    micreate_std_variable(mnc_fd, MIpatient, NC_INT, 0, NULL);
    micreate_std_variable(mnc_fd, MIacquisition, NC_INT, 0, NULL);

    /* Create the MINC image variable.  If we can't, there is no
     * further processing possible...
     */
    mnc_iid = micreate_std_variable(mnc_fd, MIimage, mnc_vtype, mnc_ndims, 
                                    mnc_dimids);
    if (mnc_iid < 0) {
        fprintf(stderr, "Can't create the image variable\n");
        return (-1);
    }

    miattputstr(mnc_fd, mnc_iid, MIsigntype, 
                (mnc_vsign) ? MI_SIGNED : MI_UNSIGNED);


    /* Calculate the starts, steps, and direction cosines. This only 
     * be done properly if the file is NIfTI-1 file.  If it is an Analyze
     * file we have to resort to other methods...
     */

    if (nii_ptr->nifti_type != 0 &&
        (nii_ptr->sform_code != NIFTI_XFORM_UNKNOWN ||
         nii_ptr->qform_code != NIFTI_XFORM_UNKNOWN)) {

         make_identity_transform(&mnc_xform);

        if (nii_ptr->sform_code != NIFTI_XFORM_UNKNOWN) {
            if (!qflag) {
                printf("Using s-form transform:\n");
            }
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    Transform_elem(mnc_xform, i, j) = nii_ptr->sto_xyz.m[i][j];
                    if (!qflag) {
                        printf("%8.4f, ", nii_ptr->sto_xyz.m[i][j]);
                    }
                }
                if (!qflag) {
                    printf("\n");
                }
            }
        }
        else {
            if (!qflag) {
                printf("Using q-form transform:\n");
            }
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    Transform_elem(mnc_xform, i, j) = nii_ptr->qto_xyz.m[i][j];
                    if (!qflag) {
                        printf("%8.4f, ", nii_ptr->qto_xyz.m[i][j]);
                    }
                }
                if (!qflag) {
                    printf("\n");
                }
            }
        }

        create_linear_transform(&mnc_linear_xform, &mnc_xform);

        convert_transform_to_starts_and_steps(&mnc_linear_xform,
                                              MAX_SPACE_DIMS,
                                              NULL,
                                              mnc_spatial_axes,
                                              mnc_starts,
                                              mnc_steps,
                                              mnc_dircos);

    }
    else {
        /* No official transform was found (possibly this is an Analyze 
         * file).  Just use some reasonable defaults.
         */
        mnc_steps[mnc_spatial_axes[DIM_X]] = nii_ptr->dx;
        mnc_steps[mnc_spatial_axes[DIM_Y]] = nii_ptr->dy;
        mnc_steps[mnc_spatial_axes[DIM_Z]] = nii_ptr->dz;
        mnc_starts[mnc_spatial_axes[DIM_X]] = -(nii_ptr->dx * nii_ptr->nx) / 2;
        mnc_starts[mnc_spatial_axes[DIM_Y]] = -(nii_ptr->dy * nii_ptr->ny) / 2;
        mnc_starts[mnc_spatial_axes[DIM_Z]] = -(nii_ptr->dz * nii_ptr->nz) / 2;

        /* Unlike the starts and steps, the direction cosines do NOT change
         * based upon the data orientation.
         */
        for (i = 0; i < MAX_SPACE_DIMS; i++) {
            for (j = 0; j < MAX_SPACE_DIMS; j++) {
                mnc_dircos[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }
    }

    switch (nii_ptr->xyz_units) {
    case NIFTI_UNITS_METER:
        for (i = 0; i < MAX_SPACE_DIMS; i++) {
            mnc_starts[i] *= 1000;
            mnc_steps[i] *= 1000;
        }
        break;
    case NIFTI_UNITS_MM:
        break;
    case NIFTI_UNITS_MICRON:
        for (i = 0; i < MAX_SPACE_DIMS; i++) {
            mnc_starts[i] /= 1000;
            mnc_steps[i] /= 1000;
        }
        break;
    default:
        fprintf(stderr, "Unknown XYZ units %d\n", nii_ptr->xyz_units);
        break;
    }

    /* Now we write the spatial axis information to the file.  The starts,
     * steps, and cosines have to be associated with the correct spatial
     * axes.  Also, we perform any orientation flipping that was requested.
     */
    for (i = 0; i < MAX_SPACE_DIMS; i++) {
        if (!qflag) {
            printf("%s start: %8.4f step: %8.4f cosines: %8.4f %8.4f %8.4f\n", 
                   mnc_spatial_names[i],
                   mnc_starts[i],
                   mnc_steps[i],
                   mnc_dircos[i][DIM_X],
                   mnc_dircos[i][DIM_Y],
                   mnc_dircos[i][DIM_Z]);
        }
        mnc_vid = ncvarid(mnc_fd, mnc_spatial_names[i]);

        /* If we selected "flipping" of the appropriate axis, do it here
         */
        if (flip[i]) {
            miattputdbl(mnc_fd, mnc_vid, MIstart, 
                        mnc_starts[i]+((mnc_count[i]-1)*mnc_steps[i]));
            miattputdbl(mnc_fd, mnc_vid, MIstep, -mnc_steps[i]);
        }
        else {
            miattputdbl(mnc_fd, mnc_vid, MIstart, mnc_starts[i]);
            miattputdbl(mnc_fd, mnc_vid, MIstep, mnc_steps[i]);
        }
        ncattput(mnc_fd, mnc_vid, MIdirection_cosines, NC_DOUBLE, 
                 MAX_SPACE_DIMS, mnc_dircos[i]);
    }

    /* Find the valid minimum and maximum of the data, in order to set the
     * global image minimum and image maximum properly.
     */
    if (rflag) {
        find_data_range(nii_ptr->datatype, 
                        nii_ptr->nvox,
                        nii_ptr->data,
                        mnc_vrange);
    }

    if (nii_ptr->scl_slope != 0.0) {
        /* Convert slope/offset to min/max 
         */
        mnc_srange[0] = (mnc_vrange[0] * nii_ptr->scl_slope) + nii_ptr->scl_inter;
        mnc_srange[1] = (mnc_vrange[1] * nii_ptr->scl_slope) + nii_ptr->scl_inter;
    }
    else {
        mnc_srange[0] = mnc_vrange[0];
        mnc_srange[1] = mnc_vrange[1];
    }

    ncattput(mnc_fd, mnc_iid, MIvalid_range, NC_DOUBLE, 2, mnc_vrange);
    miattputstr(mnc_fd, NC_GLOBAL, MIhistory, mnc_hist);

    /* Switch out of definition mode.
     */
    ncendef(mnc_fd);

    /* Finally, write the values of the image-min, image-max, and image
     * variables.
     */
    mivarput1(mnc_fd, ncvarid(mnc_fd, MIimagemin), mnc_start, NC_DOUBLE,
              MI_SIGNED, &mnc_srange[0]);

    mivarput1(mnc_fd, ncvarid(mnc_fd, MIimagemax), mnc_start, NC_DOUBLE,
              MI_SIGNED, &mnc_srange[1]);

    mivarput(mnc_fd, mnc_iid, mnc_start, mnc_count, mnc_mtype, 
             (mnc_msign) ? MI_SIGNED : MI_UNSIGNED, nii_ptr->data);

    miclose(mnc_fd);

    return (0);
}
