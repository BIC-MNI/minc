/**\file record.c
 * \brief MINC 2.0 Record Functions
 * \author Bert Vincent
 */

/** This method gets the name of the record dimension
 * TODO: set record name??
 */
int 
miget_record_name(mihandle_t volume,
                  char **name)
{
    return (MI_NOERROR);
}


/** This method gets the length (i.e., number of fields in the case of
 * uniform records and number of bytes for non_uniform ones) of the
 * record.
 */
int 
miget_record_length(mihandle_t volume,
                    int *length)
{
    return (MI_NOERROR);
}

/** This method returns the field name for the given field.
 */
int
miget_record_field_name(mihandle_t volume,
                        int index,
                        char **name)
{
    return (MI_NOERROR);
}

/** This method sets a field name for the given record. e.g. field is "red"
 */
int
miset_record_field_name(mihandle_t volume,
                        int index,
                        const char *name)
{
    return (MI_NOERROR);
}
