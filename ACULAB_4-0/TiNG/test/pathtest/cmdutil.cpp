#include "cmdutil.hpp"
#include <cctype>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

char *rqspc(char *s)
{
 if (!s) return 0;
 if (!*s) return s;
 if (!isspace(*s)) return 0;
 do s++; while (isspace(*s));
 return s;
}

char *skipspc(char *s)
{
 while (*s && isspace(*s)) s++;
 return s;
}

char *prefix(char *s, const char *pfx)
{
 while (*pfx) {
 	if (!*s || *s++ != *pfx++) return 0;
 }
 return s;
}

char *wprefix(char *s, const char *pfx)
{
 return rqspc(prefix(s, pfx));
}

char *unesc(char *out, char *cmd)
{
 int limit;
 int c = *cmd++;
 if (!c) return 0;
 *out = c;
 if (c != '\\') return cmd;
 c = *cmd++;
 switch (c) {
 case 'a': *out = '\a'; return cmd;
 case 'b': *out = '\b'; return cmd;
 case 'f': *out = '\f'; return cmd;
 case 'n': *out = '\n'; return cmd;
 case 'r': *out = '\r'; return cmd;
 case 't': *out = '\t'; return cmd;
 case 'v': *out = '\v'; return cmd;
 default: *out = c; return cmd;
 case '0': case '1': case '2': case '3':
 case '4': case '5': case '6': case '7':
	*out = c - '0';
	c = *cmd;
	if (c >= '0' && c <= '7') {
		*out <<= 3;
		*out += c - '0';
		c = *++cmd;
		if (c >= '0' && c <= '7') {
			*out <<= 3;
			*out += c - '0';
			++cmd;
		}
	}
	return cmd;
 case 'x': 
 	limit = 2; break;
 case 'u':
 	limit = 4; break;
 case 'U':
 	limit = 8; break;
 }
 *out = 0;
 for (; limit--;) {
	c = *cmd++;
	if (c >= '0' && c <= '9') c -= '0';
	else if (c >= 'a' && c <= 'f') c -= 'a' - 10;
	else return cmd - 1;
	*out <<= 4;
	*out += c;
 }
 return cmd - 1;
}

char *copyword(char *buff, unsigned buflen, char *cmd)
{
 for (; *cmd;) {
 	if (!buflen) {
 		std::cerr << "Buffer overflow\n";
 		return 0;
	}
	if (*cmd == ' ' || *cmd == '\t') break;
 	cmd = unesc(buff++, cmd);
 	if (!cmd) return 0;	// cannot fail - not at end
 }
 if (!buflen) return 0;
 *buff = 0;
 return cmd;
}

char *rdstring(std::string *bufp, char *cmd)
{
 if (*cmd != '"') {
 	std::cerr << "Expected string to start with '\"', got '"
 		<< cmd << "'\n";
	return 0;
 }
 std::string s;
 cmd++;		// skip initial quote
 for (;;) {
 	char c = *cmd;
 	if (!c) {
 		std::cerr << "Got newline in string\n";
 		return 0;
	} else if (c == '"') {
		*bufp = s;
		return cmd + 1;
	}
	else cmd = unesc(&c, cmd);
	s += c;
 }
}

char *readline(FILE *f)
{
 unsigned space = 0;
 unsigned len = 0;
 char *s = 0;
 for (;;) {
 	int c = getc(f);
 	if (c == EOF) {
 		if (!len) return 0;
 		c = 0;
	} else if (c == '\n') c = 0;
 	if (len >= space) {
 		unsigned ns = space * 2 + 1;
 		char *s2 = (char *) realloc(s, ns);
 		if (!s2) {
 			perror("realloc() failed");
 			free(s);
 			std::cerr << "Cannot read inpit line\n";
 			return 0;
		}
		s = s2;
		space = ns;
	}
	s[len++] = c;
	if (!c) return s;
 }
}

void printquote(char *s)
{
 std::cout << "\"";
 for (;;) {
	int c = *s++;
	if (c & ~0x7f) {
		std::cout << "\\"
			<< ((c >> 6) & 7)
			<< ((c >> 3) & 7)
			<< (c & 7);
	} else switch (c) {
	case 0: std::cout << "\""; return;
	case '\a': std::cout << "\\a"; break;
	case '\b': std::cout << "\\b"; break;
	case '\f': std::cout << "\\f"; break;
	case '\n': std::cout << "\\n"; break;
	case '\r': std::cout << "\\r"; break;
	case '\t': std::cout << "\\t"; break;
	case '\v': std::cout << "\\v"; break;
	case '\\': std::cout << "\\\\"; break;
	default:
		std::cout << (char) c; break;
	}
 }
}
