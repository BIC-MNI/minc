
/* Header file for scanditronix file definition */
#ifndef SCX_FILE_HEADER_DEFINITION_H
#define SCX_FILE_HEADER_DEFINITION_H

/* Mnemonic types */
typedef enum scx_mnem_types_enum 
   {scx_time, scx_byte, scx_string, scx_short_float, scx_date, scx_float, scx_word, scx_long} scx_mnem_types;
static int scx_mnem_type_size[] =
   {1, 1, 1, 2, 1, 4, 2, 4};

/* Block type */
typedef struct scx_block_struct scx_block_type;
struct scx_block_struct {
   int length;
   int multiplicity;
   scx_block_type *parent;
};

/* Mnemonic type */
typedef struct scx_mnemonic_struct scx_mnemonic_type;
struct scx_mnemonic_struct {
   char *name;
   scx_mnem_types type;
   int length;
   int in_file;
   long mdefault;
   long start;
   scx_block_type *block;
};


/* Blocks for drs file with id pc_5_r_1 */
static scx_block_type drs_block_pc_5_r_1[] = {
   1872, 1, NULL,
   4, 8, &(drs_block_pc_5_r_1[0]),
   4, 8, &(drs_block_pc_5_r_1[0]),
   18, 5, &(drs_block_pc_5_r_1[0]),
   18, 1, &(drs_block_pc_5_r_1[0]),
   87, 15, &(drs_block_pc_5_r_1[0]),
   2, 32, &(drs_block_pc_5_r_1[5]),
   4, 15, &(drs_block_pc_5_r_1[0]),
   0, 0, NULL
};


/* Mnemonics for drs file with id pc_5_r_1 */
static scx_mnemonic_type drs_mnems_pc_5_r_1[] = {
   "FHS", scx_byte, 1, 0, 4, -1, &(drs_block_pc_5_r_1[0]),
   "REC", scx_word, 1, 0, 512, -1, &(drs_block_pc_5_r_1[0]),
   "TYP", scx_word, 1, 1, 5, 0, &(drs_block_pc_5_r_1[0]),
   "NCS", scx_byte, 1, 1, 15, 2, &(drs_block_pc_5_r_1[0]),
   "SITE", scx_string, 4, 1, 0, 3, &(drs_block_pc_5_r_1[0]),
   "REV", scx_word, 1, 1, 1, 7, &(drs_block_pc_5_r_1[0]),
   "NDET", scx_word, 1, 1, 256, 9, &(drs_block_pc_5_r_1[0]),
   "NPRO", scx_word, 1, 1, 256, 11, &(drs_block_pc_5_r_1[0]),
   "NMEM", scx_word, 1, 1, 48, 13, &(drs_block_pc_5_r_1[0]),
   "NWOB", scx_byte, 1, 1, 5, 15, &(drs_block_pc_5_r_1[0]),
   "CDI", scx_short_float, 1, 1, 0, 16, &(drs_block_pc_5_r_1[0]),
   "CSZ", scx_short_float, 1, 1, 0, 18, &(drs_block_pc_5_r_1[0]),
   "CSX", scx_short_float, 1, 1, 0, 20, &(drs_block_pc_5_r_1[0]),
   "RID", scx_string, 10, 1, 0, 22, &(drs_block_pc_5_r_1[0]),
   "RIN", scx_long, 1, 1, 0, 32, &(drs_block_pc_5_r_1[0]),
   "ETN", scx_word, 1, 1, 0, 36, &(drs_block_pc_5_r_1[0]),
   "PNM", scx_string, 24, 1, 0, 38, &(drs_block_pc_5_r_1[0]),
   "BRN", scx_long, 1, 1, 0, 62, &(drs_block_pc_5_r_1[0]),
   "ASS", scx_string, 22, 1, 0, 66, &(drs_block_pc_5_r_1[0]),
   "DOC", scx_string, 22, 1, 0, 88, &(drs_block_pc_5_r_1[0]),
   "DAT", scx_date, 3, 1, 0, 110, &(drs_block_pc_5_r_1[0]),
   "DATY", scx_byte, 1, 1, 0, 110, &(drs_block_pc_5_r_1[0]),
   "DATM", scx_byte, 1, 1, 0, 111, &(drs_block_pc_5_r_1[0]),
   "DATD", scx_byte, 1, 1, 0, 112, &(drs_block_pc_5_r_1[0]),
   "TIM", scx_time, 4, 1, 0, 113, &(drs_block_pc_5_r_1[0]),
   "TIMH", scx_byte, 1, 1, 0, 113, &(drs_block_pc_5_r_1[0]),
   "TIMM", scx_byte, 1, 1, 0, 114, &(drs_block_pc_5_r_1[0]),
   "TIMS", scx_byte, 1, 1, 0, 115, &(drs_block_pc_5_r_1[0]),
   "TIHU", scx_byte, 1, 1, 0, 116, &(drs_block_pc_5_r_1[0]),
   "QES", scx_string, 12, 1, 0, 117, &(drs_block_pc_5_r_1[0]),
   "POR", scx_string, 4, 1, 0, 129, &(drs_block_pc_5_r_1[0]),
   "PAN", scx_word, 1, 1, 0, 133, &(drs_block_pc_5_r_1[0]),
   "CLV", scx_short_float, 1, 1, 0, 135, &(drs_block_pc_5_r_1[0]),
   "ZDI", scx_short_float, 1, 1, 0, 137, &(drs_block_pc_5_r_1[0]),
   "NRV", scx_word, 1, 1, 0, 139, &(drs_block_pc_5_r_1[0]),
   "NME", scx_word, 1, 1, 0, 141, &(drs_block_pc_5_r_1[0]),
   "MTM", scx_float, 1, 1, 0, 143, &(drs_block_pc_5_r_1[0]),
   "RPRT", scx_short_float, 1, 1, 0, 147, &(drs_block_pc_5_r_1[0]),
   "ISO", scx_string, 6, 1, 0, 149, &(drs_block_pc_5_r_1[0]),
   "HALF", scx_float, 1, 1, 0, 155, &(drs_block_pc_5_r_1[0]),
   "CAR", scx_string, 8, 1, 0, 159, &(drs_block_pc_5_r_1[0]),
   "ACT", scx_float, 1, 1, 0, 167, &(drs_block_pc_5_r_1[0]),
   "INP", scx_string, 4, 1, 0, 171, &(drs_block_pc_5_r_1[0]),
   "INT", scx_string, 4, 1, 0, 175, &(drs_block_pc_5_r_1[0]),
   "ITM", scx_time, 3, 1, 0, 179, &(drs_block_pc_5_r_1[0]),
   "ITMH", scx_byte, 1, 1, 0, 179, &(drs_block_pc_5_r_1[0]),
   "ITMM", scx_byte, 1, 1, 0, 180, &(drs_block_pc_5_r_1[0]),
   "ITMS", scx_byte, 1, 1, 0, 181, &(drs_block_pc_5_r_1[0]),
   "COL", scx_string, 1, 1, 0, 182, &(drs_block_pc_5_r_1[0]),
   "SAMP", scx_byte, 1, 1, 0, 183, &(drs_block_pc_5_r_1[0]),
   "DTYP", scx_byte, 1, 1, 0, 184, &(drs_block_pc_5_r_1[0]),
   "FOV", scx_byte, 1, 1, 0, 185, &(drs_block_pc_5_r_1[0]),
   "PRJL", scx_word, 1, 1, 0, 186, &(drs_block_pc_5_r_1[0]),
   "TILT", scx_short_float, 1, 1, 0, 188, &(drs_block_pc_5_r_1[0]),
   "SLEW", scx_short_float, 1, 1, 0, 190, &(drs_block_pc_5_r_1[0]),
   "ELV", scx_word, 1, 1, 0, 192, &(drs_block_pc_5_r_1[0]),
   "TIW", scx_short_float, 1, 1, 0, 194, &(drs_block_pc_5_r_1[0]),
   "COM", scx_string, 20, 1, 0, 196, &(drs_block_pc_5_r_1[0]),
   "STYP", scx_byte, 1, 1, 0, 216, &(drs_block_pc_5_r_1[0]),
   "BLAN", scx_long, 1, 1, 0, 217, &(drs_block_pc_5_r_1[0]),
   "TRAN", scx_long, 1, 1, 0, 221, &(drs_block_pc_5_r_1[0]),
   "PAR", scx_byte, 1, 1, 0, 225, &(drs_block_pc_5_r_1[0]),
   "NORM", scx_word, 1, 1, 0, 226, &(drs_block_pc_5_r_1[0]),
   "DPR", scx_byte, 1, 1, 0, 228, &(drs_block_pc_5_r_1[0]),
   "TNM", scx_string, 8, 1, 0, 229, &(drs_block_pc_5_r_1[0]),
   "FERR", scx_long, 1, 1, 0, 237, &(drs_block_pc_5_r_1[0]),
   "FLOS", scx_float, 1, 1, 0, 241, &(drs_block_pc_5_r_1[0]),
   "INVA", scx_float, 1, 1, 0, 245, &(drs_block_pc_5_r_1[0]),
   "TRAT", scx_float, 1, 1, 0, 249, &(drs_block_pc_5_r_1[0]),
   "TAC", scx_word, 1, 1, 0, 253, &(drs_block_pc_5_r_1[0]),
   "IMFM", scx_word, 1, 1, 0, 255, &(drs_block_pc_5_r_1[0]),
   "IMTP", scx_string, 10, 1, 0, 257, &(drs_block_pc_5_r_1[0]),
   "IMUN", scx_string, 10, 1, 0, 267, &(drs_block_pc_5_r_1[0]),
   "PXS", scx_short_float, 1, 1, 0, 277, &(drs_block_pc_5_r_1[0]),
   "FFN", scx_string, 4, 1, 0, 279, &(drs_block_pc_5_r_1[0]),
   "FWD", scx_short_float, 1, 1, 0, 283, &(drs_block_pc_5_r_1[0]),
   "SLOT", scx_word, 1, 1, 0, 285, &(drs_block_pc_5_r_1[0]),
   "CNTX", scx_string, 10, 1, 0, 287, &(drs_block_pc_5_r_1[0]),
   "AGE", scx_byte, 1, 1, 0, 297, &(drs_block_pc_5_r_1[0]),
   "SEX", scx_string, 1, 1, 0, 298, &(drs_block_pc_5_r_1[0]),
   "MAX", scx_word, 1, 1, 0, 299, &(drs_block_pc_5_r_1[0]),
   "SING", scx_float, 1, 1, 0, 335, &(drs_block_pc_5_r_1[1]),
   "XRAT", scx_float, 1, 1, 0, 367, &(drs_block_pc_5_r_1[2]),
   "USR", scx_float, 1, 1, 0, 399, &(drs_block_pc_5_r_1[3]),
   "USI", scx_long, 1, 1, 0, 403, &(drs_block_pc_5_r_1[3]),
   "USC", scx_string, 10, 1, 0, 407, &(drs_block_pc_5_r_1[3]),
   "CYCL", scx_float, 1, 1, 0, 489, &(drs_block_pc_5_r_1[4]),
   "CYCH", scx_float, 1, 1, 0, 493, &(drs_block_pc_5_r_1[4]),
   "GTLL", scx_short_float, 1, 1, 0, 497, &(drs_block_pc_5_r_1[4]),
   "GTLH", scx_short_float, 1, 1, 0, 499, &(drs_block_pc_5_r_1[4]),
   "GTNR", scx_byte, 1, 1, 0, 501, &(drs_block_pc_5_r_1[4]),
   "FRNR", scx_word, 1, 1, 0, 502, &(drs_block_pc_5_r_1[4]),
   "DCY", scx_short_float, 1, 1, 0, 504, &(drs_block_pc_5_r_1[4]),
   "NGAT", scx_byte, 1, 1, 0, 506, &(drs_block_pc_5_r_1[4]),
   "STOT", scx_float, 1, 1, 0, 507, &(drs_block_pc_5_r_1[5]),
   "CAL", scx_float, 1, 1, 0, 511, &(drs_block_pc_5_r_1[5]),
   "MAG", scx_float, 1, 1, 0, 515, &(drs_block_pc_5_r_1[5]),
   "SPOS", scx_byte, 1, 1, 0, 519, &(drs_block_pc_5_r_1[5]),
   "SCA", scx_float, 1, 1, 0, 520, &(drs_block_pc_5_r_1[5]),
   "RAN", scx_float, 1, 1, 0, 524, &(drs_block_pc_5_r_1[5]),
   "DTC", scx_short_float, 1, 1, 0, 528, &(drs_block_pc_5_r_1[5]),
   "PERX", scx_byte, 1, 1, 0, 530, &(drs_block_pc_5_r_1[6]),
   "PERY", scx_byte, 1, 1, 0, 531, &(drs_block_pc_5_r_1[6]),
   "OFFS", scx_float, 1, 1, 0, 1812, &(drs_block_pc_5_r_1[7]),
   NULL, scx_byte, 0, 0, 0, NULL
};


/* List of drs files and types */

static struct {
   int file_type;
   scx_block_type *block_list;
   scx_mnemonic_type *mnemonic_list;
} scx_file_types[] = {
   5, drs_block_pc_5_r_1, drs_mnems_pc_5_r_1,
   0, NULL, NULL
};

#endif  /* SCX_FILE_HEADER_DEFINITION_H */
