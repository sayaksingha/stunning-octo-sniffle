#include <stdlib.h>

#include "v18a.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static char ltrs[32] = "\010E\nA SIU\rDRJNFCKTZLWHYPQOBG_MXV^";
static char figs[32] = "\0103\n- \00087\r$4',!:(5\")2=6019?+_./;^";

static const unsigned V18A_FIGS = 0x1b;
static const unsigned V18A_LTRS = 0x1f;

#define V18A_NULL 0
#define V18A_ISFIGS 0x20
#define V18A_ISLTRS 0x40

static unsigned char fromT50[256] = {
	V18A_NULL,			// 0    NUL '\0'       
	V18A_NULL,			// 1    SOH            
	V18A_NULL,			// 2    STX            
	V18A_NULL,			// 3    ETX            
	V18A_NULL,			// 4    EOT            
	V18A_NULL,			// 5    ENQ            
	V18A_NULL,			// 6    ACK            
	V18A_NULL,			// 7    BEL '\a'       
	V18A_ISLTRS | V18A_ISFIGS | 0x00,		// 8    BS  '\b'       
	V18A_ISLTRS | V18A_ISFIGS | 0x04,		// 9    HT  '\t'       
	V18A_ISLTRS | V18A_ISFIGS | 0x02,			// 10    LF  '\n'       
	V18A_ISLTRS | V18A_ISFIGS | 0x02,			// 11    VT  '\v'       
	V18A_ISLTRS | V18A_ISFIGS | 0x02,			// 12    FF  '\f'       
	V18A_ISLTRS | V18A_ISFIGS | 0x08,			// 13    CR  '\r'       
	V18A_NULL,			// 14    SO             
	V18A_NULL,			// 15    SI             
	V18A_NULL,			// 16    DLE            
	V18A_NULL,			// 17    DC1            
	V18A_NULL,			// 18    DC2            
	V18A_NULL,			// 19    DC3            
	V18A_NULL,			// 20    DC4            
	V18A_NULL,			// 21    NAK            
	V18A_NULL,			// 22    SYN            
	V18A_NULL,			// 23    ETB            
	V18A_NULL,			// 24    CAN            
	V18A_NULL,			// 25    EM             
	V18A_ISFIGS | 0x19,			// 26    SUB            
	V18A_NULL,			// 27    ESC            
	V18A_ISLTRS | V18A_ISFIGS | 0x02,			// 28    FS             
	V18A_ISLTRS | V18A_ISFIGS | 0x02,			// 29    GS             
	V18A_ISLTRS | V18A_ISFIGS | 0x02,			// 30    RS             
	V18A_ISLTRS | V18A_ISFIGS | 0x04,			// 31    US             
	V18A_ISLTRS | V18A_ISFIGS | 0x04,			// 32    SPACE          
	V18A_ISFIGS | 0x0d,			// 33    !              
	V18A_ISFIGS | 0x11,			// 34    "              
	V18A_ISFIGS | 0x09,			// 35    #              
	V18A_ISFIGS | 0x09,			// 36    $              
	V18A_ISFIGS | 0x1d,			// 37    %              
	V18A_ISFIGS | 0x1a,			// 38    &              
	V18A_ISFIGS | 0x0b,			// 39    '              
	V18A_ISFIGS | 0x0f,			// 40    (              
	V18A_ISFIGS | 0x12,			// 41    )              
	V18A_ISFIGS | 0x1c,			// 42    *              
	V18A_ISFIGS | 0x1a,			// 43    +              
	V18A_ISFIGS | 0x0c,			// 44    ,              
	V18A_ISFIGS | 0x03,			// 45    -              
	V18A_ISFIGS | 0x1c,			// 46    .              
	V18A_ISFIGS | 0x1d,			// 47    /              
	V18A_ISFIGS | 0x16,			// 48    0              
	V18A_ISFIGS | 0x17,			// 49    1              
	V18A_ISFIGS | 0x13,			// 50    2              
	V18A_ISFIGS | 0x01,			// 51    3              
	V18A_ISFIGS | 0x0a,			// 52    4              
	V18A_ISFIGS | 0x10,			// 53    5              
	V18A_ISFIGS | 0x15,			// 54    6              
	V18A_ISFIGS | 0x07,			// 55    7              
	V18A_ISFIGS | 0x06,			// 56    8              
	V18A_ISFIGS | 0x18,			// 57    9              
	V18A_ISFIGS | 0x0e,			// 58    :              
	V18A_ISFIGS | 0x1e,			// 59    ;              
	V18A_ISFIGS | 0x0f,			// 60    <              
	V18A_ISFIGS | 0x14,			// 61    =              
	V18A_ISFIGS | 0x12,			// 62    >              
	V18A_ISFIGS | 0x19,			// 63    ?              
	V18A_ISLTRS | 0x1d,			// 64    @
	V18A_ISLTRS | 0x03,			// 65    A
	V18A_ISLTRS | 0x19,			// 66    B
	V18A_ISLTRS | 0x0e,			// 67    C
	V18A_ISLTRS | 0x09,			// 68    D
	V18A_ISLTRS | 0x01,			// 69    E
	V18A_ISLTRS | 0x0d,			// 70    F
	V18A_ISLTRS | 0x1a,			// 71    G
	V18A_ISLTRS | 0x14,			// 72    H
	V18A_ISLTRS | 0x06,			// 73    I
	V18A_ISLTRS | 0x0b,			// 74    J
	V18A_ISLTRS | 0x0f,			// 75    K
	V18A_ISLTRS | 0x12,			// 76    L
	V18A_ISLTRS | 0x1c,			// 77    M
	V18A_ISLTRS | 0x0c,			// 78    N
	V18A_ISLTRS | 0x18,			// 79    O
	V18A_ISLTRS | 0x16,			// 80    P
	V18A_ISLTRS | 0x17,			// 81    Q
	V18A_ISLTRS | 0x0a,			// 82    R
	V18A_ISLTRS | 0x05,			// 83    S
	V18A_ISLTRS | 0x10,			// 84    T
	V18A_ISLTRS | 0x07,			// 85    U
	V18A_ISLTRS | 0x1e,			// 86    V
	V18A_ISLTRS | 0x13,			// 87    W
	V18A_ISLTRS | 0x1d,			// 88    X
	V18A_ISLTRS | 0x15,			// 89    Y
	V18A_ISLTRS | 0x11,			// 90    Z
	V18A_ISFIGS | 0x0f,			// 91    [
	V18A_ISFIGS | 0x1d,			// 92    \   '\\'
	V18A_ISFIGS | 0x12,			// 93    ]
	V18A_ISFIGS | 0x0b,			// 94    ^
	V18A_ISLTRS | V18A_ISFIGS | 0x04,			// 95    _
	V18A_ISFIGS | 0x0b,			// 96    `
	V18A_ISLTRS | 0x03,			// 97    a
	V18A_ISLTRS | 0x19,			// 98    b
	V18A_ISLTRS | 0x0e,			// 99    c
	V18A_ISLTRS | 0x09,			// 100    d
	V18A_ISLTRS | 0x01,			// 101    e
	V18A_ISLTRS | 0x0d,			// 102    f
	V18A_ISLTRS | 0x1a,			// 103    g
	V18A_ISLTRS | 0x14,			// 104    h
	V18A_ISLTRS | 0x06,			// 105    i
	V18A_ISLTRS | 0x0b,			// 106    j
	V18A_ISLTRS | 0x0f,			// 107    k
	V18A_ISLTRS | 0x12,			// 108    l
	V18A_ISLTRS | 0x1c,			// 109    m
	V18A_ISLTRS | 0x0c,			// 110    n
	V18A_ISLTRS | 0x18,			// 111    o
	V18A_ISLTRS | 0x16,			// 112    p
	V18A_ISLTRS | 0x17,			// 113    q
	V18A_ISLTRS | 0x0a,			// 114    r
	V18A_ISLTRS | 0x05,			// 115    s
	V18A_ISLTRS | 0x10,			// 116    t
	V18A_ISLTRS | 0x07,			// 117    u
	V18A_ISLTRS | 0x1e,			// 118    v
	V18A_ISLTRS | 0x13,			// 119    w
	V18A_ISLTRS | 0x1d,			// 120    x
	V18A_ISLTRS | 0x15,			// 121    y
	V18A_ISLTRS | 0x11,			// 122    z
	V18A_ISFIGS | 0x0f,			// 123    {
	V18A_ISFIGS | 0x0d,			// 124    |
	V18A_ISFIGS | 0x12,			// 125    }
	V18A_ISLTRS | V18A_ISFIGS | 0x04,			// 126    ~
	V18A_NULL,			// 127    DEL
};

void v18astate(V18ASTATE *ts)
{
 ts->mode = V18A_ISLTRS;
 ts->nsame = 72;		// provoke immediate LTRS code
}

char *v18a_fromT50(char *os, char *oe, V18ASTATE *ts, unsigned char **ibuf, unsigned char *ie)
{
 while (os + 1 < oe) {
	unsigned char c;
	if (*ibuf >= ie) break;
	c = **ibuf;
	if (c & ~0x7f) break;
	++*ibuf;
	c = fromT50[c];
	if (c != V18A_NULL) {
		unsigned char t = c & (V18A_ISLTRS | V18A_ISFIGS);
		if (! (t & ts->mode) || ts->nsame >= 72) {
			ts->nsame = 0;
			if (t & V18A_ISLTRS) {
				*os++ = V18A_LTRS;
				ts->mode = V18A_ISLTRS;
			} else {
				*os++ = V18A_FIGS;
				ts->mode = V18A_ISFIGS;
			}
		}
		*os++ = c;
		ts->nsame++;
	}
 }
 return os;
}

char *v18a_toT50(char *os, char *oe, V18ASTATE *ts, unsigned char **ibuf, unsigned char *ie)
{
 while (os < oe) {
 	unsigned char c;
 	if (*ibuf >= ie) break;
 	c = **ibuf;
 	if (c >= arlen(ltrs)) break;
 	++*ibuf;
 	if (c == V18A_FIGS) {
 		ts->mode = V18A_ISFIGS;
 		ts->nsame = 0;
 	} else if (c == V18A_LTRS) {
 		ts->mode = V18A_ISLTRS;
 		ts->nsame = 0;
 	} else {
		if (ts->mode & V18A_ISLTRS) c = ltrs[c];
		else c = figs[c];
		*os++ = c;
		ts->nsame++;
	}
 }
 return os;
}

static void ofndec(void (*ofn)(void *p, char c), void *p, unsigned d)
{
 unsigned t = d / 10;
 if (t) {
	ofndec(ofn, p, t);
	d -= t * 10;
 }
 ofn(p, d + '0');
}

void v18a_show(void (*ofn)(void *p, char c), void *p, V18ASTATE *ts, unsigned char c)
{
 static const char hex[] = "0123456789abcdef";
 if (c >= arlen(ltrs)) {
	ofn(p, '[');
	ofn(p, hex[(c >> 4) & 0xf]);
	ofn(p, hex[c & 0xf]);
	ofn(p, ']');
 } else {
	if (c == V18A_LTRS) {
		ofn(p, '[');
		ofn(p, 'L');
		if (ts->mode == V18A_ISLTRS) {
			ofn(p, '+');
			ofndec(ofn, p, ts->nsame);
		}
		ofn(p, ']');
		ts->mode = V18A_ISLTRS;
		ts->nsame = 0;
	} else if (c == V18A_FIGS) {
		ofn(p, '[');
		ofn(p, 'N');
		if (ts->mode == V18A_ISFIGS) {
			ofn(p, '+');
			ofndec(ofn, p, ts->nsame);
		}
		ofn(p, ']');
		ts->mode = V18A_ISFIGS;
		ts->nsame = 0;
	} else {
		if (ts->mode == V18A_ISLTRS) c = ltrs[c];
		else c = figs[c];
		if (c == ' ') c = '_';
		else if (c < ' ') {
			ofn(p, '^');
			c += 'A'-1;
		}
		ofn(p,  c);
		ts->nsame++;
	}
 }
}

#ifdef CHECK_V18A
	/* code to cross check the tables
	 */
int main(int argc, char **argv)
{
 unsigned u;
 for (u=0; u < 0x20; u++) {
 	int c = ltrs[u];
 	int d = fromT50[c];
 	if ((d & 0x1f) != u || !(d & V18A_ISLTRS)) {
		printf("v18 L%02x -> a %02x (%c) -> v18 %s%s%02x\n",
			u,
			c,
			c >= ' ' && c < 127 ? c : '*',
			d & V18A_ISLTRS ? "L" : "",
			d & V18A_ISFIGS ? "F" : "",
			d & 0x1f);
 	}
 }
 for (u=0; u < 0x20; u++) {
 	int c = figs[u];
 	int d = fromT50[c];
 	if ((d & 0x1f) != u || !(d & V18A_ISFIGS)) {
		printf("v18 F%02x -> a %02x (%c) -> v18 %s%s%02x\n",
			u,
			c,
			c >= ' ' && c < 127 ? c : '*',
			d & V18A_ISLTRS ? "L" : "",
			d & V18A_ISFIGS ? "F" : "",
			d & 0x1f);
 	}
 }
 for (u=0; u < 128; u++) {
	int c = fromT50[u];
	if (c & V18A_ISLTRS) {
		int d = ltrs[c & 0x1f];
		if (u >= 'a' && u <= 'z' && d >= 'A' && d <= 'Z') d += 'a'-'A';
		if (d != u) {
			printf("a %02x (%c) -> v18 L%02x -> a %02x (%c)\n",
				u,
				u >= ' ' && u < 127 ? u : '*',
				c & 0x1f,
				d,
				d >= ' ' && d < 127 ? d : '*');
		}
	}
	if (c & V18A_ISFIGS) {
		int d = figs[c & 0x1f];
		if (d != u) {
			printf("a %02x (%c) -> v18 F%02x -> a %02x (%c)\n",
				u,
				u >= ' ' && u < 127 ? u : '*',
				c & 0x1f,
				d,
				d >= ' ' && d < 127 ? d : '*');
		}
	}
 }
}
#endif
