/* smwavlib.c - WAV file handling */

#include <stdlib.h>
#include <string.h>
#include "smbesp.h"
#include "smhlib.h"
#include "smwavlib.h"
#include "../apilib/trace.h"
#include "../apilib/once.h"

#ifndef ACUAPI
#define ACUAPI
#endif

#ifndef __SMWIN32HLIB__
#ifdef TiNGTYPE_WINNT
#define ssize_t size_t

extern efopen_fn fopen_aculab_TiNG_highapi_ptr;
extern efread_fn fread_aculab_TiNG_highapi_ptr;
extern efwrite_fn fwrite_aculab_TiNG_highapi_ptr;
extern efseek_fn fseek_aculab_TiNG_highapi_ptr;
extern eftell_fn ftell_aculab_TiNG_highapi_ptr;
extern eferror_fn ferror_aculab_TiNG_highapi_ptr;
extern efclose_fn fclose_aculab_TiNG_highapi_ptr;
extern eerrno_ptr_fn errno_aculab_TiNG_highapi_ptr;


#define CALL_FUNC(x) (*(x ## _aculab_TiNG_highapi_ptr))
#define GET_ERRNO (*(*errno_aculab_TiNG_highapi_ptr)())
#endif
#endif

#include "efile.i"

#ifndef  WAVE_FORMAT_UNKNOWN
#define  WAVE_FORMAT_UNKNOWN    0x0000  /*  Microsoft Corporation  */

#ifndef WAVE_FORMAT_PCM	
#define  WAVE_FORMAT_PCM	0x0001  /*  Microsoft Corporation  */
#endif

#define  WAVE_FORMAT_ADPCM      0x0002  /*  Microsoft Corporation  */
#define  WAVE_FORMAT_ALAW       0x0006  /*  Microsoft Corporation  */
#define  WAVE_FORMAT_MULAW      0x0007  /*  Microsoft Corporation  */
#define  WAVE_FORMAT_OKI_ADPCM  0x0010  /*  OKI  */
#define  WAVE_FORMAT_IMA_ADPCM  0x0011  /*  IMA  */
#define  WAVE_FORMAT_DSPGROUP_TRUESPEECH        0x0022  /*  DSP Group, Inc  */
#define  WAVE_FORMAT_PROSODY_1612  0x0027 /* Registered tag for Prosody 16kbps and 12kbps data compression */
#define  WAVE_FORMAT_PROSODY_8KBPS 0x0094 /* Registered tag for Prosody 8kbps data compression */
#define  WAVE_FORMAT_ACURATE_16 0xAC16   /* Defunct development tag, until registered with Microsoft */
#define  WAVE_FORMAT_G721_ADPCM 0x0040   /* Antex Electronics Corporation  */
#define  WAVE_FORMAT_DIALOGIC_OKI_ADPCM  0x0017
#define  WAVE_FORMAT_SPEEX 0xa109

#endif

#define WAV_FILE_DOESNT_EXIST    1
#define WAV_FILE_READ_ERROR      2
#define WAV_FILE_SEEK_ERROR      3
#define WAV_FILE_WRITE_ERROR     4
#define WAV_FILE_NOT_RIFF_FORMAT 8
#define WAV_FILE_NOT_WAV_FORMAT  9
#define WAV_WAVEFORMAT_SEEK_ERROR 10
#define WAV_CHUNK_SEEK_ERROR     11
#define WAV_UNKNOWN_WAV_FORMAT   12

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

static char generator[] = "Prosody high level API, "
	" @(#)$from: " __FILE__  " "
#include "../gen/version.i"
	" $";
;

static unsigned cvt16(unsigned char *buf)
{
 return buf[0] + (buf[1] << 8);
}

static unsigned cvt32(unsigned char *buf)
{
 return buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}

static void cpy16(unsigned char *buf, int v)
{
 buf[0] = v & 0xff;
 buf[1] = (v >> 8) & 0xff;
}

static void cpy32(unsigned char *buf, int v)
{
 buf[0] = v & 0xff;
 buf[1] = (v >> 8) & 0xff;
 buf[2] = (v >> 16) & 0xff;
 buf[3] = (v >> 24) & 0xff;
}

static void cpyfourcc(unsigned char *buf, char *s)
{
 buf[0] = s[0];
 buf[1] = s[1];
 buf[2] = s[2];
 buf[3] = s[3];
}

static int wav_in_open(EFILE *fp, struct wav_info *wavInfo)
{
 unsigned size;
 unsigned char buf[16];
 	// read and check the file header
 if (efread(buf, 12, 1, fp) != 1) return WAV_FILE_READ_ERROR;
 if (buf[0] != 'R'
 	|| buf[1] != 'I'
 	|| buf[2] != 'F'
 	|| buf[3] != 'F') return WAV_FILE_NOT_RIFF_FORMAT;
 	// form type should be 'WAVE'
 if (buf[8] != 'W'
 	|| buf[9] != 'A'
 	|| buf[10] != 'V'
 	|| buf[11] != 'E') return WAV_FILE_NOT_RIFF_FORMAT;
 for (;;) {	// look for a 'fmt ' sub-chunk
	if (efread(buf, 8, 1, fp) != 1) return WAV_FILE_READ_ERROR;
	size = cvt32(&buf[4]);
	if (buf[0] == 'f'
		&& buf[1] == 'm'
		&& buf[2] == 't'
		&& buf[3] == ' ') break;
		// else skip past it
	if (size && efseek(fp, size, SEEK_CUR)) return WAV_FILE_SEEK_ERROR;
 }
	// we're at the 'fmt ' sub-chunk. 'size' is its size
 if (efread(buf, 16, 1, fp) != 1) return WAV_FILE_READ_ERROR;
 size -= 16;	// deduct the size of the bit we have already read
 wavInfo->wFormatTag = cvt16(&buf[0]);
 wavInfo->nChannels = cvt16(&buf[2]);
 wavInfo->nSamplesPerSec = cvt32(&buf[4]);
 wavInfo->nAvgBytesPerSec = cvt32(&buf[8]);
 wavInfo->nBlockAlign = cvt16(&buf[12]);
 wavInfo->wBitsPerSample = cvt16(&buf[14]);
 	// 'size' is remaining data in this sub-chunk
 for (;;) {	// look for a 'data' sub-chunk
		// 'size' is remaining data in this (unsuitable) sub-chunk
	if (size & 1) size++;	// alignment
	if (size && efseek(fp, size, SEEK_CUR)) return WAV_FILE_SEEK_ERROR;
		// we're now at the start of the next sub-chunk
	if (efread(buf, 8, 1, fp) != 1) return WAV_FILE_READ_ERROR;
	size = cvt32(&buf[4]);
	if (buf[0] == 'd'
		&& buf[1] == 'a'
		&& buf[2] == 't'
		&& buf[3] == 'a') break;
	// else go around again, skip past it, and try the next one
 }
 	// we're at the start of the 'data' sub-chunk
 wavInfo->data_start = eftell(fp);
 wavInfo->data_size = size;
 return 0;
}

static int sm_replay_wav_common(char *filename, EFILE **pfd, enum kSMDataFormat *pType, tSM_UT32 *ratep, struct wav_info *pWavInfo)
{
 int err = 0;
 if (! (*pfd = efopen(filename, "rb")) ) return err(ERR_SM_FILE_ACCESS);
 if (wav_in_open(*pfd, pWavInfo)) {
	efclose(*pfd);
	return err(ERR_SM_FILE_FORMAT);
 }
 *ratep = pWavInfo->nSamplesPerSec;
 switch (pWavInfo->wFormatTag) {
	case WAVE_FORMAT_PCM:
		switch (pWavInfo->wBitsPerSample) {
		case 8:
			*pType = kSMDataFormat8bit; 
			break;
		case 16:
			*pType = kSMDataFormat16bit; 
			break;
		default:
			err = err(ERR_SM_FILE_FORMAT);
		}
		break;
	case WAVE_FORMAT_ALAW:
		*pType = kSMDataFormatALawPCM; 
		break;
	case WAVE_FORMAT_MULAW:
		*pType = kSMDataFormatULawPCM; 
		break;
	case WAVE_FORMAT_IMA_ADPCM:
		*pType = kSMDataFormatIMAADPCM;
		break;
	case WAVE_FORMAT_DIALOGIC_OKI_ADPCM:
	case WAVE_FORMAT_OKI_ADPCM:
		*pType = kSMDataFormatOKIADPCM; 
		break;
	case WAVE_FORMAT_SPEEX:
		*pType = kSMDataFormatSpeex; 
		break;
	case WAVE_FORMAT_ACURATE_16: 
	case WAVE_FORMAT_PROSODY_1612:
		*pType = kSMDataFormatACUBLKPCM; 
		break;
	case WAVE_FORMAT_PROSODY_8KBPS:
		*pType = kSMDataFormatCELP8KBPS; 
		break;
#if 0
	case WAVE_FORMAT_G721_ADPCM:
	/* case WAVE_FORMAT_G726_ADPCM: doesnt exist yet in mmreg.h */
		switch(pWavInfo->wBitsPerSample) {
		case 6:
			*pType = kSMDataFormatG726_48KBPS; 
			break;
		case 4:
			*pType = kSMDataFormatG726_32KBPS; 
			break;
		case 3:
			*pType = kSMDataFormatG726_24KBPS; 
			break;
		case 2:
			*pType = kSMDataFormatG726_16KBPS; 
			break;
		default:
			err = err(ERR_SM_FILE_FORMAT);
			break;
		}
		break;
#endif
	default:
		err = err(ERR_SM_FILE_FORMAT);
 }
 if (err) efclose(*pfd);
 return err;
}

STATIC int real_sm_replay_wav_start(char *filename, struct sm_file_replay_parms *file_parms)
{
 struct wav_info wavInfo;
 int rc;
 file_parms->status = 0;
 rc = sm_replay_wav_common(filename,
 	&file_parms->fd,
 	&file_parms->replay_parms.type,
 	&file_parms->replay_parms.sampling_rate,
 	&wavInfo);
 if (rc) return file_parms->status = rc;
 if (file_parms->replay_parms.data_length == 0) {
	file_parms->replay_parms.data_length
		= wavInfo.data_size - file_parms->offset;
 }
 file_parms->offset += wavInfo.data_start;
 rc = sm_replay_file_start(file_parms);
 if (rc) efclose(file_parms->fd);
 return rc;
}


/*
 * Determine Prosody API replay type from header of WAV file - may need
 * to do this in order to locate module capable of playing this format.
 */
STATIC int real_sm_replay_wav_get_type(char *filename, int *replay_type)
{
 struct wav_info wavInfo;
 enum kSMDataFormat type;
 tSM_UT32 rate;
 EFILE *fp;
 int err = sm_replay_wav_common(filename, &fp, &type, &rate, &wavInfo);
 if (!err) efclose(fp);
 *replay_type = type;
 return err;
}

/*
 */
STATIC int real_sm_replay_wav_prepare(struct sm_replay_wav_prepare_parms *prep_parms)
{
 struct wav_info wavInfo;
 EFILE *fp;
 int err = sm_replay_wav_common(prep_parms->filename, &fp, &prep_parms->replay_parms->type, &prep_parms->replay_parms->sampling_rate, &wavInfo);
 if (!err) efclose(fp);
 return err;
}

/*
 * Close file.
 */
STATIC int real_sm_replay_wav_close( struct sm_file_replay_parms* fileparms )
{
 if (efclose(fileparms->fd)) return err(ERR_SM_FILE_ACCESS);
 return 0;
}

/* 
 * Dump a wav header
 * Before calling this routine,
 * wavInfo.wFormatTag and wavInfo.waveformat.nSamplesPerSec
 * should be appropriately set.
 * If the data has been already written, data_size must be the total
 * length of the file.
 */
static int wav_dump_header(struct wav_info *wavInfo, enum kSMDataFormat type, EFILE *fp)
{
 unsigned char hdr[128 + sizeof(generator)];
 unsigned char *p = hdr;
 unsigned char *riff_size;
 unsigned char *list_size;
 unsigned char *fmt_size;
 unsigned char *fact_pos;
 switch (type) {
 	// Can't do 8-bit signed, as no code to represent it
 case kSMDataFormat8bit: 
	wavInfo->wFormatTag 		= WAVE_FORMAT_PCM;
	wavInfo->wBitsPerSample   = 8;
	wavInfo->nBlockAlign = 1;
	break;
 case kSMDataFormat16bit: 
	wavInfo->wFormatTag 		= WAVE_FORMAT_PCM;
	wavInfo->wBitsPerSample   = 16;
	wavInfo->nBlockAlign = 2;
	break;
 case kSMDataFormat8KHzALawPCM: 
	wavInfo->nSamplesPerSec 	= 8000;
 case kSMDataFormatALawPCM: 
	wavInfo->wFormatTag 		= WAVE_FORMAT_ALAW;
	wavInfo->wBitsPerSample   = 8;
	wavInfo->nBlockAlign = 1;
	break;
 case kSMDataFormat6KHzALawPCM: 
	wavInfo->wFormatTag 		= WAVE_FORMAT_ALAW;
	wavInfo->nSamplesPerSec 	= 6000;
	wavInfo->wBitsPerSample   = 8;
	wavInfo->nBlockAlign = 1;
	break;
 case kSMDataFormat8KHzULawPCM:
	wavInfo->nSamplesPerSec 	= 8000;
 case kSMDataFormatULawPCM:
	wavInfo->wFormatTag 		= WAVE_FORMAT_MULAW;
	wavInfo->wBitsPerSample   = 8;
	wavInfo->nBlockAlign = 1;
	break;
 case kSMDataFormat8KHzOKIADPCM:
	wavInfo->nSamplesPerSec 	= 8000;
 case kSMDataFormatOKIADPCM:
	// wavInfo->wFormatTag 		= WAVE_FORMAT_DIALOGIC_OKI_ADPCM;
	wavInfo->wFormatTag 		= WAVE_FORMAT_OKI_ADPCM;
	wavInfo->wBitsPerSample   = 4;
	wavInfo->nBlockAlign = 1;
	break;
 case kSMDataFormat6KHzOKIADPCM:
	// wavInfo->wFormatTag 		= WAVE_FORMAT_DIALOGIC_OKI_ADPCM;
	wavInfo->wFormatTag 		= WAVE_FORMAT_OKI_ADPCM;
	wavInfo->nSamplesPerSec 	= 6000;
	wavInfo->wBitsPerSample   = 4;
	wavInfo->nBlockAlign = 1;
	break;
 case kSMDataFormatIMAADPCM:
	if (!wavInfo->nSamplesPerSec) wavInfo->nSamplesPerSec = 8000;
	wavInfo->wFormatTag 		= WAVE_FORMAT_IMA_ADPCM;
	wavInfo->wBitsPerSample = 4;
	wavInfo->nBlockAlign = 256;
	break;
 case kSMDataFormatSpeex:
	if (!wavInfo->nSamplesPerSec) wavInfo->nSamplesPerSec = 8000;
	wavInfo->wFormatTag 		= WAVE_FORMAT_SPEEX;
	wavInfo->wBitsPerSample = 1;
	wavInfo->nBlockAlign = 8;
	break;
 case kSMDataFormat8KHzACUBLKPCM:
	wavInfo->nSamplesPerSec 	= 8000;
 case kSMDataFormatACUBLKPCM:
	wavInfo->wFormatTag 		= WAVE_FORMAT_PROSODY_1612;
	wavInfo->wBitsPerSample   = 2;
	wavInfo->nBlockAlign = 4;
	break;
 case kSMDataFormat6KHzACUBLKPCM:
	wavInfo->wFormatTag 		= WAVE_FORMAT_PROSODY_1612;
	wavInfo->nSamplesPerSec 	= 6000;
	wavInfo->wBitsPerSample   = 2;
	wavInfo->nBlockAlign = 4;
	break;
 case kSMDataFormatCELP8KBPS:
	wavInfo->wFormatTag 		= WAVE_FORMAT_PROSODY_8KBPS;
	wavInfo->nSamplesPerSec 	= 8000;
	wavInfo->wBitsPerSample   = 1;
	wavInfo->nBlockAlign = 4;
	break;
#if 0
 case kSMDataFormatG726_48KBPS:
	wavInfo->wFormatTag 		= WAVE_FORMAT_G721_ADPCM;
	wavInfo->nSamplesPerSec 	= 8000;
	wavInfo->wBitsPerSample   = 6;
	break;
 case kSMDataFormatG726_32KBPS:
	wavInfo->wFormatTag 		= WAVE_FORMAT_G721_ADPCM;
	wavInfo->nSamplesPerSec 	= 8000;
	wavInfo->wBitsPerSample   = 4;
	break;
 case kSMDataFormatG726_24KBPS:
	wavInfo->wFormatTag 		= WAVE_FORMAT_G721_ADPCM;
	wavInfo->nSamplesPerSec 	= 8000;
	wavInfo->wBitsPerSample   = 3;
	break;
 case kSMDataFormatG726_16KBPS:
	wavInfo->wFormatTag 		= WAVE_FORMAT_G721_ADPCM;
	wavInfo->nSamplesPerSec 	= 8000;
	wavInfo->wBitsPerSample   = 2;
	break;
#endif
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 wavInfo->nAvgBytesPerSec = wavInfo->nSamplesPerSec
				* wavInfo->wBitsPerSample / 8;
 wavInfo->nChannels = 1;
 if (efseek(fp, 0, SEEK_SET)) return err(ERR_SM_FILE_ACCESS);
 cpyfourcc(p, "RIFF"); p += 4;
 riff_size = p; p += 4;			// save pointer to RIFF length
 cpyfourcc(p, "WAVE"); p += 4;
		// the LIST chunk
 cpyfourcc(p, "LIST"); p += 4;
 list_size = p; p += 4;			// save pointer to LIST length
 cpyfourcc(p, "INFO"); p += 4;			// kind of LIST
 cpyfourcc(p, "ISFT"); p += 4;			// software generator
 cpy32(p, sizeof(generator)); p += 4;		// length already known
 memcpy(p, generator, sizeof(generator)); p += sizeof(generator);
 if (sizeof(generator) & 1) *p++ = 0;	// padding, if necessary
 cpy32(list_size, p - list_size - 4);	// fill in LIST length
 		// the fmt chunk
 cpyfourcc(p, "fmt "); p += 4;
 fmt_size = p; p += 4;			// save pointer to fmt length
 cpy16(p, wavInfo->wFormatTag); p += 2;
 cpy16(p, wavInfo->nChannels); p += 2;
 cpy32(p, wavInfo->nSamplesPerSec); p += 4;
 cpy32(p, wavInfo->nAvgBytesPerSec); p += 4;
 cpy16(p, wavInfo->nBlockAlign); p += 2;
 cpy16(p, wavInfo->wBitsPerSample); p += 2;
 if (wavInfo->wFormatTag == WAVE_FORMAT_IMA_ADPCM) {
	int wSamplesPerBlock = (wavInfo->nBlockAlign - 4) * 8
		/ wavInfo->wBitsPerSample + 1;
	cpy16(p, 2); p += 2;
	cpy16(p, wSamplesPerBlock); p += 2;
 } else {
#ifndef OMIT_CBSIZE
	cpy16(p, 0); p += 2;
#endif
 }
 cpy32(fmt_size, p - fmt_size - 4);	// fill in fmt length
 		// the fact chunk
 cpyfourcc(p, "fact"); p += 4;
 cpy32(p, 4); p += 4;			// size of fact
 fact_pos = p; p += 4;
		// the data chunk
		// (just header - actual content is written elsewhere)
 cpyfourcc(p, "data"); p += 4;
 if (wavInfo->data_size) {
		// subtract size of riff header so far
	 wavInfo->data_size -= p - riff_size - 4;
		// subtract size of RIFF header and this length field
	 wavInfo->data_size -= 8 + 4;
 }
 cpy32(p, wavInfo->data_size); p += 4;
 cpy32(fact_pos, wavInfo->data_size * 8 / wavInfo->wBitsPerSample);
 cpy32(riff_size, (p - riff_size) - 4 + wavInfo->data_size);
 if (efwrite(hdr, p - hdr, 1, fp) != 1) return err(ERR_SM_FILE_ACCESS);
 return 0;
}

STATIC int real_sm_record_wav_start( char *filename, struct sm_file_record_parms* file_parms )
{
 struct wav_info wavInfo;
 int err = 0;
 file_parms->status = 0;
 file_parms->fd = efopen(filename, "wb");
 if (!file_parms->fd) return file_parms->status = err(ERR_SM_FILE_ACCESS);
 wavInfo.data_size = 0;
	 // The header will become valid when sm_record_wav_close is called.
 wavInfo.wFormatTag = WAVE_FORMAT_UNKNOWN; 
 wavInfo.nBlockAlign = 1;
 wavInfo.nSamplesPerSec = file_parms->record_parms.sampling_rate;
 err = wav_dump_header(&wavInfo, file_parms->record_parms.type, file_parms->fd);
 if (!err) err = sm_record_file_start(file_parms);
 if (err) efclose(file_parms->fd);
 return err;
}

STATIC int real_sm_record_wav_trim_close( struct sm_file_record_parms *file_parms, tSM_UT32 trimLength )
{
 struct wav_info wavInfo;
 long pos;
 int rc;
 file_parms->status = 0;
 wavInfo.nSamplesPerSec = file_parms->record_parms.sampling_rate;
 pos = eftell(file_parms->fd);
 if (pos == -1) file_parms->status = err(ERR_SM_FILE_ACCESS);
 wavInfo.data_size = pos;	// modified by wav_dump_header
 if (trimLength > wavInfo.data_size) trimLength = wavInfo.data_size;
 wavInfo.data_size -= trimLength;
 if (!file_parms->status) {
	file_parms->status = wav_dump_header(&wavInfo, file_parms->record_parms.type, file_parms->fd);
 }
 rc = efclose(file_parms->fd);
 if (rc && !file_parms->status) file_parms->status = err(ERR_SM_FILE_ACCESS);
 return file_parms->status;
}

STATIC int real_sm_record_wav_close( struct sm_file_record_parms *file_parms )
{
 return real_sm_record_wav_trim_close(file_parms, 0);
}

#include "../apilib/timestamp.h"
#include "../pubdoc/gen/highapi_wavfile.i"
