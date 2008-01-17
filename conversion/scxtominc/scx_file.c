/* ----------------------------- MNI Header -----------------------------------
@NAME       : scx_file.c
@DESCRIPTION: File containing routines to read scanditronix image files
@GLOBALS    : 
@CREATED    : January 8, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: scx_file.c,v $
 * Revision 6.3  2008-01-17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.2  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.1  1999/10/29 17:52:07  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:23:31  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:32  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:13  neelin
 * Release of minc version 0.4
 *
 * Revision 3.0  1995/05/15  19:31:05  neelin
 * Release of minc version 0.3
 *
 * Revision 2.0  1994/09/28  10:33:47  neelin
 * Release of minc version 0.2
 *
 * Revision 1.9  94/09/28  10:33:28  neelin
 * Pre-release
 * 
 * Revision 1.8  93/11/03  17:16:19  neelin
 * Removed check for DPR==1 when getting images (this applies only to data).
 * 
 * Revision 1.7  93/10/06  10:15:46  neelin
 * Added include of memory.h for compilation on SUNs
 * 
 * Revision 1.6  93/09/22  14:50:32  neelin
 * Added DTYP = 2 for short values in scx_get_image (this isn't documented,
 * but seems to occur for version 6 files sometimes).
 * 
 * Revision 1.5  93/08/31  12:08:28  neelin
 * Added conditional definition of SEEK_SET.
 * 
 * Revision 1.4  93/08/11  15:27:42  neelin
 * Added RCS logging in source.
 * 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <scx_header_def.h>
#include <scx_file.h>
#include <vax_conversions.h>

#ifndef SEEK_SET
#  define SEEK_SET 0
#endif

/* Private functions */
private void scx_get_value(unsigned char *header, long position, 
                           scx_mnem_types type, int length, 
                           long *lvalue, float *fvalue, char *svalue);

/* Constants */
#define TRUE 1
#define FALSE 0
#define HEADER_SIZE 2048
#define FILE_TYPE_POSITION 0
#define FILE_TYPE_TYPE scx_word
#define SCX_NCS  "NCS"
#define SCX_SPOS "SPOS"
#define SCX_DPR  "DPR"
#define SCX_FHS  "FHS"
#define SCX_REC  "REC"
#define SCX_DTYP "DTYP"
#define SCX_IMFM "IMFM"
#define SCX_SPOS "SPOS"
#define MAX_PIX_SIZE 2


/* ----------------------------- MNI Header -----------------------------------
@NAME       : scx_open
@INPUT      : filename - name of file to open
@OUTPUT     : (none)
@RETURNS    : Pointer to scanditronix file descriptor or NULL if an error 
              occurs.
@DESCRIPTION: Routine to open a scanditronix file (for reading only), given 
              its pathname.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 8, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public scx_file *scx_open(char *filename)
{
   scx_file *file;
   long file_type, nslice, spos;
   int i;

   /* Allocate space for a scx file structure */
   file = (void *) malloc(sizeof(scx_file));

   /* Open the file */
   if ((file->file_pointer=fopen(filename, "rb")) == NULL) {
      free(file);
      return NULL;
   }

   /* Read in the header */
   file->header = (void *) malloc(HEADER_SIZE);
   if (fread(file->header, sizeof(char), (size_t) HEADER_SIZE, 
             file->file_pointer) != HEADER_SIZE) {
      free(file->header);
      free(file);
      return NULL;
   }

   /* Get the file type from the header */
   scx_get_value(file->header, FILE_TYPE_POSITION, FILE_TYPE_TYPE, 1,
                 &file_type, NULL, NULL);

   /* Search for the file type */
   for (i=0; (scx_file_types[i].file_type!=0) && 
             (file_type!=scx_file_types[i].file_type); i++) {}
   if (scx_file_types[i].file_type==0) {
      free(file->header);
      free(file);
      return NULL;
   }
   file->blocks = scx_file_types[i].block_list;
   file->mnemonics = scx_file_types[i].mnemonic_list;

   /* Get the number of mnemonics in the header */
   for (i=0; file->mnemonics[i].name!=NULL; i++) {}
   file->num_mnemonics = i;

   /* Check for a consistent header by looking at the slice positions */
   if (scx_get_mnem(file, SCX_NCS, 0, &nslice, NULL, NULL) || (nslice<=0)) {
      free(file->header);
      free(file);
      return NULL;
   }
   for (i=0; i<nslice; i++) {
      if (scx_get_mnem(file, SCX_SPOS, i, &spos, NULL, NULL) ||
          (spos<0) || (spos>nslice)) {
         free(file->header);
         free(file);
         return NULL;
      }
   }

   return file;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scx_close
@INPUT      : file - scanditronix file descriptor to close
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to close a scanditronix file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 8, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void scx_close(scx_file *file)
{
   if (file==NULL) return;
   (void) fclose(file->file_pointer);
   free(file->header);
   free(file);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scx_get_mnem
@INPUT      : file - scanditronix file descriptor
              mnem - mnemonic to find
              multiplicity - index of value to get
@OUTPUT     : lvalue - long integer value to return
              fvalue - floating point value to return
              svalue - string value to return
@RETURNS    : TRUE if an error occurs, FALSE otherwise
@DESCRIPTION: Routine to get a mnemonic's value from a scanditronix file
              header.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 8, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int scx_get_mnem(scx_file *file, char *mnem, int multiplicity,
                        long *lvalue, float *fvalue, char *svalue)
{
   scx_block_type *block_ptr;
   scx_mnemonic_type *mnem_ptr;
   long position;

   /* Check file pointer */
   if (file==NULL) return TRUE;

   /* Check for negative multiplicity */
   if (multiplicity < 0) return TRUE;

   /* Search for the mnemonic */
   for (mnem_ptr=file->mnemonics; 
           (mnem_ptr->name!=NULL) && (strcmp(mnem_ptr->name, mnem)!=0);
              mnem_ptr++) {}
   if (mnem_ptr->name==NULL) return TRUE;

   /* Check for a mnemonic not in the file (a constant). We only handle
      integer constants */
   if (!mnem_ptr->in_file) {
      switch (mnem_ptr->type) {
      case scx_byte:
      case scx_word:
      case scx_long:
         if (lvalue!=NULL) *lvalue = mnem_ptr->mdefault;
         if (fvalue!=NULL) *fvalue = mnem_ptr->mdefault;
         if (svalue!=NULL) 
            (void) sprintf(svalue, "%d", (int) mnem_ptr->mdefault);
         break;
      default:
         return TRUE;
      }
   }

   /* Mnemonic is in file header */
   else {

      /* Calculate the position of the mnemonic by looping through
         the list of parent blocks */
      position = mnem_ptr->start;
      for (block_ptr = mnem_ptr->block;
              (multiplicity>0) && (block_ptr!=NULL);
                 block_ptr = block_ptr->parent) {
         position += block_ptr->length *
            (multiplicity % block_ptr->multiplicity);
         multiplicity /= block_ptr->multiplicity;
      }
      if (multiplicity > 0) return TRUE;

      /* Get the value */
      scx_get_value(file->header, position, mnem_ptr->type, mnem_ptr->length,
                    lvalue, fvalue, svalue);

   }

   return FALSE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scx_get_value
@INPUT      : header - pointer to header data
              position - postion of value to read
              type - type of value to get
              length - number of values to get
@OUTPUT     : lvalue - long integer value to return
              fvalue - floating point value to return
              svalue - string value to return
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get a mnemonic's value from a scanditronix file
              header, given the position and type.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 8, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void scx_get_value(unsigned char *header, long position, 
                           scx_mnem_types type, int length, 
                           long *lvalue, float *fvalue, char *svalue)
{
   void *data;
   unsigned char *bdata;
   short *idata;
   long *ldata;
   float *fdata;

   /* Check length */
   if (length<=0) return;
   
   /* Get the values */
   data = (void *) malloc(length * scx_mnem_type_size[type]);
   bdata = data; idata = data; ldata = data; fdata = data;
   switch (type) {
   case scx_byte:
   case scx_string:
   case scx_time:
   case scx_date:
      (void) memcpy(bdata, &header[position], length);
      break;
   case scx_word:
   case scx_short_float:
      get_vax_short(length, &header[position], idata);
      break;
   case scx_long:
      get_vax_long(length, &header[position], ldata);
      break;
   case scx_float:
      get_vax_float(length, &header[position], fdata);
   }

   /* Convert the values */
   switch (type) {
   case scx_byte:
      if (lvalue!=NULL) *lvalue = bdata[0];
      if (fvalue!=NULL) *fvalue = bdata[0];
      if (svalue!=NULL) (void) sprintf(svalue, "%d", (int) bdata[0]);
      break;
   case scx_string:
      if (lvalue!=NULL) *lvalue = 0;
      if (fvalue!=NULL) *fvalue = 0;
      if (svalue!=NULL) {
         (void) memcpy(svalue, bdata, length);
         svalue[length]='\0';
      }
      break;
   case scx_time:
      if (lvalue!=NULL) *lvalue = 0;
      if (fvalue!=NULL) *fvalue = 0;
      if (svalue!=NULL) {
         if (length==3)
            (void) sprintf(svalue, "%02d:%02d:%02d", 
                           (int) bdata[0], (int) bdata[1], (int) bdata[2]);
         else if (length==4)
            (void) sprintf(svalue, "%02d:%02d:%02d.%02d", 
                           (int) bdata[0], (int) bdata[1], 
                           (int) bdata[2], (int) bdata[3]);
         else
            svalue[0]='\0';
      }
      break;
   case scx_date:
      if (lvalue!=NULL) *lvalue = 0;
      if (fvalue!=NULL) *fvalue = 0;
      if (svalue!=NULL) {
         if (length==3)
            (void) sprintf(svalue, "%02d-%02d-%02d", 
                           (int) bdata[0], (int) bdata[1], (int) bdata[2]);
         else
            svalue[0]='\0';
      }
      break;
   case scx_word:
      if (lvalue!=NULL) *lvalue = idata[0];
      if (fvalue!=NULL) *fvalue = idata[0];
      if (svalue!=NULL) (void) sprintf(svalue, "%d", (int) idata[0]);
      break;
   case scx_short_float:
      if (lvalue!=NULL) *lvalue = idata[0]/100.0;
      if (fvalue!=NULL) *fvalue = idata[0]/100.0;
      if (svalue!=NULL) (void) sprintf(svalue, "%8g", (double) idata[0]/100.0);
      break;
   case scx_long:
      if (lvalue!=NULL) *lvalue = ldata[0];
      if (fvalue!=NULL) *fvalue = ldata[0];
      if (svalue!=NULL) (void) sprintf(svalue, "%d", (int) ldata[0]);
      break;
   case scx_float:
      if (lvalue!=NULL) *lvalue = fdata[0];
      if (fvalue!=NULL) *fvalue = fdata[0];
      if (svalue!=NULL) (void) sprintf(svalue, "%8g", (double) fdata[0]);
      break;
   }

   free(data);
      
   return;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scx_get_image
@INPUT      : file - scanditronix file pointer
              image_num - image to get (first image is zero)
@OUTPUT     : image - pointer to array into which image should be written.
@RETURNS    : TRUE if an error occurs, false otherwise.
@DESCRIPTION: Routine to get an image from a scanditronix image file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 11, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public int scx_get_image(scx_file *file, int image_num, short *image)
{
   long header_size, record_size, data_type;
   long image_width, image_pos, image_npix, image_size;
   long file_offset, array_offset, ipix;
   int pix_size;
   unsigned char *bimage;

   /* Check file pointer */
   if (file==NULL) return TRUE;

   /* Parameters for calculating image position */
   if (scx_get_mnem(file, SCX_FHS, 0, &header_size, NULL, NULL) ||
       scx_get_mnem(file, SCX_REC, 0, &record_size, NULL, NULL) ||
       scx_get_mnem(file, SCX_DTYP, 0, &data_type, NULL, NULL) ||
       scx_get_mnem(file, SCX_IMFM, 0, &image_width, NULL, NULL) ||
       scx_get_mnem(file, SCX_SPOS, image_num, &image_pos, NULL, NULL) ||
       (header_size<0) || (record_size<0) || (image_width<0) ||
       (image_pos<0)) {
      return TRUE;
   }

   /* Check that image is in file */
   if (image_pos<1) return TRUE;

   /* Check data type */
   switch (data_type) {
   case 0: pix_size=2; break;
   case 1: pix_size=1; break;
   case 2: pix_size=2; break;
   default: return TRUE;
   }

   /* Calculate image size and offsets */
   image_npix = image_width * image_width;
   image_size = image_npix * pix_size;
   file_offset = header_size * record_size + (image_pos-1) * image_size;
   array_offset = image_npix * (MAX_PIX_SIZE - pix_size);
   bimage = (unsigned char *) image;

   /* Read in the image */
   if (fseek(file->file_pointer, file_offset, SEEK_SET) ||
       (fread(&bimage[array_offset], (size_t) pix_size, (size_t) image_npix, 
              file->file_pointer) != image_npix)) {
      return TRUE;
   }

   /* Transform the image to the right type */
   switch (pix_size) {
   case 1:
      for (ipix=0; ipix<image_npix; ipix++) {
         image[ipix] = bimage[array_offset+ipix];
      }
      break;
   case 2:
      get_vax_short(image_npix, image, image);
      break;
   default:
      return TRUE;
   }

   return FALSE;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scx_list_mnems
@INPUT      : file - pointer to an open scanditronix file
              index - counter to identify next mnemonic
@OUTPUT     : mname - pointer to array into which name should be written
              mmult - multiplicity of mnemonic
              mtype - type of mnemonic (can be scx_long, scx_float or 
                 scx_string)
@RETURNS    : Value of mname or NULL if index out of range.
@DESCRIPTION: Routine to list the mnemonics that can be found in a scanditronix
              file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public char *scx_list_mnems(scx_file *file,int index, char *mname, 
                            int *mmult, scx_mnem_types *mtype)
{
   scx_block_type *block_ptr;

   /* Check file pointer and index */
   if ((file == NULL) || (index < 0) || (index >= file->num_mnemonics))
      return NULL;

   /* Copy the name */
   (void) strcpy(mname, file->mnemonics[index].name);

   /* Get the multiplicity */
   *mmult = 1;
   for (block_ptr = file->mnemonics[index].block;
        block_ptr!=NULL;
        block_ptr = block_ptr->parent) {
      *mmult *= block_ptr->multiplicity;
   }

   /* Get the mnemonic type */
   switch (file->mnemonics[index].type) {
   case scx_byte: *mtype = scx_long; break;
   case scx_string: *mtype = scx_string; break;
   case scx_time: *mtype = scx_string; break;
   case scx_date: *mtype = scx_string; break;
   case scx_word: *mtype = scx_long; break;
   case scx_short_float: *mtype = scx_float; break;
   case scx_long: *mtype = scx_long; break;
   case scx_float: *mtype = scx_float; break;
   }

   return mname;

}

