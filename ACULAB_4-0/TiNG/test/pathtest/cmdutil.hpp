#ifndef INCLUDED_CMDUTIL_HPP
#define INCLUDED_CMDUTIL_HPP

#include <string>
#include <stdio.h>
char *rqspc(char *s);
char *skipspc(char *s);
char *prefix(char *s, const char *pfx);
char *wprefix(char *s, const char *pfx);
char *unesc(char *out, char *cmd);
char *copyword(char *buff, unsigned buflen, char *cmd);
char *rdstring(std::string *bufp, char *cmd);
char *readline(FILE *f);
void printquote(char *s);

#endif
