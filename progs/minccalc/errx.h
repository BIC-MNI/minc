/* Just in case you don't have errx, but you have gcc */

#if !defined(__OpenBSD__) && defined(__GNUC__)
# define errx(exitcode, fmt , args) \
   do { fprintf(stderr, "imgcalc: " fmt "\n", ## args ); exit(1); } while(0)
# define err(exitcode, fmt , args) \
   do { fprintf(stderr, "imgcalc: " fmt , ## args ); \
      fprintf(stderr, ": %s\n", strerror(errno)); exit(1); } while(0)
#else
# include <errno.h>
#endif

