#ifndef machine_indep_h
#define machine_indep_h

#include <stdio.h>


#if defined(__STDC__) || defined(__cplusplus)
#if defined(__cplusplus)
extern "C" {
#endif
void SWAB(const void *from, void *to, int length);
int file_data_to_host(char *dptr, int nblks, int dtype);
int read_matrix_data( FILE *fptr, int strtblk, int nblks,
					 char *dptr, int dtype);
int write_matrix_data( FILE *fptr, int strtblk, int nblks,
					 char *dptr, int dtype);
void bufWrite(char* s, char* buf, int* i, int len);
void bufWrite_s(short val, char* buf, int* i);
void bufWrite_i(int val, char* buf, int* i);
void bufWrite_u(unsigned int val, char* buf, int* i);
void bufWrite_f(float val, char* buf, int* i);
void bufRead(char* s, char* buf, int* i, int len);
void bufRead_s(short*, char* buf, int* i);
void bufRead_i(int*, char* buf, int* i);
void bufRead_u(unsigned int*, char* buf, int* i);
void bufRead_f(float*, char* buf, int* i);
#if defined(__cplusplus)
}
#endif /* __cpluplus */
#else /* __STDC__ */
extern void bufWrite(), bufWrite_s(), bufWrite_i(), bufWrite_f();
extern void bufRead(), bufRead_s(), bufRead_i(), bufRead_f();
#endif  /* __STDC__ */

#endif  /* machine_indep_h */
