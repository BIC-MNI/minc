/* concorde microPET to minc */
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#define _XOPEN_SOURCE
#include <time.h>
#include <minc.h>

/* Concorde microPET definitions */
#define UPET_FT_IMAGE 5

#define UPET_DT_UNKNOWN 0
#define UPET_DT_BYTE 1
#define UPET_DT_II16 2           /* Intel 16-bit signed integer */
#define UPET_DT_II32 3           /* Intel 32-bit signed integer */
#define UPET_DT_IF32 4           /* Intel 32-bit float */
#define UPET_DT_MF32 5           /* Sun 32-bit float */
#define UPET_DT_MI16 6           /* Sun 16-bit signed integer */
#define UPET_DT_MI32 7           /* Sun 32-bit signed integer */

#define DECLARE_FUNC(x) \
static int x(struct conversion_info *ci_ptr, char *val_str, char *new_var, char *new_att)

struct conversion_info {
    FILE *hdr_fp;
    FILE *img_fp;
    int mnc_fd;
    int frame_index;
    int data_type;
    nc_type minc_type;
    int dim_count;
    int dim_lengths[5];
    int dim_ids[5];
    double dim_steps[5];
    int frame_nbytes;
    int frame_nvoxels;
    void *frame_buffer;
    double scale_factor;
    double deadtime_correction;
    double decay_correction;
};

DECLARE_FUNC(upet_file_type);
DECLARE_FUNC(upet_acq_mode);
DECLARE_FUNC(upet_bed_motion);
DECLARE_FUNC(upet_data_type);
DECLARE_FUNC(upet_data_order);
DECLARE_FUNC(upet_ndims);
DECLARE_FUNC(upet_total_frames);
DECLARE_FUNC(upet_x_dim);
DECLARE_FUNC(upet_y_dim);
DECLARE_FUNC(upet_z_dim);
DECLARE_FUNC(upet_vector_dim);
DECLARE_FUNC(upet_injection_time);
DECLARE_FUNC(upet_scan_time);
DECLARE_FUNC(upet_axial_crystal_pitch);
DECLARE_FUNC(upet_pixel_size);
DECLARE_FUNC(upet_dose_units);

DECLARE_FUNC(upet_frame_no);
DECLARE_FUNC(upet_frame_start);
DECLARE_FUNC(upet_frame_duration);
DECLARE_FUNC(upet_frame_min);
DECLARE_FUNC(upet_frame_max);
DECLARE_FUNC(upet_frame_file_ptr);
DECLARE_FUNC(upet_frame_scale_factor);
DECLARE_FUNC(upet_frame_decay_correction);
DECLARE_FUNC(upet_frame_deadtime_correction);

static void copy_init(struct conversion_info *ci_ptr);
static void copy_frame(struct conversion_info *ci_ptr);

#define UPET_TYPE_STR 1
#define UPET_TYPE_INT 2
#define UPET_TYPE_REAL 3
#define UPET_TYPE_TIME 4
#define UPET_TYPE_FILTER 5
#define UPET_TYPE_3X64 6
#define UPET_TYPE_FPTR 7
#define UPET_TYPE_SINGLE 8

/* concorde keywords */
struct keywd_entry {
    char *upet_kwd;
    short upet_type;
    char *mnc_var;
    char *mnc_att;
    int (*func)(struct conversion_info *, char *, char *, char *);
};

/* Per-frame attributes in the concorde microPET header */
struct keywd_entry frm_atts[] = {
    { "frame", 
      UPET_TYPE_INT, NULL, NULL, upet_frame_no },
    { "event_type", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "gate", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "bed", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "bed_offset", 
      UPET_TYPE_REAL, NULL, NULL, NULL },
    { "ending_bed_offset", 
      UPET_TYPE_REAL, NULL, NULL, NULL },
    { "vertical_bed_offset", 
      UPET_TYPE_REAL, NULL, NULL, NULL },
    { "data_file_pointer", 
      UPET_TYPE_FPTR, NULL, NULL, upet_frame_file_ptr },
    { "frame_start", 
      UPET_TYPE_REAL, NULL, NULL, upet_frame_start },
    { "frame_duration", 
      UPET_TYPE_REAL, NULL, NULL, upet_frame_duration },
    { "scale_factor", 
      UPET_TYPE_REAL, NULL, NULL, upet_frame_scale_factor },
    { "minimum", 
      UPET_TYPE_REAL, NULL, NULL, upet_frame_min },
    { "maximum", 
      UPET_TYPE_REAL, NULL, NULL, upet_frame_max },
    { "deadtime_correction", 
      UPET_TYPE_REAL, NULL, NULL, upet_frame_deadtime_correction },
    { "decay_correction", 
      UPET_TYPE_REAL, NULL, NULL, upet_frame_decay_correction },
    { "prompts", 
      UPET_TYPE_3X64, NULL, NULL, NULL },
    { "delays", 
      UPET_TYPE_3X64, NULL, NULL, NULL },
    { "trues", 
      UPET_TYPE_3X64, NULL, NULL, NULL },
    { "prompts_rate", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "delays_rate", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "singles", 
      UPET_TYPE_SINGLE, NULL, NULL, NULL },
    { NULL, 0, NULL, NULL, NULL }
};

/* Per-volume attributes in the concorde microPET header */
struct keywd_entry vol_atts[] = {
    { "ROI_file:",
      UPET_TYPE_STR, NULL, NULL, NULL },
    { "version", 
      UPET_TYPE_STR, NULL, NULL, NULL },
    { "model", 
      UPET_TYPE_INT, MIstudy, MIdevice_model, NULL },
    { "institution", 
      UPET_TYPE_STR, MIstudy, MIinstitution, NULL },
    { "study",
      UPET_TYPE_STR, NULL, NULL, NULL },
    { "file_name", 
      UPET_TYPE_STR, NULL, NULL, NULL }, /* actual file data */
    { "file_type", 
      UPET_TYPE_INT, NULL, NULL, upet_file_type },
    { "acquisition_mode", 
      UPET_TYPE_INT, MIacquisition, "acquisition_mode", upet_acq_mode},
    { "bed_motion", 
      UPET_TYPE_INT, MIacquisition, "bed_motion", upet_bed_motion},
    { "total_frames", 
      UPET_TYPE_INT, NULL, NULL, upet_total_frames },
    { "isotope", 
      UPET_TYPE_STR, MIacquisition, MIradionuclide, NULL },
    { "isotope_half_life", 
      UPET_TYPE_REAL, MIacquisition, MIradionuclide_halflife, NULL },
    { "isotope_branching_fraction", 
      UPET_TYPE_REAL, MIacquisition, "isotope_branching_fraction"},
    { "transaxial_crystals_per_block", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "axial_crystals_per_block", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "intrinsic_crystal_offset", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "transaxial_blocks", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "axial_blocks", 
      UPET_TYPE_INT, NULL, NULL, NULL },
    { "transaxial_crystal_pitch", 
      UPET_TYPE_REAL, NULL, NULL, NULL },
    { "axial_crystal_pitch", 
      UPET_TYPE_REAL, NULL, NULL, upet_axial_crystal_pitch },
    { "radius", 
      UPET_TYPE_REAL, NULL, NULL, NULL },
    { "radial_fov", 
      UPET_TYPE_REAL, NULL, NULL, NULL },
    { "src_radius", 
      UPET_TYPE_REAL, NULL, NULL, NULL },
    {"src_cm_per_rev", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"src_steps_per_rev", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"tx_src_type", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"default_projections", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"default_transaxial_angles", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"crystal_thickness", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"depth_of_interaction", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"transaxial_bin_size", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"axial_plane_size", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"lld", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"uld", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"timing_window", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"data_type", 
     UPET_TYPE_INT, NULL, NULL, upet_data_type },
    {"data_order", 
     UPET_TYPE_INT, NULL, NULL, upet_data_order },
    {"span", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"ring_difference", 
     UPET_TYPE_INT,  NULL, NULL, NULL },
    {"number_of_dimensions", 
     UPET_TYPE_INT, NULL, NULL, upet_ndims },
    {"x_dimension", 
     UPET_TYPE_INT, NULL, NULL, upet_x_dim },
    {"y_dimension", 
     UPET_TYPE_INT, NULL, NULL, upet_y_dim },
    {"z_dimension", 
     UPET_TYPE_INT, NULL, NULL, upet_z_dim },
    {"w_dimension", 
     UPET_TYPE_INT, NULL, NULL, upet_vector_dim },
    {"x_filter", 
     UPET_TYPE_FILTER, NULL, NULL, NULL },
    {"y_filter", 
     UPET_TYPE_FILTER, NULL, NULL, NULL },
    {"z_filter", 
     UPET_TYPE_FILTER, NULL, NULL, NULL },
    {"histogram_version", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"rebinning_type", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"rebinning_version", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"recon_algorithm", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"recon_version", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"map_subsets", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"map_osem3d_iterations", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"map_iterations", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"map_beta", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"map_blur_type", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"map_prior_type", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"map_blur_file", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"map_pmatrix_file", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"deadtime_correction_applied", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"decay_correction_applied", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"normalization_applied", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"normalization_filename", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"attenuation_applied", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"attenuation_filename", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"scatter_correction", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"scatter_version", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"arc_correction_applied", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"x_offset", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"y_offset", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"z_offset", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"zoom", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"pixel_size", 
     UPET_TYPE_REAL, NULL, NULL, upet_pixel_size },
    {"calibration_units", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"calibration_factor", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"calibration_branching_fraction", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"number_of_singles_rates", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"investigator", 
     UPET_TYPE_STR, MIstudy, "investigator", NULL },
    {"operator", 
     UPET_TYPE_STR, MIstudy, MIoperator, NULL },
    {"study_identifier", 
     UPET_TYPE_STR, MIstudy, MIstudy_id, NULL },
    {"scan_time", 
     UPET_TYPE_TIME, NULL, NULL, upet_scan_time },
    {"injected_compound", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"dose_units", 
     UPET_TYPE_INT, MIacquisition, MIdose_units, upet_dose_units },
    {"dose", 
     UPET_TYPE_REAL, MIacquisition, MIinjection_dose, NULL },
    {"injection_time", 
     UPET_TYPE_TIME, NULL, NULL, upet_injection_time },
    {"injection_decay_correction", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"subject_identifier", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"subject_genus", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"subject_orientation", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"subject_length_units", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"subject_length", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"subject_weight_units", 
     UPET_TYPE_INT, NULL, NULL, NULL },
    {"subject_weight", 
     UPET_TYPE_REAL, NULL, NULL, NULL },
    {"subject_phenotype", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"study_model", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"anesthesia", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"analgesia", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"other_drugs", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"food_access", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {"water_access", 
     UPET_TYPE_STR, NULL, NULL, NULL },
    {NULL, 0, NULL, NULL, NULL }
};

/* Reflects "normal" image data order */
#define DIM_T 0
#define DIM_Z 1
#define DIM_Y 2
#define DIM_X 3
#define DIM_W 4


void usage()
{
    fprintf(stderr, "usage: upet2mnc infile outfile\n");
}

int
main(int argc, char **argv)
{
    char line_buf[1024];
    char *line_ptr;
    char *val_ptr;
    int i;
    int in_header;
    double dbl_tmp;
    int int_tmp;
    char img_fname[1024];
    char hdr_fname[1024];
    struct conversion_info ci;
    struct keywd_entry *ke_ptr;
    int is_known;

    if (argc < 3) {
        usage();
        return (-1);
    }

    /* Open the header and the associated binary file. */

    line_ptr = strrchr(argv[1], '.');
    if (line_ptr != NULL && !strcmp(line_ptr, ".hdr")) {
        strcpy(img_fname, argv[1]);
        strcpy(hdr_fname, argv[1]);
        line_ptr = strrchr(img_fname, '.');
        if (line_ptr != NULL) {
            *line_ptr = '\0';
        }
    }
    else if (line_ptr != NULL && !strcmp(line_ptr, ".img")) {
        strcpy(img_fname, argv[1]);
        strcpy(hdr_fname, argv[1]);
        strcat(hdr_fname, ".hdr");
    }
    else {
        strcpy(img_fname, argv[1]);
        strcpy(hdr_fname, argv[1]);
        strcat(img_fname, ".img");
        strcat(hdr_fname, ".img.hdr");
    }

    ci.hdr_fp = fopen(hdr_fname, "r");
    if (ci.hdr_fp == NULL) {
        perror(hdr_fname);
        return (-1);
    }

    ci.img_fp = fopen(img_fname, "r");
    if (ci.img_fp == NULL) {
        perror(img_fname);
        return (-1);
    }


    ci.mnc_fd = micreate(argv[2], NC_NOCLOBBER);
    if (ci.mnc_fd < 0) {
        perror(argv[2]);
        return (-1);
    }

    /* Define the basic MINC group variables.
     */
    micreate_group_variable(ci.mnc_fd, MIstudy);
    micreate_group_variable(ci.mnc_fd, MIacquisition);
    micreate_group_variable(ci.mnc_fd, MIpatient);
    /* TODO: add some comments & other information */
    ncvardef(ci.mnc_fd, "micropet", NC_SHORT, 0, NULL);
        
    in_header = 1;

    ci.frame_nbytes = 1;
    ci.frame_nvoxels = 1;

    /* Collect the headers */
    while (fgets(line_buf, sizeof(line_buf), ci.hdr_fp) != NULL) {
        if (line_buf[0] == '#') /*  */
            continue;
        line_ptr = line_buf;
        while (!isspace(*line_ptr)) {
            line_ptr++;
        }
        *line_ptr++ = '\0';
        val_ptr = line_ptr;
        while (*line_ptr != '\n' && *line_ptr != '\r' && *line_ptr != '\0') {
            line_ptr++;
        }
        *line_ptr = '\0';

        is_known = 0;

        if (in_header) {
            if (*val_ptr != '\0') {
                /* Save the raw attribute into the file */
                ncattput(ci.mnc_fd, ncvarid(ci.mnc_fd, "micropet"),
                         line_buf, NC_CHAR, strlen(val_ptr), val_ptr);
            }

            for (ke_ptr = vol_atts; ke_ptr->upet_kwd != NULL; ke_ptr++) {
                if (!strcmp(ke_ptr->upet_kwd, line_buf)) {

                    is_known = 1;

                    if (ke_ptr->func != NULL) {
                        (*ke_ptr->func)(&ci, val_ptr, 
                                        ke_ptr->mnc_var,
                                        ke_ptr->mnc_att);
                    }
                    else if (ke_ptr->mnc_var != NULL &&
                             ke_ptr->mnc_att != NULL) {

                        /* Interpret based upon type */
                        switch (ke_ptr->upet_type) {
                        case UPET_TYPE_INT:
                            int_tmp = atoi(val_ptr);
                            miattputint(ci.mnc_fd, 
                                        ncvarid(ci.mnc_fd, ke_ptr->mnc_var),
                                        ke_ptr->mnc_att,
                                        int_tmp);
                            break;

                        case UPET_TYPE_REAL:
                            dbl_tmp = atof(val_ptr);
                            miattputdbl(ci.mnc_fd, 
                                        ncvarid(ci.mnc_fd, ke_ptr->mnc_var),
                                        ke_ptr->mnc_att,
                                        dbl_tmp);
                            break;

                        case UPET_TYPE_STR:
                            miattputstr(ci.mnc_fd, 
                                        ncvarid(ci.mnc_fd, ke_ptr->mnc_var),
                                        ke_ptr->mnc_att,
                                        val_ptr);
                            break;

                        }
                        
                    }
                    break;
                }
            }
        }
        else {
            /* Not in the header any longer 
             */
            for (ke_ptr = frm_atts; ke_ptr->upet_kwd != NULL; ke_ptr++) {
                if (!strcmp(ke_ptr->upet_kwd, line_buf)) {
                    
                    is_known = 1;
                    
                    if (ke_ptr->func != NULL) {
                        (*ke_ptr->func)(&ci, val_ptr, 
                                        ke_ptr->mnc_var,
                                        ke_ptr->mnc_att);
                    }
                    break;
                }
            }
        }

        if (!is_known) {
            if (!strcmp(line_buf, "end_of_header")) {
                if (in_header) {
                    in_header = 0;

                    copy_init(&ci);

                }
                else {
                    copy_frame(&ci);
                }
            }
            else {
                printf("Unrecognized keyword %s\n", line_buf);
            }
        }
    }

    fclose(ci.hdr_fp);
    fclose(ci.img_fp);
    miclose(ci.mnc_fd);

    fprintf(stderr, "\nFinished creating %s\n", argv[2]);
}


DECLARE_FUNC(upet_file_type)
{
    if (atoi(val_str) == UPET_FT_IMAGE) { /* Image file */
        return (0);
    }
    fprintf(stderr, "WARNING: This file is not an image file.  Conversion results may be problematic...\n");
    return (1);
}

DECLARE_FUNC(upet_acq_mode)
{
    return (0);
}

DECLARE_FUNC(upet_bed_motion)
{
    return (0);
}

DECLARE_FUNC(upet_data_type)
{
    ci_ptr->data_type = atoi(val_str);
    switch (ci_ptr->data_type) {
    case UPET_DT_BYTE:
        ci_ptr->minc_type = NC_BYTE;
        break;
    case UPET_DT_II16:
        ci_ptr->minc_type = NC_SHORT;
        ci_ptr->frame_nbytes *= 2;
        break;
    case UPET_DT_II32:
        ci_ptr->minc_type = NC_INT;
        ci_ptr->frame_nbytes *= 4;
        break;
    case UPET_DT_IF32:
        ci_ptr->minc_type = NC_FLOAT;
        ci_ptr->frame_nbytes *= 4;
        break;
    case UPET_DT_MF32:
        ci_ptr->minc_type = NC_FLOAT;
        ci_ptr->frame_nbytes *= 4;
        break;
    case UPET_DT_MI16:
        ci_ptr->minc_type = NC_SHORT;
        ci_ptr->frame_nbytes *= 2;
        break;
    case UPET_DT_MI32:
        ci_ptr->minc_type = NC_INT;
        ci_ptr->frame_nbytes *= 4;
        break;
    default:
        fprintf(stderr, "ERROR: Unknown data type %d\n", ci_ptr->data_type);
        return (1);
    }
    return (0);
}

DECLARE_FUNC(upet_data_order)
{
    if (atoi(val_str) != 1) {
        fprintf(stderr, "WARNING: Unknown data order.\n");
    }
    return (0);
}

DECLARE_FUNC(upet_ndims)
{
    ci_ptr->dim_count = atoi(val_str);
    return (0);
}

DECLARE_FUNC(upet_total_frames)
{
    ci_ptr->dim_lengths[DIM_T] = atoi(val_str);
    ci_ptr->dim_ids[DIM_T] = ncdimdef(ci_ptr->mnc_fd, MItime, ci_ptr->dim_lengths[DIM_T]);
    micreate_std_variable(ci_ptr->mnc_fd, MItime, NC_DOUBLE, 
                          1, &ci_ptr->dim_ids[DIM_T]);
    micreate_std_variable(ci_ptr->mnc_fd, MItime_width, NC_DOUBLE,
                          1, &ci_ptr->dim_ids[DIM_T]);
    return (0);
}

DECLARE_FUNC(upet_x_dim)
{
    int x = atoi(val_str);
    ci_ptr->frame_nbytes *= x;
    ci_ptr->frame_nvoxels *= x;
    ci_ptr->dim_lengths[DIM_X] = x;
    ci_ptr->dim_ids[DIM_X] = ncdimdef(ci_ptr->mnc_fd, MIxspace, x);
    micreate_std_variable(ci_ptr->mnc_fd, MIxspace, NC_DOUBLE, 0, NULL);
    return (0);
}

DECLARE_FUNC(upet_y_dim)
{
    int y = atoi(val_str);
    ci_ptr->frame_nbytes *= y;
    ci_ptr->frame_nvoxels *= y;
    ci_ptr->dim_lengths[DIM_Y] = y;
    ci_ptr->dim_ids[DIM_Y] = ncdimdef(ci_ptr->mnc_fd, MIyspace, y);
    micreate_std_variable(ci_ptr->mnc_fd, MIyspace, NC_DOUBLE, 0, NULL);
    return (0);
}

DECLARE_FUNC(upet_z_dim)
{
    int z = atoi(val_str);
    ci_ptr->frame_nbytes *= z;
    ci_ptr->frame_nvoxels *= z;
    ci_ptr->dim_lengths[DIM_Z] = z;
    ci_ptr->dim_ids[DIM_Z] = ncdimdef(ci_ptr->mnc_fd, MIzspace, z);
    micreate_std_variable(ci_ptr->mnc_fd, MIzspace, NC_DOUBLE, 0, NULL);
    return (0);
}

DECLARE_FUNC(upet_vector_dim)
{
    int w = atoi(val_str);
    ci_ptr->frame_nbytes *= w;
    ci_ptr->frame_nvoxels *= w;
    ci_ptr->dim_lengths[DIM_W] = w;
    if (w > 1) {
        ci_ptr->dim_ids[DIM_W] = ncdimdef(ci_ptr->mnc_fd, MIvector_dimension, w);
    }
    else {
        ci_ptr->dim_ids[DIM_W] = -1;
    }
    return (0);
}

DECLARE_FUNC(upet_injection_time)
{
    struct tm tmbuf;
    int id;
    char str_buf[128];

    strptime(val_str, "%A %B %d %H:%M:%S %Y", &tmbuf);

    id = ncvarid(ci_ptr->mnc_fd, MIacquisition);

    sprintf(str_buf, "%02d%02d%02d", 
            tmbuf.tm_hour, tmbuf.tm_min, tmbuf.tm_sec);
    miattputstr(ci_ptr->mnc_fd, id, MIinjection_time, str_buf);

    sprintf(str_buf, "%d", tmbuf.tm_year + 1900);
    miattputstr(ci_ptr->mnc_fd, id, MIinjection_year, str_buf);

    sprintf(str_buf, "%d", tmbuf.tm_mon + 1);
    miattputstr(ci_ptr->mnc_fd, id, MIinjection_month, str_buf);

    sprintf(str_buf, "%d", tmbuf.tm_mday);
    miattputstr(ci_ptr->mnc_fd, id, MIinjection_day, str_buf);
}

DECLARE_FUNC(upet_scan_time)
{
    struct tm tmbuf;
    int id;
    char str_buf[128];

    strptime(val_str, "%A %B %d %H:%M:%S %Y", &tmbuf);

    id = ncvarid(ci_ptr->mnc_fd, MIstudy);

    sprintf(str_buf, "%02d%02d%02d", 
            tmbuf.tm_hour, tmbuf.tm_min, tmbuf.tm_sec);
    miattputstr(ci_ptr->mnc_fd, id, MIstart_time, str_buf);

    sprintf(str_buf, "%d", tmbuf.tm_year + 1900);
    miattputstr(ci_ptr->mnc_fd, id, MIstart_year, str_buf);

    sprintf(str_buf, "%d", tmbuf.tm_mon + 1);
    miattputstr(ci_ptr->mnc_fd, id, MIstart_month, str_buf);

    sprintf(str_buf, "%d", tmbuf.tm_mday);
    miattputstr(ci_ptr->mnc_fd, id, MIstart_day, str_buf);

}

DECLARE_FUNC(upet_axial_crystal_pitch)
{
    double dbl_tmp = atof(val_str);

    /* dbl_tmp is in cm.  Convert to mm. */
    dbl_tmp *= 10.0;

    /* Now convert from crystal pitch to actual slice width */
    dbl_tmp /= 2.0;

    ci_ptr->dim_steps[DIM_Z] = dbl_tmp;
}

DECLARE_FUNC(upet_pixel_size)
{
    double dbl_tmp = atof(val_str);

    /* dbl_tmp is in cm.  Convert to mm. */
    dbl_tmp *= 10.0;

    ci_ptr->dim_steps[DIM_X] = ci_ptr->dim_steps[DIM_Y] = dbl_tmp;
}

DECLARE_FUNC(upet_dose_units)
{
    int tmp = atoi(val_str);
    char *str_ptr;

    if (tmp == 0) {
        str_ptr = "unknown";
    }
    else if (tmp == 1) {
        str_ptr = "mCi";
    }
    else if (tmp == 2) {
        str_ptr = "MBq";
    }
    else {
        str_ptr = "???????";
    }
    miattputstr(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, new_var), new_att, str_ptr);
}


/***********************/
/* Per-frame functions */
DECLARE_FUNC(upet_frame_no)
{
    ci_ptr->frame_index = atoi(val_str);
}

DECLARE_FUNC(upet_frame_start)
{
    long index = ci_ptr->frame_index;
    double dbl_tmp = atof(val_str);
    mivarput1(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MItime), &index, 
              NC_DOUBLE, MI_SIGNED, &dbl_tmp);
}

DECLARE_FUNC(upet_frame_duration)
{
    long index = ci_ptr->frame_index;
    double dbl_tmp = atof(val_str);
    mivarput1(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MItime_width), &index, 
              NC_DOUBLE, MI_SIGNED, &dbl_tmp);
}

DECLARE_FUNC(upet_frame_min)
{
#if 0
    long index = ci_ptr->frame_index;
    double dbl_tmp = atof(val_str);
    mivarput1(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIimagemin), &index, 
              NC_DOUBLE, MI_SIGNED, &dbl_tmp);
#endif
}

DECLARE_FUNC(upet_frame_max)
{
#if 0
    long index = ci_ptr->frame_index;
    double dbl_tmp = atof(val_str);
    dbl_tmp *= ci_ptr->scale_factor;
    mivarput1(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIimagemax), &index, 
              NC_DOUBLE, MI_SIGNED, &dbl_tmp);
#endif
}

DECLARE_FUNC(upet_frame_file_ptr)
{
    long index = ci_ptr->frame_index;
    long hipart;
    long lopart;
    char *end_ptr;

    lopart = strtol(val_str, &end_ptr, 10);
    if (*end_ptr == ' ') {
        while (*end_ptr == ' ') {
            end_ptr++;
        }
        if (isdigit(*end_ptr)) {
            hipart = lopart;
            lopart = strtol(end_ptr, NULL, 10);
        }
    }

    /* Seek the image file to the data */

    fseek(ci_ptr->img_fp, lopart, SEEK_SET);
}

DECLARE_FUNC(upet_frame_scale_factor)
{
    ci_ptr->scale_factor = atof(val_str);
}


DECLARE_FUNC(upet_frame_deadtime_correction)
{
    ci_ptr->deadtime_correction = atof(val_str);
}

DECLARE_FUNC(upet_frame_decay_correction)
{
    ci_ptr->decay_correction = atof(val_str);
}

static void scale_data(nc_type datatype,
                       long nvox,
                       void *data,
                       double scale)
{
    int i;
    double tmp;

    switch (datatype) {
    case NC_BYTE:
        for (i = 0; i < nvox; i++) {
            tmp = (double) ((char *)data)[i];
            tmp *= scale;
            ((char *)data)[i] = tmp;
        }
        break;
    case NC_SHORT:
        for (i = 0; i < nvox; i++) {
            tmp = (double) ((short *)data)[i];
            tmp *= scale;
            ((short *)data)[i] = tmp;
        }
        break;
    case NC_INT:
        for (i = 0; i < nvox; i++) {
            tmp = (double) ((int *)data)[i];
            tmp *= scale;
            ((int *)data)[i] = tmp;
        }
        break;
    case NC_FLOAT:
        for (i = 0; i < nvox; i++) {
            tmp = (double) ((float *)data)[i];
            tmp *= scale;
            ((float *)data)[i] = tmp;
        }
        break;
    case NC_DOUBLE:
        for (i = 0; i < nvox; i++) {
            tmp = (double) ((double *)data)[i];
            tmp *= scale;
            ((double *)data)[i] = tmp;
        }
        break;
    default:
        fprintf(stderr, "Data type %d not handled\n", datatype);
        break;
    }
}

void
copy_init(struct conversion_info *ci_ptr)
{
    ci_ptr->frame_buffer = malloc(ci_ptr->frame_nbytes);
    if (ci_ptr->frame_buffer == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(-1);
    }

    /* Create the image, imagemax, and imagemin variables.
     */
    micreate_std_variable(ci_ptr->mnc_fd, MIimagemax, NC_DOUBLE,
                          1, ci_ptr->dim_ids);
    micreate_std_variable(ci_ptr->mnc_fd, MIimagemin, NC_DOUBLE,
                          1, ci_ptr->dim_ids);
    micreate_std_variable(ci_ptr->mnc_fd, MIimage, ci_ptr->minc_type,
                          ci_ptr->dim_count + 1, ci_ptr->dim_ids);

    /* Set up the dimension step and start values.  Because of the microPET
     * data orientation, we set Z and Y to be the inverse of the norm, to 
     * put the animal's nose at the top of the display.
     * TODO: allow this behavior to be controlled on the command line.
     */
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIzspace), MIstep, 
                -ci_ptr->dim_steps[DIM_Z]);
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIxspace), MIstep, 
                ci_ptr->dim_steps[DIM_X]);
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIyspace), MIstep, 
                -ci_ptr->dim_steps[DIM_Y]);

    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIzspace), MIstart, 
                ci_ptr->dim_steps[DIM_Z] * ci_ptr->dim_lengths[DIM_Z]);
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIxspace), MIstart, 
                0.0);
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIyspace), MIstart, 
                ci_ptr->dim_steps[DIM_Y] * ci_ptr->dim_lengths[DIM_Y]);

    ncendef(ci_ptr->mnc_fd);
}

static void
copy_frame(struct conversion_info *ci_ptr)
{
    long start[5];
    long count[5];

    fprintf(stderr, "Inserting frame #%d\n", ci_ptr->frame_index);

    fread(ci_ptr->frame_buffer, ci_ptr->frame_nbytes, 1, ci_ptr->img_fp);

    start[DIM_T] = ci_ptr->frame_index;
    start[DIM_X] = 0;
    start[DIM_Y] = 0;
    start[DIM_Z] = 0;
    start[DIM_W] = 0;
    
    count[DIM_T] = 1;
    count[DIM_X] = ci_ptr->dim_lengths[DIM_X];
    count[DIM_Y] = ci_ptr->dim_lengths[DIM_Y];
    count[DIM_Z] = ci_ptr->dim_lengths[DIM_Z];
    count[DIM_W] = ci_ptr->dim_lengths[DIM_W];

    scale_data(ci_ptr->minc_type, ci_ptr->frame_nvoxels, ci_ptr->frame_buffer,
               ci_ptr->scale_factor*10.0 /* fudge factor */);

    mivarput(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIimage), start, count, 
             ci_ptr->minc_type, MI_SIGNED, ci_ptr->frame_buffer);
}
