#include <limits.h>
#include <float.h>

#include <minc.h>

#include <volume_io.h>
#undef X                        /* These are used in nifti1_io */
#undef Y
#undef Z

#include <time_stamp.h>

#include "nifti1_io.h"

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
    fprintf(stderr, "usage: nii2mnc [] filename.nii [filename.mnc]\n");
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
    int nii_dimids[MAX_NII_DIMS];
    int nii_dir[MAX_NII_DIMS];
    int nii_map[MAX_NII_DIMS];
    unsigned long nii_lens[MAX_NII_DIMS];
    int nii_ndims;
    int nifti_file_type;

    /* MINC stuff */
    int mnc_fd;                 /* MINC file descriptor */
    nc_type mnc_mtype;          /* MINC memory data type */
    int mnc_msign;              /* MINC !0 if signed data */
    nc_type mnc_vtype;          /* MINC voxel data type */
    int mnc_vsign;            /* MINC !0 if signed data */
    int mnc_ndims;              /* MINC image dimension count */
    int mnc_dimids[MAX_VAR_DIMS]; /* MINC image dimension identifiers */
    long mnc_dlen;              /* MINC dimension length value */
    double mnc_dstep;           /* MINC dimension step value */
    int mnc_icv;                /* MINC image conversion variable */
    int mnc_vid;                /* MINC Image variable ID */
    long mnc_start[MAX_VAR_DIMS]; /* MINC data starts */
    long mnc_count[MAX_VAR_DIMS]; /* MINC data counts */
    char *mnc_hist;             /* MINC history */
    double mnc_vrange[2];       /* MINC valid min/max */
    double mnc_srange[2];       /* MINC image min/max */

    /* Other stuff */
    char out_str[1024];         /* Big string for filename */
    char att_str[1024];         /* Big string for attribute values */
    int i;                      /* Generic loop counter the first */
    int j;                      /* Generic loop counter the second */
    char *str_ptr;              /* Generic ASCIZ string pointer */
    int r;                      /* Result code. */
    int qflag = 0;              /* Quiet flag (default is non-quiet) */
    int rflag = 0;              /* Scan range flag */

    ncopts = 0;                 /* Clear global netCDF error reporting flag */

    mnc_hist = time_stamp(argc, argv);
    mnc_vtype = NC_NAT;

    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        return usage();
    }

    for (i = 1; i < argc && *(str_ptr = argv[i]) == '-'; i++) {
        for (j = 1; str_ptr[j] != '\0'; j++) {
            switch (str_ptr[1]) {
            case 'q':
                qflag++;
                break;
            case 'r':
                rflag++;
                break;
            case 'i':
                mnc_vsign = 1;
                if (str_ptr[j+1] == '1' && str_ptr[j+2] == '6') {
                    mnc_vtype = NC_SHORT;
                }
                else if (str_ptr[j+1] == '3' && str_ptr[j+2] == '2') {
                    mnc_vtype = NC_INT;
                }
                break;
            case 'u':
                mnc_vsign = 0;
                if (str_ptr[j+1] == '1' && str_ptr[j+2] == '6') {
                    mnc_vtype = NC_SHORT;
                }
                else if (str_ptr[j+1] == '3' && str_ptr[j+2] == '2') {
                    mnc_vtype = NC_INT;
                }
                break;
            case 'f':
                mnc_vsign = 1;
                if (str_ptr[j+1] == '6' && str_ptr[j+2] == '4') {
                    mnc_vtype = NC_DOUBLE;
                }
                else if (str_ptr[j+1] == '3' && str_ptr[j+2] == '2') {
                    mnc_vtype = NC_FLOAT;
                }
                break;
            default:
                fprintf(stderr, "Unrecognized option '%c'\n", str_ptr[j]);
                return usage();
            }
        }
    }

    if ((argc - i) == 1) {
        strcpy(out_str, argv[i]);
        str_ptr = strrchr(out_str, '.');
        if (str_ptr != NULL) {
            if (!strcmp(str_ptr, ".nii") || !strcmp(str_ptr, ".hdr")) {
                *str_ptr = '\0';
                strcat(out_str, ".mnc");
            }
        }
    }
    else if ((argc - i) == 2) {
        strcpy(out_str, argv[i+1]);
    }
    else {
        fprintf(stderr, "Filename argument required\n");
        return usage();
    }

    /* Read in the entire NIfTI file. */
    nii_ptr = nifti_image_read(argv[i], 1);

    if (!qflag) {
        nifti_image_infodump(nii_ptr);
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
        fprintf(stderr, "Can't create input file '%s'\n", out_str);
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
        miattputdbl(mnc_fd, r, MIstep, nii_ptr->dt);
        miattputstr(mnc_fd, r, MIunits, "s");
    }

    if (nii_ptr->nz > 1) {
        mnc_dimids[mnc_ndims] = ncdimdef(mnc_fd, MIzspace, nii_ptr->nz);
        mnc_start[mnc_ndims] = 0;
        mnc_count[mnc_ndims] = nii_ptr->nz;
        mnc_ndims++;

        r = micreate_std_variable(mnc_fd, MIzspace, NC_INT, 0, NULL);
        miattputdbl(mnc_fd, r, MIstep, nii_ptr->dz);
        miattputstr(mnc_fd, r, MIunits, "mm");
    }

    if (nii_ptr->ny > 1) {
        mnc_dimids[mnc_ndims] = ncdimdef(mnc_fd, MIyspace, nii_ptr->ny);
        mnc_start[mnc_ndims] = 0;
        mnc_count[mnc_ndims] = nii_ptr->ny;
        mnc_ndims++;

        r = micreate_std_variable(mnc_fd, MIyspace, NC_INT, 0, NULL);
        miattputdbl(mnc_fd, r, MIstep, nii_ptr->dy);
        miattputstr(mnc_fd, r, MIunits, "mm");
    }

    if (nii_ptr->nx > 1) {
        mnc_dimids[mnc_ndims] = ncdimdef(mnc_fd, MIxspace, nii_ptr->nx);
        mnc_start[mnc_ndims] = 0;
        mnc_count[mnc_ndims] = nii_ptr->nx;
        mnc_ndims++;

        r = micreate_std_variable(mnc_fd, MIxspace, NC_INT, 0, NULL);
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

    /* Create the MINC image variable.  If we can't, there is no
     * further processing possible...
     */
    mnc_vid = micreate_std_variable(mnc_fd, MIimage, mnc_vtype, mnc_ndims, 
                                    mnc_dimids);
    if (mnc_vid < 0) {
        fprintf(stderr, "Can't create the image variable\n");
        return (-1);
    }

    miattputstr(mnc_fd, mnc_vid, MIsigntype, 
                (mnc_vsign) ? MI_SIGNED : MI_UNSIGNED);

    
    if (nii_ptr->sform_code != NIFTI_XFORM_UNKNOWN) {
        int spatial_axes[MAX_NII_DIMS];
        double starts[MAX_SPACE_DIMS];
        double steps[MAX_SPACE_DIMS];
        double dircos[MAX_SPACE_DIMS][MAX_SPACE_DIMS];
        Transform transform;
        General_transform linear_transform;
        int id;

        make_identity_transform(&transform);

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                Transform_elem(transform, i, j) = nii_ptr->sto_xyz.m[i][j];
                printf("%f, ", nii_ptr->sto_xyz.m[i][j]);
            }
            printf("\n");
        }

        create_linear_transform(&linear_transform, &transform);

        /* Hrm??? */
        spatial_axes[0] = 0;
        spatial_axes[1] = 1;
        spatial_axes[2] = 2;
        
        convert_transform_to_starts_and_steps(&linear_transform,
                                              3,
                                              NULL,
                                              spatial_axes,
                                              starts,
                                              steps,
                                              dircos);

        for (i = 0; i < 3; i++) {
            printf("%f %f %f %f %f\n", starts[i], steps[i],
                   dircos[i][0],
                   dircos[i][1],
                   dircos[i][2]);
        }

        id = ncvarid(mnc_fd, MIxspace);
        miattputdbl(mnc_fd, id, MIstart, starts[0]);
        miattputdbl(mnc_fd, id, MIstep, steps[0]);
        ncattput(mnc_fd, id, MIdirection_cosines, NC_DOUBLE, 3, dircos[0]);

        id = ncvarid(mnc_fd, MIyspace);
        miattputdbl(mnc_fd, id, MIstart, starts[1]);
        miattputdbl(mnc_fd, id, MIstep, steps[1]);
        ncattput(mnc_fd, id, MIdirection_cosines, NC_DOUBLE, 3, dircos[1]);

        id = ncvarid(mnc_fd, MIzspace);
        miattputdbl(mnc_fd, id, MIstart, starts[2]);
        miattputdbl(mnc_fd, id, MIstep, steps[2]);
        ncattput(mnc_fd, id, MIdirection_cosines, NC_DOUBLE, 3, dircos[2]);
    }

    /* Find the valid minimum and maximum of the data */
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

    ncattput(mnc_fd, mnc_vid, MIvalid_range, NC_DOUBLE, 2, mnc_vrange);
    miattputstr(mnc_fd, NC_GLOBAL, MIhistory, mnc_hist);
   
    ncendef(mnc_fd);

    mivarput1(mnc_fd, ncvarid(mnc_fd, MIimagemin), mnc_start, NC_DOUBLE,
              MI_SIGNED, &mnc_srange[0]);

    mivarput1(mnc_fd, ncvarid(mnc_fd, MIimagemax), mnc_start, NC_DOUBLE,
              MI_SIGNED, &mnc_srange[1]);

    mivarput(mnc_fd, mnc_vid, mnc_start, mnc_count, mnc_mtype, 
             (mnc_msign) ? MI_SIGNED : MI_UNSIGNED, nii_ptr->data);

    miclose(mnc_fd);

    return (0);
}
