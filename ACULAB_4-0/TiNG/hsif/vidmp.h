#ifndef TiNGK_INCLUDED_HSIF_VIDMP_H
#define TiNGK_INCLUDED_HSIF_VIDMP_H

#define MSG_RFC3984ENC_CFG 0xcff
	/*
	 * RFC3984ENC_CFG(repeat_ps, deaggregate)
	 * h->s only
	 */

#define MSG_RFC3984DEC_CFG 0xd01
	/*
	 * RFC3984DEC_CFG(picture_width, picture_height)
	 * h->s only
	 */

#define MSG_RFC4629ENC_CFG 0xd00
	/*
	 * RFC4629ENC_CFG(rem_ext_pic_hdr)
	 * h->s only
	 */

#define D_H263_LEVEL 10
#define MSG_RFC4629DEC_CFG 0xd02
	/*
	 * RFC4629DEC_CFG(h263_profile, h263_level)
	 * h->s only
	 */

#endif
