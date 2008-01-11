/* concorde microPET to minc */
#include "config.h"

#define _XOPEN_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
#include <time.h>
#include <string.h>
#include <minc.h>
#include <time_stamp.h>
#include <ParseArgv.h>

#define VERSIONSTR VERSION " built " __DATE__ " " __TIME__

/*************************************************************************
 * Concorde microPET definitions 
 */
/* file_type field */
#define UPET_FT_UNKNOWN 0
#define UPET_FT_LIST_MODE 1
#define UPET_FT_SINOGRAM 2
#define UPET_FT_NORMALIZATION 3
#define UPET_FT_ATTENUATION_CORRECTION 4
#define UPET_FT_IMAGE 5         /* Standard image data file (typical) */
#define UPET_FT_BLANK 6
#define UPET_FT_MU_MAP 8        /* Mu map data file */
#define UPET_FT_SCATTER_CORRECTION 9

/* acquisition_mode field */
#define UPET_AM_UNKNOWN 0
#define UPET_AM_BLANK 1
#define UPET_AM_EMISSION 2
#define UPET_AM_DYNAMIC 3
#define UPET_AM_GATED 4
#define UPET_AM_CONTINUOUS_BED_MOTION 5
#define UPET_AM_SINGLES_TRANSMISSION 6
#define UPET_AM_WINDOWED_COINCIDENCE_TRANSMISSION 7
#define UPET_AM_NONWINDOWED_COINCIDENCE_TRANSMISSION 8

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
    int frame_zero;
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
    double calibration_factor;
    double isotope_branching_fraction;
    int swap_size;              /* 0, 2, 4 */
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
DECLARE_FUNC(upet_calibration_factor);
DECLARE_FUNC(upet_rotation);
DECLARE_FUNC(upet_isotope_branching_fraction);

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

/* These values are used to represent the field types in the microPET
 * header file.
 */
#define UPET_TYPE_STR 1         /* String */
#define UPET_TYPE_INT 2         /* Integer */
#define UPET_TYPE_REAL 3        /* Floating-point */
#define UPET_TYPE_TIME 4        /* Timestamp */
#define UPET_TYPE_FILTER 5      /* Integer type followed by a float cutoff */
#define UPET_TYPE_3X64 6        /* 3 64-bit integers */
#define UPET_TYPE_FPTR 7        /* File pointer (2 32 bit integers) */
#define UPET_TYPE_SINGLE 8      /* Block #, singles/sec, raw singles/sec */

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
      UPET_TYPE_INT, NULL, NULL, upet_acq_mode},
    { "bed_motion", 
      UPET_TYPE_INT, NULL, NULL, upet_bed_motion},
    { "total_frames", 
      UPET_TYPE_INT, NULL, NULL, upet_total_frames },
    { "isotope", 
      UPET_TYPE_STR, MIacquisition, MIradionuclide, NULL },
    { "isotope_half_life", 
      UPET_TYPE_REAL, MIacquisition, MIradionuclide_halflife, NULL },
    { "isotope_branching_fraction", 
      UPET_TYPE_REAL, NULL, NULL, upet_isotope_branching_fraction },
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
    {"rotation",
     UPET_TYPE_REAL, NULL, NULL, upet_rotation },
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
     UPET_TYPE_REAL, NULL, NULL, upet_calibration_factor },
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

static char *_dimnames[5];

/* Calculate the overall scaling factor for the image data from the
 * conversion information structure.
 */
#define COMBINED_SCALE_FACTOR(ci_ptr) \
     ((ci_ptr->scale_factor * ci_ptr->calibration_factor) / \
      (ci_ptr->isotope_branching_fraction))


#define ORIENT_BODY 1
#define ORIENT_HEAD 2

int _orient_flag = ORIENT_HEAD;
int _verbose_flag = 1;

ArgvInfo argTable[] = {
    {"-head", ARGV_CONSTANT, (char *) ORIENT_HEAD, (char *) &_orient_flag,
     "Orient image for cerebral viewing (as with human brain)"},
    {"-body", ARGV_CONSTANT, (char *) ORIENT_BODY, (char *) &_orient_flag,
     "Orient image for whole-body viewing (Z along long axis)"},
    {"-quiet", ARGV_CONSTANT, (char *) 0, (char *) &_verbose_flag,
     "Turn off the various progress reporting messages."},
     {NULL, ARGV_VERINFO, (char *) VERSIONSTR, (char *) NULL, NULL},
     {NULL, ARGV_END, NULL, NULL, NULL}
};

typedef enum {
    MSG_INFO,
    MSG_WARNING,
    MSG_ERROR,
    MSG_FATAL
} msg_level_t;

static void 
message(msg_level_t level, char *fmt, ...)
{
    va_list ap;
    const char *prefix_str;

    switch (level) {
    case MSG_WARNING:
        prefix_str = "WARNING: ";
        break;
    case MSG_ERROR:
        prefix_str = "ERROR: ";
        break;
    case MSG_FATAL:
        prefix_str = "FATAL: ";
        break;
    default:
        prefix_str = NULL;
        break;
    }
    va_start(ap, fmt);
    if (_verbose_flag || level != MSG_INFO) {
        if (level != MSG_INFO) {
            if (prefix_str != NULL) {
                fprintf(stderr, prefix_str);
            }
            vfprintf(stderr, fmt, ap);
        }
        if (prefix_str != NULL) {
            fprintf(stdout, prefix_str);
        }
        vfprintf(stdout, fmt, ap);
    }
    va_end(ap);
}

static int
is_host_big_endian()
{
    long ltmp = 0x04030201;
    char *ctmp = (char *) &ltmp;

    if (ctmp[0] == 0x01) {
        return (0);
    }
    if (ctmp[0] == 0x04) {
        return (1);
    }
    return (-1);
}

static void
usage(const char *progname)
{
   fprintf(stderr, "\nUsage: %s [<options>] input.img[.hdr] [output.mnc]\n", 
           progname);
   fprintf(stderr,   "       %s [-help]\n\n", progname);
   exit(-1);
}

int
upet_to_minc(char *hdr_fname, char *img_fname, char *out_fname, 
             char *prog_name)
{
    char *line_ptr;
    char line_buf[1024];
    char *val_ptr;
    int in_header;
    double dbl_tmp;
    int int_tmp;
    struct conversion_info ci;
    struct keywd_entry *ke_ptr;
    int is_known;
    char *argv_tmp[5];
    char *out_history;

    ci.hdr_fp = fopen(hdr_fname, "r"); /* Text file */
    if (ci.hdr_fp == NULL) {
        perror(hdr_fname);
        return (-1);
    }

    ci.img_fp = fopen(img_fname, "rb"); /* Binary file */
    if (ci.img_fp == NULL) {
        perror(img_fname);
        return (-1);
    }

    ci.mnc_fd = micreate(out_fname, NC_NOCLOBBER);
    if (ci.mnc_fd < 0) {
        perror(out_fname);
        return (-1);
    }

    ci.frame_zero = -1;     /* Initial frame is -1 until set. */

    /* Define the basic MINC group variables.
     */
    micreate_group_variable(ci.mnc_fd, MIstudy);
    micreate_group_variable(ci.mnc_fd, MIacquisition);
    micreate_group_variable(ci.mnc_fd, MIpatient);
    ncvardef(ci.mnc_fd, "micropet", NC_SHORT, 0, NULL);

    /* Fake the history here */
    argv_tmp[0] = prog_name;
    argv_tmp[1] = VERSIONSTR;
    argv_tmp[2] = hdr_fname;
    argv_tmp[3] = img_fname;
    argv_tmp[4] = out_fname;

    out_history = time_stamp(5, argv_tmp);

    miattputstr(ci.mnc_fd, NC_GLOBAL, MIhistory, out_history);
    free(out_history);
        
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
                message(MSG_WARNING, "Unrecognized keyword %s\n", line_buf);
            }
        }
    }

    fclose(ci.hdr_fp);
    fclose(ci.img_fp);
    miclose(ci.mnc_fd);
    return (0);
}

int
main(int argc, char **argv)
{
    char *line_ptr;
    int i;
    char img_fname[1024];
    char hdr_fname[1024];
    char out_fname[1024];
    int result;

    if (ParseArgv(&argc, argv, argTable, 0) || argc < 2) {
        usage(argv[0]);
        return (-1);
    }

    ncopts = 0;

    /* Set the dimension names.  This is done here since the correct 
     * arrangement depends on the value of _orient_flag 
     */
    _dimnames[DIM_T] = MItime;
    _dimnames[DIM_X] = MIxspace;
    _dimnames[DIM_Y] = MIyspace;
    _dimnames[DIM_Z] = MIzspace;
    _dimnames[DIM_W] = MIvector_dimension;

    if (_orient_flag == ORIENT_HEAD) {
        /* If using head orientation, exchange Y and Z.
         */
        _dimnames[DIM_Y] = MIzspace;
        _dimnames[DIM_Z] = MIyspace;
    }


    /* Open the header and the associated binary file. */

    for (i = 1; i < argc; i++) {
        /* Here we try to be flexible about allowing the user to specify
         * either the name of the .hdr file or the name of the .img file,
         * or just the base name of the two files.  All three options
         * should work.
         */
        strcpy(img_fname, argv[i]);
        strcpy(hdr_fname, argv[i]);

        /* Find the last extension.
         */
        line_ptr = strrchr(argv[i], '.');

        /* Did the user specify the .hdr file??
         */
        if (line_ptr != NULL && !strcmp(line_ptr, ".hdr")) {
            line_ptr = strrchr(img_fname, '.');
            if (line_ptr != NULL) {
                *line_ptr = '\0';
            }
        }
        /* Did the user specify the .img file??
         */
        else if (line_ptr != NULL && !strcmp(line_ptr, ".img")) {
            strcat(hdr_fname, ".hdr");
        }
        /* Or perhaps just the base name??
         */
        else {
            strcat(img_fname, ".img");
            strcat(hdr_fname, ".img.hdr");
        }

        /* See if there is a filename following this one, and if so, does it
         * end with the ".mnc" extension.  If so, take that names as the 
         * output for this conversions.
         */
        if (i < argc - 1 && 
            (line_ptr = strrchr(argv[i+1], '.')) != NULL &&
            !strcmp(line_ptr, ".mnc")) {

            strcpy(out_fname, argv[i+1]);
            i++;
        }
        else {
            strcpy(out_fname, img_fname);
            line_ptr = strrchr(out_fname, '.');
            if (line_ptr != NULL) {
                strcpy(line_ptr, ".mnc");
            }
        }

        /* Perform the conversion.
         */

        message(MSG_INFO, "Starting conversion\n");
        message(MSG_INFO, "- Input header: %s\n", hdr_fname);
        message(MSG_INFO, "- Input image:  %s\n", img_fname);
        message(MSG_INFO, "- Output file:  %s\n", out_fname);
               
        result = upet_to_minc(hdr_fname, img_fname, out_fname, argv[0]);
        if (result < 0) {
            message(MSG_ERROR, "Error creating %s\n", out_fname);
        }
        else {
            message(MSG_INFO, "Finished creating %s\n", out_fname);
        }
    }
}


DECLARE_FUNC(upet_file_type)
{
    int file_type = atoi(val_str);
    switch (file_type) {
    case UPET_FT_IMAGE:         /* Image file */
    case UPET_FT_MU_MAP:        /* Mu map file */
        return (0);
    default:
        message(MSG_WARNING, 
                "File type %d is not handled.  Conversion results may be problematic...\n", file_type);
        break;
    }
    return (1);
}

DECLARE_FUNC(upet_acq_mode)
{
    int mode_int = atoi(val_str);
    char *mode_str;

    switch (mode_int) {
    case UPET_AM_UNKNOWN:
        mode_str = "unknown";
        break;
    case UPET_AM_BLANK:
        mode_str = "blank";
        break;
    case UPET_AM_EMISSION:
        mode_str = "emission";
        break;
    case UPET_AM_DYNAMIC:
        mode_str = "dynamic";
        break;
    case UPET_AM_GATED:
        mode_str = "gated";
        break;
    case UPET_AM_CONTINUOUS_BED_MOTION:
        mode_str = "continuous_bed_motion";
        break;
    case UPET_AM_SINGLES_TRANSMISSION:
        mode_str = "singles_transmission";
        break;
    case UPET_AM_WINDOWED_COINCIDENCE_TRANSMISSION:
        mode_str = "windowed_coincidence_transmission";
        break;
    case UPET_AM_NONWINDOWED_COINCIDENCE_TRANSMISSION:
        mode_str = "non-windowed_coincidence_transmission";
        break;
    default:
        message(MSG_WARNING, "Unknown acquisition mode %d\n", mode_int);
        mode_str = NULL;
        break;
    }
    if (mode_str != NULL) {
        miattputstr(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIacquisition),
                    "micropet_mode", mode_str);
    }
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
        if (is_host_big_endian()) {
            ci_ptr->swap_size = 2;
        }
        else {
            ci_ptr->swap_size = 0;
        }
        break;
    case UPET_DT_II32:
        ci_ptr->minc_type = NC_INT;
        ci_ptr->frame_nbytes *= 4;
        if (is_host_big_endian()) {
            ci_ptr->swap_size = 4;
        }
        else {
            ci_ptr->swap_size = 0;
        }
        break;
    case UPET_DT_IF32:
        ci_ptr->minc_type = NC_FLOAT;
        ci_ptr->frame_nbytes *= 4;
        if (is_host_big_endian()) {
            ci_ptr->swap_size = 4;
        }
        else {
            ci_ptr->swap_size = 0;
        }
        break;
    case UPET_DT_MF32:
        ci_ptr->minc_type = NC_FLOAT;
        ci_ptr->frame_nbytes *= 4;
        if (!is_host_big_endian()) {
            ci_ptr->swap_size = 4;
        }
        else {
            ci_ptr->swap_size = 0;
        }
        break;
    case UPET_DT_MI16:
        ci_ptr->minc_type = NC_SHORT;
        ci_ptr->frame_nbytes *= 2;
        if (!is_host_big_endian()) {
            ci_ptr->swap_size = 2;
        }
        else {
            ci_ptr->swap_size = 0;
        }
        break;
    case UPET_DT_MI32:
        ci_ptr->minc_type = NC_INT;
        ci_ptr->frame_nbytes *= 4;
        if (!is_host_big_endian()) {
            ci_ptr->swap_size = 4;
        }
        else {
            ci_ptr->swap_size = 0;
        }
        break;
    default:
        message(MSG_ERROR, "Unknown data type %d\n", ci_ptr->data_type);
        return (1);
    }
    if (ci_ptr->swap_size != 0) {
        message(MSG_INFO, "Swapping groups of %d bytes.\n", ci_ptr->swap_size);
    }
    else {
        message(MSG_INFO, "No byte-swapping required.\n");
    }
    return (0);
}

DECLARE_FUNC(upet_data_order)
{
    if (atoi(val_str) != 1) {
        message(MSG_WARNING, "Unknown data order.\n");
    }
    return (0);
}

DECLARE_FUNC(upet_ndims)
{
    ci_ptr->dim_count = atoi(val_str);
    return (0);
}

static void
create_dimension(struct conversion_info *ci_ptr, int index, int length)
{
    ci_ptr->dim_lengths[index] = length;
    if (index == DIM_W && length <= 1) {
        return;
    }

    ci_ptr->dim_ids[index] = ncdimdef(ci_ptr->mnc_fd, 
                                      _dimnames[index], 
                                      length);
    if (index == DIM_T) {
        micreate_std_variable(ci_ptr->mnc_fd, _dimnames[index], 
                              NC_DOUBLE, 1, &ci_ptr->dim_ids[index]);
        micreate_std_variable(ci_ptr->mnc_fd, MItime_width, 
                              NC_DOUBLE, 1, &ci_ptr->dim_ids[index]);
    }
    else if (index != DIM_W) {
        micreate_std_variable(ci_ptr->mnc_fd, _dimnames[index], 
                              NC_DOUBLE, 0, NULL);
    }
}

DECLARE_FUNC(upet_total_frames)
{
    create_dimension(ci_ptr, DIM_T, atoi(val_str));
    return (0);
}

DECLARE_FUNC(upet_x_dim)
{
    int x = atoi(val_str);
    ci_ptr->frame_nbytes *= x;
    ci_ptr->frame_nvoxels *= x;
    create_dimension(ci_ptr, DIM_X, x);
    return (0);
}

DECLARE_FUNC(upet_y_dim)
{
    int y = atoi(val_str);
    ci_ptr->frame_nbytes *= y;
    ci_ptr->frame_nvoxels *= y;
    create_dimension(ci_ptr, DIM_Y, y);
    return (0);
}

DECLARE_FUNC(upet_z_dim)
{
    int z = atoi(val_str);
    ci_ptr->frame_nbytes *= z;
    ci_ptr->frame_nvoxels *= z;
    create_dimension(ci_ptr, DIM_Z, z);
    return (0);
}

DECLARE_FUNC(upet_vector_dim)
{
    int w = atoi(val_str);
    ci_ptr->frame_nbytes *= w;
    ci_ptr->frame_nvoxels *= w;
    create_dimension(ci_ptr, DIM_W, w);
    return (0);
}

/* Parse a micropet time string of the form: Ddd Mmm NN HH:MM:SS YYYY
 * e.g. Fri Jan 7 14:16:31 2005
 */
static int
parse_time(char *str_ptr, struct tm *tm_ptr)
{
    /* Just skip the first three characters. */
    while (*str_ptr != '\0' && *str_ptr != ' ') {
        str_ptr++;
    }

    while (*str_ptr == ' ') {
        str_ptr++;
    }

    /* Decode the month */
    if (str_ptr[0] == 'A') {
        if (str_ptr[1] == 'p') {
            tm_ptr->tm_mon = 4 - 1; /* April */
        }
        else {
            tm_ptr->tm_mon = 8 - 1; /* August */
        }
    }
    else if (str_ptr[0] == 'D') {
        tm_ptr->tm_mon = 12 - 1; /* December */
    }
    else if (str_ptr[0] == 'F') { /* February */
        tm_ptr->tm_mon = 2 - 1;
    }
    else if (str_ptr[0] == 'J') {
        if (str_ptr[1] == 'a') {
            tm_ptr->tm_mon = 1 - 1; /* January */
        }
        else if (str_ptr[2] == 'l') {
            tm_ptr->tm_mon = 7 - 1; /* July */
        }
        else {
            tm_ptr->tm_mon = 6 - 1; /* June */
        }
    }
    else if (str_ptr[0] == 'M') {
        if (str_ptr[2] == 'r') {
            tm_ptr->tm_mon = 3 - 1; /* March */
        }
        else {
            tm_ptr->tm_mon = 5 - 1; /* May */
        }
    }
    else if (str_ptr[0] == 'N') {
        tm_ptr->tm_mon = 11 - 1; /* November */
    }
    else if (str_ptr[0] == 'O') {
        tm_ptr->tm_mon = 10 - 1; /* October */
    }
    else if (str_ptr[0] == 'S') {
        tm_ptr->tm_mon = 9 - 1; /* September */
    }
    else {
        return 0;
    }

    /* Skip past the month */
    while (*str_ptr != ' ' && *str_ptr != '\0') {
        str_ptr++;
    }

    while (*str_ptr == ' ') {
        str_ptr++;
    }

    tm_ptr->tm_mday = 0;
    while (isdigit(*str_ptr)) {
        tm_ptr->tm_mday = (tm_ptr->tm_mday * 10) + (*str_ptr++ - '0');
    }

    while (*str_ptr == ' ') {
        str_ptr++;
    }

    tm_ptr->tm_hour = 0;
    while (isdigit(*str_ptr)) {
        tm_ptr->tm_hour = (tm_ptr->tm_hour * 10) + (*str_ptr++ - '0');
    }

    if (*str_ptr == ':') {
        str_ptr++;
    }
    else {
        return 0;
    }

    tm_ptr->tm_min = 0;
    while (isdigit(*str_ptr)) {
        tm_ptr->tm_min = (tm_ptr->tm_min * 10) + (*str_ptr++ - '0');
    }

    if (*str_ptr == ':') {
        str_ptr++;
    }
    else {
        return 0;
    }

    tm_ptr->tm_sec = 0;
    while (isdigit(*str_ptr)) {
        tm_ptr->tm_sec = (tm_ptr->tm_sec * 10) + (*str_ptr++ - '0');
    }

    while (*str_ptr == ' ') {
        str_ptr++;
    }

    tm_ptr->tm_year = 0;
    while (isdigit(*str_ptr)) {
        tm_ptr->tm_year = (tm_ptr->tm_year * 10) + (*str_ptr++ - '0');
    }
    tm_ptr->tm_year -= 1900;

    return 1;
}

DECLARE_FUNC(upet_injection_time)
{
    struct tm tmbuf;
    int id;
    char str_buf[128];

    id = ncvarid(ci_ptr->mnc_fd, MIacquisition);

    if (!parse_time(val_str, &tmbuf)) {
        strcpy(str_buf, "unknown");
        miattputstr(ci_ptr->mnc_fd, id, MIinjection_time, str_buf);
        miattputstr(ci_ptr->mnc_fd, id, MIinjection_year, str_buf);
        miattputstr(ci_ptr->mnc_fd, id, MIinjection_month, str_buf);
        miattputstr(ci_ptr->mnc_fd, id, MIinjection_day, str_buf);
    }
    else {
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
    return (0);
}

DECLARE_FUNC(upet_scan_time)
{
    struct tm tmbuf;
    int id;
    char str_buf[128];

    id = ncvarid(ci_ptr->mnc_fd, MIstudy);

    if (!parse_time(val_str, &tmbuf)) {
        strcpy(str_buf, "unknown");
        miattputstr(ci_ptr->mnc_fd, id, MIstart_time, str_buf);
        miattputstr(ci_ptr->mnc_fd, id, MIstart_year, str_buf);
        miattputstr(ci_ptr->mnc_fd, id, MIstart_month, str_buf);
        miattputstr(ci_ptr->mnc_fd, id, MIstart_day, str_buf);
    }
    else {
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
    return (0);
}

DECLARE_FUNC(upet_axial_crystal_pitch)
{
    double dbl_tmp = atof(val_str);

    /* dbl_tmp is in cm.  Convert to mm. */
    dbl_tmp *= 10.0;

    /* Now convert from crystal pitch to actual slice width */
    dbl_tmp /= 2.0;

    ci_ptr->dim_steps[DIM_Z] = dbl_tmp;
    return (0);
}

DECLARE_FUNC(upet_pixel_size)
{
    double dbl_tmp = atof(val_str);

    /* dbl_tmp is in cm.  Convert to mm. */
    dbl_tmp *= 10.0;

    ci_ptr->dim_steps[DIM_X] = dbl_tmp;
    ci_ptr->dim_steps[DIM_Y] = dbl_tmp;
    return (0);
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
        message(MSG_WARNING, "Unrecognized dose_units value %d\n", tmp);
    }
    miattputstr(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, new_var), new_att, str_ptr);
    return (0);
}

DECLARE_FUNC(upet_calibration_factor)
{
    double dbl_tmp = atof(val_str);

    ci_ptr->calibration_factor = dbl_tmp;
    return (0);
}

DECLARE_FUNC(upet_isotope_branching_fraction)
{
    double dbl_tmp = atof(val_str);

    ci_ptr->isotope_branching_fraction = dbl_tmp;
    return (0);
}


DECLARE_FUNC(upet_rotation)
{
    double dbl_tmp = atof(val_str);
    if (dbl_tmp != 0.0) {
        message(MSG_WARNING, "Rotation is %f\n", dbl_tmp);
    }
    return (0);
}

/***********************/
/* Per-frame functions */
DECLARE_FUNC(upet_frame_no)
{
    ci_ptr->frame_index = atoi(val_str);
    /* Set index of "zeroth" frame if not already set.
     */
    if (ci_ptr->frame_zero < 0) {
        ci_ptr->frame_zero = ci_ptr->frame_index;
    }
    return (0);
}

DECLARE_FUNC(upet_frame_start)
{
    long index = ci_ptr->frame_index - ci_ptr->frame_zero;
    double dbl_tmp = atof(val_str);
    mivarput1(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MItime), &index, 
              NC_DOUBLE, MI_SIGNED, &dbl_tmp);
    return (0);
}

DECLARE_FUNC(upet_frame_duration)
{
    long index = ci_ptr->frame_index - ci_ptr->frame_zero;
    double dbl_tmp = atof(val_str);
    mivarput1(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MItime_width), &index, 
              NC_DOUBLE, MI_SIGNED, &dbl_tmp);
    return (0);
}

DECLARE_FUNC(upet_frame_min)
{
    long index = ci_ptr->frame_index - ci_ptr->frame_zero;
    double dbl_tmp = atof(val_str);
    dbl_tmp *= COMBINED_SCALE_FACTOR(ci_ptr);
    mivarput1(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIimagemin), &index, 
              NC_DOUBLE, MI_SIGNED, &dbl_tmp);
    return (0);
}

DECLARE_FUNC(upet_frame_max)
{
    long index = ci_ptr->frame_index - ci_ptr->frame_zero;
    double dbl_tmp = atof(val_str);
    dbl_tmp *= COMBINED_SCALE_FACTOR(ci_ptr);
    mivarput1(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIimagemax), &index, 
              NC_DOUBLE, MI_SIGNED, &dbl_tmp);
    return (0);
}

DECLARE_FUNC(upet_frame_file_ptr)
{
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
    return (0);
}

DECLARE_FUNC(upet_frame_scale_factor)
{
    ci_ptr->scale_factor = atof(val_str);
    return (0);
}


DECLARE_FUNC(upet_frame_deadtime_correction)
{
    ci_ptr->deadtime_correction = atof(val_str);
    return (0);
}

DECLARE_FUNC(upet_frame_decay_correction)
{
    ci_ptr->decay_correction = atof(val_str);
    return (0);
}

static void 
swap_data(int swap_size,
          long nvox,
          unsigned char *data)
{
  unsigned char tmp;

  if (swap_size == 2) {
    while (nvox--) {
      tmp = data[0];
      data[0] = data[1];
      data[1] = tmp;
      data += 2;
    }
  }
  else if (swap_size == 4) {
    while (nvox--) {
      tmp = data[0];
      data[0] = data[3];
      data[3] = tmp;
      tmp = data[1];
      data[1] = data[2];
      data[2] = tmp;
      data += 4;
    }
  }
}

static void 
scale_data(nc_type datatype,
           long nvox,
           void *data,
           double scale)
{
    long i;
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
        message(MSG_ERROR, "Data type %d not handled\n", datatype);
        break;
    }
}

void
copy_init(struct conversion_info *ci_ptr)
{
    ci_ptr->frame_buffer = malloc(ci_ptr->frame_nbytes);
    if (ci_ptr->frame_buffer == NULL) {
        message(MSG_FATAL, "Out of memory\n");
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
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_Z]), 
                MIstep, -ci_ptr->dim_steps[DIM_Z]);
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_X]), 
                MIstep, ci_ptr->dim_steps[DIM_X]);
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_Y]), 
                MIstep, -ci_ptr->dim_steps[DIM_Y]);

    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_Z]), 
                MIstart, ci_ptr->dim_steps[DIM_Z] * ci_ptr->dim_lengths[DIM_Z]);
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_X]), 
                MIstart, 0.0);
    miattputdbl(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_Y]), 
                MIstart, ci_ptr->dim_steps[DIM_Y] * ci_ptr->dim_lengths[DIM_Y]);

    miattputstr(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_Z]), 
                MIunits, "mm");
    miattputstr(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_X]), 
                MIunits, "mm");
    miattputstr(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_Y]), 
                MIunits, "mm");
    miattputstr(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, _dimnames[DIM_T]), 
                MIunits, "s");

    ncendef(ci_ptr->mnc_fd);
}

static void
copy_frame(struct conversion_info *ci_ptr)
{
    long start[5];
    long count[5];
    size_t nitems;

    message(MSG_INFO, "Inserting frame #%d\n", ci_ptr->frame_index);

    /* Actually read the data from the image file.
     */
    nitems = fread(ci_ptr->frame_buffer, ci_ptr->frame_nbytes, 1, 
                   ci_ptr->img_fp);

    if (nitems != 1) {
        message(MSG_FATAL, "Read failed with error %d, return %d\n",
                errno, nitems);
        exit(-1);
    }

    /* Setup the starts and counts for the data block.
     */
    start[DIM_T] = ci_ptr->frame_index - ci_ptr->frame_zero;
    start[DIM_X] = 0;
    start[DIM_Y] = 0;
    start[DIM_Z] = 0;
    start[DIM_W] = 0;
    
    count[DIM_T] = 1;
    count[DIM_X] = ci_ptr->dim_lengths[DIM_X];
    count[DIM_Y] = ci_ptr->dim_lengths[DIM_Y];
    count[DIM_Z] = ci_ptr->dim_lengths[DIM_Z];
    count[DIM_W] = ci_ptr->dim_lengths[DIM_W];

    /* Perform swapping if necessary.
     */
    if (ci_ptr->swap_size != 0) {
      swap_data(ci_ptr->swap_size, ci_ptr->frame_nvoxels, 
                ci_ptr->frame_buffer);
    }

    /* Scale the raw data into the final range.
     */
    scale_data(ci_ptr->minc_type, ci_ptr->frame_nvoxels, ci_ptr->frame_buffer,
               COMBINED_SCALE_FACTOR(ci_ptr));

    /* For now we perform no conversions on the data as it is stored.
     * This may be worth modifying in the future, to allow storage of
     * non-floating-point formats from a typical microPET file.
     */
    ncvarput(ci_ptr->mnc_fd, ncvarid(ci_ptr->mnc_fd, MIimage), start, count, 
             ci_ptr->frame_buffer);
}

