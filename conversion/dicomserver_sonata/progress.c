
// This function prints a text progress bar within a term window
// Input arguments assume a for loop starting at zero:
//
//      for (index =  0; index < end; index++) { ...

#include <stdio.h>
#include <math.h>

int progress(long index, int end, char *message) {

  int ix;
  int width = 50;
  int nchars;

  if (index == 0) {

    if (strlen(message) > 20) {
      // truncate message if too long
      message[20] = '\0';
    }

    printf("%-20s |<--",message);
    for (ix = 0; ix < width; ix++) { 
      printf(" ");
    }
    printf("|");
    for (ix = 0; ix < width+1; ix++) { 
      printf("\b");
    }
  } else if ((index > 0) && (index < end)) {

    nchars = (((float)index/(float)(end-1)) * width) - 
      floor(((float)(index-1)/(float)(end-1)) * width);

    for (ix = 0; ix < nchars; ix++) {
      printf("\b->");
      (void) fflush(stdout);
    }

    // print terminating newline at end if we're done
    if (index == end-1) {
      printf("\n");
    }
  } else {
    fprintf(stderr,"PROGRESS:  bad input indices!\n");
    return 0;
  }
}





