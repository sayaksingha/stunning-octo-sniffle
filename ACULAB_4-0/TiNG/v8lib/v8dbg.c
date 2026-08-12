#include <stdio.h>
#include "v8.h"

void v8_show_code(unsigned char *cp, unsigned char *endp)
{
 if (cp >= endp) return;
 if (!*cp) cp++;	// CI synchronisation
 else if (*cp == 0xe0) cp++;	// CM/JM synchronisation
 for (;;) {
	if (cp >= endp) return;
	switch (*cp & 0x1f) {
	case 1:
		printf("call ");
		switch (*cp) {
		case 1: printf("reserved"); break;
		case 0x21: printf("H.324"); break;
		case 0x41: printf("V.18"); break;
		case 0x61: printf("T.101"); break;
		case 0x81: printf("T.30tx"); break;
		case 0xa1: printf("T.30rx"); break;
		case 0xc1: printf("data"); break;
		case 0xe1: printf("ext"); break;
		}
		cp++;
		break;
	case 5:
		printf("mod");
		if (*cp & 0x20) printf(" V.90");
		if (*cp & 0x40) printf(" V.34fdx");
		if (*cp & 0x80) printf(" V.34hdx");
		if ((*++cp & 0x38) == 0x10) {
			if (*cp & 1) printf(" V.32");
			if (*cp & 2) printf(" V.22");
			if (*cp & 4) printf(" V.17");
			if (*cp & 0x40) printf(" V.29");
			if (*cp & 0x80) printf(" V.27ter");
			if ((*++cp & 0x38) == 0x10) {
				if (*cp & 1) printf(" V.26ter");
				if (*cp & 2) printf(" V.26bis");
				if (*cp & 4) printf(" V.23fdx");
				if (*cp & 0x40) printf(" V.23hdx");
				if (*cp & 0x80) printf(" V.21");
			}
		}
		break;
	case 0xa:
		printf("proto");
		switch (*cp) {
		case 0x2a: printf(" V.42"); break;
		case 0xea: printf(" ext"); break;
		}
		cp++;
		break;
	case 0xd:
		printf("PSTN");
		if (*cp & 0x20) printf(" cell-call");
		if (*cp & 0x40) printf(" cell-ans");
		printf(*cp & 0x80 ? " dig" : " ana");
		cp++;
		break;
	case 0xf:
		printf("NSF 0x%02x", *cp);
		cp++;
		break;
	case 0x6:
		printf("TIA 0x%02x", *cp);
		cp++;
		break;
	case 0x7:
		printf("V.90");
		if (*cp & 0x20) printf(" ana");
		if (*cp & 0x40) printf(" dig");
		if (*cp & 0x80) printf(" res");
		cp++;
		break;
	default:
		if (*cp & 0x10) {
			printf("cat:%02x - bogus\n", *cp);
			return;
		}
		printf("unk %02x", *cp++);
		break;
	}
	for (; cp < endp && (*cp & 0x10); cp++) {
		printf(" x%02x", *cp);
	}
	printf("\n");
 }
}
