/* showwav.c - show the contents of WAV files */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TiNGTYPE_QNX
#include <sys/types.h>
#endif

#ifdef TiNGTYPE_WINNT
#define ssize_t size_t
#endif

static FILE *efopen(const char *filename, const char *mode)
{
 FILE *f = fopen(filename, mode);
 if (!f) {
 	fprintf(stderr, "fopen(%s, %s) failed: %s\n",
 		filename, mode, strerror(errno));
 }
 return f;
}

static ssize_t efread(void *ptr, size_t size, size_t nitems, FILE *stream)
{
 ssize_t n = fread(ptr, size, nitems, stream);
 if (n < 0) {
	fprintf(stderr, "fread(, %d, %d, ) failed: %s\n",
		size, nitems, strerror(errno));
 }
 return n;
}

static int efseek(FILE *stream, long offset, int whence)
{
 int n = fseek(stream, offset, whence);
 if (n == -1) {
	fprintf(stderr, "fseek(, %ld, %d) failed: %s\n",
		offset, whence, strerror(errno));
 }
 return n;
}

static int efclose(FILE *stream)
{
 int n = fclose(stream);
 if (n) {
	fprintf(stderr, "fclose() failed: %s\n", strerror(errno));
 }
 return n;
}

struct wav_info {
	unsigned data_size;
	unsigned data_start;
	int wFormatTag;			// format type
	unsigned nChannels;		// number of channels
	unsigned nSamplesPerSec;	// sample rate
	unsigned nAvgBytesPerSec;	// for buffer estimation
	unsigned nBlockAlign;		// block size of data
	unsigned wBitsPerSample;	// bits per sample of mono data
};

static unsigned cvt16(unsigned char *buf)
{
 return buf[0] + (buf[1] << 8);
}

static unsigned cvt32(unsigned char *buf)
{
 return buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}

static void showformat(int format)
{
 switch (format) {
	case 0x0000:
		printf("Format: UNKNOWN\n");
		break;
	case 0x0001:
		printf("Format: PCM\n");
		break;
	case 0x0002:
		printf("Format: ADPCM\n");
		break;
	case 0x0003:
		printf("Format: IEEE_FLOAT\n");
		break;
	case 0x0005:
		printf("Format: IBM_CVSD\n");
		break;
	case 0x0006:
		printf("Format: ALAW\n");
		break;
	case 0x0007:
		printf("Format: MULAW\n");
		break;
	case 0x0010:
		printf("Format: OKI_ADPCM\n");
		break;
	case 0x0011:
		printf("Format: IMA/DVI_ADPCM\n");
		break;
	case 0x0012:
		printf("Format: MEDIASPACE_ADPCM\n");
		break;
	case 0x0013:
		printf("Format: SIERRA_ADPCM\n");
		break;
	case 0x0014:
		printf("Format: G723_ADPCM\n");
		break;
	case 0x0015:
		printf("Format: DIGISTD\n");
		break;
	case 0x0016:
		printf("Format: DIGIFIX\n");
		break;
	case 0x0017:
		printf("Format: DIALOGIC_OKI_ADPCM\n");
		break;
	case 0x0018:
		printf("Format: MEDIAVISION_ADPCM\n");
		break;
	case 0x0020:
		printf("Format: YAMAHA_ADPCM\n");
		break;
	case 0x0021:
		printf("Format: SONARC\n");
		break;
	case 0x0022:
		printf("Format: DSPGROUP_TRUESPEECH\n");
		break;
	case 0x0023:
		printf("Format: ECHOSC1\n");
		break;
	case 0x0024:
		printf("Format: AUDIOFILE_AF36\n");
		break;
	case 0x0025:
		printf("Format: APTX\n");
		break;
	case 0x0026:
		printf("Format: AUDIOFILE_AF10\n");
		break;
	case 0x0027: /* Registered tag for Prosody 16kbps and 12kbps data compression */
		printf("Format: PROSODY_1612\n");
		break;
	case 0x0030:
		printf("Format: DOLBY_AC2\n");
		break;
	case 0x0031:
		printf("Format: GSM610\n");
		break;
	case 0x0032:
		printf("Format: MSNAUDIO\n");
		break;
	case 0x0033:
		printf("Format: ANTEX_ADPCME\n");
		break;
	case 0x0034:
		printf("Format: CONTROL_RES_VQLPC\n");
		break;
	case 0x0035:
		printf("Format: DIGIREAL\n");
		break;
	case 0x0036:
		printf("Format: DIGIADPCM\n");
		break;
	case 0x0037:
		printf("Format: CONTROL_RES_CR10\n");
		break;
	case 0x0038:
		printf("Format: NMS_VBXADPCM\n");
		break;
	case 0x0039:
		printf("Format: CS_IMAADPCM\n");
		break;
	case 0x003A:
		printf("Format: ECHOSC3\n");
		break;
	case 0x003B:
		printf("Format: ROCKWELL_ADPCM\n");
		break;
	case 0x003C:
		printf("Format: ROCKWELL_DIGITALK\n");
		break;
	case 0x003D:
		printf("Format: XEBEC\n");
		break;
	case 0x0040:
		printf("Format: G721_ADPCM\n");
		break;
	case 0x0041:
		printf("Format: G728_CELP\n");
		break;
	case 0x0050:
		printf("Format: MPEG\n");
		break;
	case 0x0055:
		printf("Format: MPEGLAYER3\n");
		break;
	case 0x0060:
		printf("Format: CIRRUS\n");
		break;
	case 0x0061:
		printf("Format: ESPCM\n");
		break;
	case 0x0062:
		printf("Format: VOXWARE\n");
		break;
	case 0x0063:
		printf("Format: CANOPUS_ATRAC\n");
		break;
	case 0x0064:
		printf("Format: G726_ADPCM\n");
		break;
	case 0x0065:
		printf("Format: G722_ADPCM\n");
		break;
	case 0x0066:
		printf("Format: DSAT\n");
		break;
	case 0x0067:
		printf("Format: DSAT_DISPLAY\n");
		break;
	case 0x0080:
		printf("Format: SOFTSOUND\n");
		break;
	case 0x0094: /* Registered tag for Prosody 8kbps data compression */
		printf("Format: PROSODY_8KBPS\n");
		break;
	case 0x0100:
		printf("Format: RHETOREX_ADPCM\n");
		break;
	case 0x0200:
		printf("Format: CREATIVE_ADPCM\n");
		break;
	case 0x0202:
		printf("Format: CREATIVE_FASTSPEECH8\n");
		break;
	case 0x0203:
		printf("Format: CREATIVE_FASTSPEECH10\n");
		break;
	case 0x0220:
		printf("Format: QUARTERDECK\n");
		break;
	case 0x0300:
		printf("Format: FM_TOWNS_SND\n");
		break;
	case 0x0400:
		printf("Format: BTV_DIGITAL\n");
		break;
	case 0x1000:
		printf("Format: OLIGSM\n");
		break;
	case 0x1001:
		printf("Format: OLIADPCM\n");
		break;
	case 0x1002:
		printf("Format: OLICELP\n");
		break;
	case 0x1003:
		printf("Format: OLISBC\n");
		break;
	case 0x1004:
		printf("Format: OLIOPR\n");
		break;
	case 0x1100:
		printf("Format: LH_CODEC\n");
		break;
	case 0x1400:
		printf("Format: NORRIS\n");
		break;
	case 0xAC16:	/* Defunct development tag, until registered with Microsoft */
		printf("Format: ACURATE_16\n");
		break;
	case 0xFFFF:
		printf("Format: DEVELOPMENT\n");
		break;
	default:
		printf("Format: 0x%04x\n", format);
 }
}

static void indent(unsigned level)
{
 while (level--) putchar('\t');
}

static int hexdump(FILE *fp, unsigned size)
{
 for (; size;) {
	unsigned len = size > 16 ? 16 : size;
	unsigned char buf[16];
	unsigned i;
	if (efread(buf, len, 1, fp) != 1) {
		fprintf(stderr, "Cannot read more chunk data\n");
		return 1;
	}
	for (i=0; i<len; i++) printf("%02x ", buf[i]);
	for (; i<16; i++) printf("   ");
	for (i=0; i<len; i++) {
		int c = buf[i];
		if (c < 32) {
			printf("^%c", c + 'A' - 1);
		} else if (c == 127) {
			printf("^?");
		} else if (c >= 128) {
			c -= 128;
			if (c >= ' ' && c < 127) {
				printf("|%c", c);
			} else printf("%02x", c);
		} else printf(" %c", c);
	}
	printf("\n");
	size -= len;
 }
 return 0;
}

static int showchunks(FILE *fp, unsigned limit, unsigned level)
{
 for (; limit >= 8;) {	// read each chunk
	unsigned char buf[16];
	unsigned size;
	int pad;
	ssize_t e = efread(buf, 8, 1, fp);
	if (e != 1) {
		if (e) {
			fprintf(stderr, "Cannot read chunk header\n");
			return 1;
		}
		return 0;
	}
	limit -= 8;
	indent(level);
	printf("Chunk: %c%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);
	size = cvt32(&buf[4]);
	pad = size & 1;
	indent(level);
	printf("Chunksize: %u\n", size);
	if (size+pad > limit) {
		indent(level);
		printf("!Error: chunk too big: %u+%u > %u\n", size, pad, limit);
		limit = 0;
	} else limit -= size;
	if (buf[0] == 'f'
		&& buf[1] == 'm'
		&& buf[2] == 't'
		&& buf[3] == ' ') {
			// we're at the 'fmt ' sub-chunk. 'size' is its size
		if (size < 16) {
			fprintf(stderr, "fmt chunk size too small (%u) < 16\n",
				size);
			return 1;
		}
		if (efread(buf, 16, 1, fp) != 1) {
			fprintf(stderr, "Cannot read fmt chunk data\n");
			return 1;
		}
		size -= 16;	// deduct the size of the bit we have already read
		showformat(cvt16(&buf[0]));
		indent(level);
		printf("Channels: %d\n", cvt16(&buf[2]));
		indent(level);
		printf("Samples/sec: %d\n", cvt32(&buf[4]));
		indent(level);
		printf("AverageBytes/sec: %d\n", cvt32(&buf[8]));
		indent(level);
		printf("nBlockAlign: %d\n", cvt16(&buf[12]));
		indent(level);
		printf("wBitsPerSample: %d\n", cvt16(&buf[14]));
	} else if (buf[0] == 'L'
		&& buf[1] == 'I'
		&& buf[2] == 'S'
		&& buf[3] == 'T') {
		if (size < 4) {
			fprintf(stderr, "INFO chunk size too small (%u) < 4\n",
				size);
			return 1;
		}
		if (efread(buf, 4, 1, fp) != 1) {
			fprintf(stderr, "Cannot read LIST chunk type\n");
			return 1;
		}
		indent(level);
		printf("LIST-type: %c%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);
		if (buf[0] == 'I'
			&& buf[1] == 'N'
			&& buf[2] == 'F'
			&& buf[3] == 'O') {
			showchunks(fp, size-4, level+1);
			size = 0;
		}
	} else if (buf[0] == 'd'
		&& buf[1] == 'a'
		&& buf[2] == 't'
		&& buf[3] == 'a') {
		// skip past it
		if (size && efseek(fp, size, SEEK_CUR)) {
			fprintf(stderr, "Cannot seek past chunk data\n");
			return 1;
		}
		size = 0;
	}
	if (size) {
		if (hexdump(fp, size)) {
			fprintf(stderr, "Error printing chunk data\n");
			return 1;
		}
	}
	if (pad) {
		e = efread(buf, 1, 1, fp);
		if (e != 1) {
			if (e) {
				fprintf(stderr, "Cannot read padding\n");
				return 1;
			}
			return 0;
		}
		indent(level);
		printf("Pad: %x\n", buf[0]);
		limit--;
	}
	printf("\n");
 }
 if (limit) {
 	printf("!Error: space too small for chunk: %u\n", limit);
 	hexdump(fp, limit);
 }
 return 0;
}

static int showwav(FILE *fp)
{
 unsigned char buf[16];
 unsigned size;
 	// read and check the file header
 if (efread(buf, 12, 1, fp) != 1) {
	fprintf(stderr, "Cannot read wav header\n");
	return 1;
 }
 if (buf[0] != 'R'
 	|| buf[1] != 'I'
 	|| buf[2] != 'F'
 	|| buf[3] != 'F') {
	fprintf(stderr, "Not a wav file: does not start with 'RIFF'\n");
	return 1;
 }
 size = cvt32(&buf[4]);
 if (size < 4) {
	fprintf(stderr, "RIFF chunk is too short (%u) < 4\n", size);
	return 1;
 }
 size -= 4;
 	// form type should be 'WAVE'
 if (buf[8] != 'W'
 	|| buf[9] != 'A'
 	|| buf[10] != 'V'
 	|| buf[11] != 'E') {
	fprintf(stderr, "Not a wav file: does not have 'WAVE' in header\n");
 }
 if (showchunks(fp, size, 0)) return 1;
 if (efread(buf, 1, 1, fp) == 1) printf("File-pad: %x\n", buf[0]);
 if (efread(buf, 1, 1, fp)) printf("!Error: extra data after RIFF chunk\n");
 return 0;
}

#include "gen/showwav.args.i"

int main(int argc, char **argv)
{
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || !*argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " file...\n", progname);
	return 1;
 }
 for (; *argv; argv++) {
	FILE *fp = efopen(*argv, "rb");
	if (!fp || showwav(fp) || efclose(fp)) {
		fprintf(stderr, "File: %s\n", *argv);
		return 1;
	}
 }
 return 0;
}
