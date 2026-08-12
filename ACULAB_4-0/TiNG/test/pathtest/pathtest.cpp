#include "parsecmd.hpp"
#include "cmdutil.hpp"
#include <iostream>
#include <stdio.h>
#include <string>

static int docmds(CFG *cfg, RUNSTATE *runs, FILE *inf)
{
 for (;;) {
	char *s = readline(inf);
	if (!s) break;
 	parsecmd(cfg, runs, s);
	free(s);
 }
 return 0;
}

using Prosody::TiNGtrace;

#include "gen/pathtest.args.i"

int main(int argc, char **argv)
{
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 RUNSTATE runs;
 if (runs.init()) return 1;
 CFG cfs;
 int r = 0;
 if (arg.inifile) {
 	FILE *inf = fopen(arg.inifile, "r");
 	if (!inf) {
 		perror("fopen() failed");
 		std::cerr << "Canot open file: '" << arg.inifile << "'\n";
 		r = 1;
	} else {
		if (docmds(&cfs, &runs, inf)) {
			std::cerr << "Error in inifile '" << arg.inifile << "'\n";
			r = 1;
		}
		fclose(inf);
	}
 }
 if (!r) r = docmds(&cfs, &runs, stdin);
 runs.fini();
 return r;
}
