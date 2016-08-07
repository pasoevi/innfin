#ifndef PARSER_H
#define PARSER_H
#include "actor.h"

int starts_with_c(char s[], char ch);
int parse_jar(char *filename, int realm_id, struct actor **actor);

#endif
