/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_date.h

   Description: The header file defines the basic types, constants and macros for the
                data set library part "ds_date": date and time converting handling.


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

#ifndef DS_DATE
#define DS_DATE

/* DECLARATION: types */

typedef struct ds_date_tag
{
  long Year;						   /* four digits e.g. 1989 */
  long Month;						   /* 1 - 12 */
  long Day;						   /* 1 - 31 */
} ds_date_t;


typedef struct ds_time_tag
{
  long Hour;						   /* 0 - 23 */
  long Minute;						   /* 0 - 59 */
  long Second;						   /* 0 - 59 */
  long Fraction;					   /* 0 - 999 */
} ds_time_t;


typedef struct date_position_table_tag
{
  long PreBegin;					   /* number of first character of
							      date string part pre-number;
							      always 0 */
  long PreDelimiter;					   /* number of pre-delimiter
							      character */
  long MonthBegin;					   /* number of first character of
							      date string part month */
  long PostDelimiter;					   /* number of post-delimiter
							      character */
  long PostBegin;					   /* number of first character of
							      date string part post-number */
} date_position_table_t;


typedef struct time_position_table_tag
{
  long HourBegin;					   /* number of first character of
							      time string part hour; always 0 */
  long HourDelimiter;					   /* number of hour delimiter
							      character */
  long MinuteBegin;					   /* number of first character of
							      time string part minute */
  long MinuteDelimiter;					   /* number of minute delimiter
							      character */
  long SecondBegin;					   /* number of first character of
							      time string part second */
  long SecondDelimiter;					   /* number of second delimiter
							      character */
  long FractionBegin;					   /* number of first character of
							      time string part fraction */

} time_position_table_t;


typedef union ds_pdp_time_tag
{
  short ValueAsShort[2];
  long ValueAsLong;
} ds_pdp_time_t;



/* PRECOMPILER: common constants */
#define CENTURY                                                                         1900L
#define DEFAULT                                                                            0L
#define MAX_ALLOWED_DATE_STRING_LENGTH                                                    20L
#define MAX_ALLOWED_TIME_STRING_LENGTH                                                    15L
#define WITH_FRACTION                                                                      2L
#define WITH_SECOND                                                                        1L


/* PRECOMPILER: range constants */
#define DS_A_DAY_IN_MS                                          (24.0 * 60.0 * 60.0 * 1000.0)
#define DS_A_HOUR_IN_MS                                                      (60 * 60 * 1000)
#define DS_A_MINUTE_IN_MS                                                         (60 * 1000)
#define DS_A_SECOND_IN_MS                                                              (1000)

#define DS_A_HOUR_IN_S                                                              (60 * 60)
#define DS_A_MINUTE_IN_S                                                                 (60)


/* PRECOMPILER: delimiter constants */
#define COMMON_DATE_DELIMITER_SET                                                      ".-/:"
#define DB_DATE_DELIMITER_SET                                                             "-"
#define NEMA_DATE_DELIMITER_SET                                                           "."
#define VMS_DATE_DELIMITER_SET                                                            "-"
#define COMMON_TIME_DELIMITER_SET                                                      ":.-/"
#define DB_TIME_DELIMITER_SET                                                             ":"
#define NEMA_TIME_DELIMITER_SET                                                          ":."
#define VMS_TIME_DELIMITER_SET                                                           ":."


#endif
