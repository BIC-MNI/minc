/* Copyright David Leonard & Andrew Janke, 2000. All rights reserved. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "node.h"

struct intern {
   const char *s;
   int id;
   struct intern *next;
   int is_scalar;
};

static struct intern *head = NULL;
static ident_t nextid = 0;

ident_t new_ident(const char *s){
   struct intern *i;

   for (i = head; i; i = i->next)
      if (strcmp(i->s, s) == 0)
         return i->id;

   i = malloc(sizeof *i);
   i->id = nextid++;
   i->s = strdup(s);
   i->next = head;
   i->is_scalar = islower(s[0]);
   head = i;
   return i->id;
}

const char *ident_str(ident_t id){
   struct intern *i;

   for (i = head; i; i = i->next)
      if (i->id == id)
         return i->s;
   return "?";
}

int ident_is_scalar(ident_t id){
   struct intern *i;

   for (i = head; i; i = i->next)
      if (i->id == id)
         return i->is_scalar;
   /* errx(1, "ident_is_scalar: no such ident %d", id); */
   fprintf(stderr, "ident_is_scalar: no such ident %d\n", id);
   exit(1);
   return 0;
}

ident_t ident_lookup(char *string)
{
   struct intern *i;

   for (i = head; i; i = i->next) {
      if (strcmp(i->s, string) == 0) {
         return i->id;
      }
   }
   return -1;

}
