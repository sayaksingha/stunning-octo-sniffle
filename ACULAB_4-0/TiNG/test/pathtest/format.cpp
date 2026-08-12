#include <iostream> // Suse 8.2 doesn't like this being after format.hpp
#include "cmdutil.hpp"
#include "format.hpp"
#include "../../class/prosody.hpp"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static const struct {
	const char *name;
	Prosody::kSMDataFormat code;
} fmtname[] = {
	{ "OKIADPCM", Prosody::kSMDataFormatOKIADPCM, },
	{ "oki", Prosody::kSMDataFormatOKIADPCM, },
	{ "ACUBLKPCM", Prosody::kSMDataFormatACUBLKPCM, },
	{ "acu", Prosody::kSMDataFormatACUBLKPCM, },
	{ "ULawPCM", Prosody::kSMDataFormatULawPCM, },
	{ "mu", Prosody::kSMDataFormatULawPCM, },
	{ "ALawPCM", Prosody::kSMDataFormatALawPCM, },
	{ "a", Prosody::kSMDataFormatALawPCM, },
	{ "8bit", Prosody::kSMDataFormat8bit, },
	{ "8", Prosody::kSMDataFormat8bit, },
	{ "Signed8bit", Prosody::kSMDataFormatSigned8bit, },
	{ "8s", Prosody::kSMDataFormatSigned8bit, },
	{ "16bit", Prosody::kSMDataFormat16bit, },
	{ "16", Prosody::kSMDataFormat16bit, },
	{ "IMAADPCM", Prosody::kSMDataFormatIMAADPCM, },
	{ "IMA", Prosody::kSMDataFormatIMAADPCM, },
	{ "None", Prosody::kSMDataFormatNone, },
	{ "n", Prosody::kSMDataFormatNone, },
};

char *parseformat(Prosody::kSMDataFormat *code, char *name)
{
 if (*name == '?') {
	const unsigned colwid = 16;
	unsigned col = 0;
	unsigned u;
	for (u=0; u < arlen(fmtname); u++) {
		unsigned spc;
		if (!(u & 3)) {
			std::cout << "\n";
			col = 0;
		}
		spc = colwid - (col-8) % colwid;
		col += spc;
		while (spc--) std::cout << " ";
		std::cout << fmtname[u].name;
		col += strlen(fmtname[u].name);
	}
	std::cout << "\n";
	return 0;
 }
 unsigned u;
 for (u=0; u < arlen(fmtname); u++) {
	char *s = wprefix(name, fmtname[u].name);
	if (s) {
		*code = fmtname[u].code;
		return s;
	}
 }
 std::cerr << "Unknown format: '" << name << "'\n";
 return 0;
}

const char *formatname(Prosody::kSMDataFormat code)
{
 unsigned u;
 for (u=0; u < arlen(fmtname); u++) {
	if (fmtname[u].code == code) return fmtname[u].name;
 }
 return 0;
}
