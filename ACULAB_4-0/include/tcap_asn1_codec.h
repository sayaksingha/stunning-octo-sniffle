/* "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/src/tcap/asn1/tcap_asn1_codec.h,v 1.62 2013/09/18 15:30:08 davidla Exp $ $Name: SS7_6_16_1 $" */

#ifndef TCAP_ASN1_CODEC_H
#define TCAP_ASN1_CODEC_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Define an appropiate prefix for library function definitions,
 * in order to exclude internal functions from the library symbol table
 * and to ensure that the library calls its own public functions.
 *
 * The library itself is compiled with -fvisibility=hidden and
 * -DACU_TCAP_LIB_API=__attribute__((visibility("protected")))
 */

#ifndef ACU_TCAP_LIB_API
#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100) >= 30400
#define ACU_TCAP_LIB_API __attribute__((visibility("default")))
#else
#define ACU_TCAP_LIB_API
#endif
#endif


#define ACU_ASN1_TAG_UNIVERSAL      0x00
#define ACU_ASN1_TAG_APPLICATION    0x40
#define ACU_ASN1_TAG_CONTEXT        0x80
#define ACU_ASN1_TAG_PRIVATE        0xc0
#define ACU_ASN1_TAG_CONSTRUCTED    0x20
#define ACU_ASN1_TAG_CODE           0x1f

#define ACU_ASN1_TAG_CODE_CONTINUED 0x80

/* Universal tag values */
#define ACU_ASN1_BOOLEAN            0x01
#define ACU_ASN1_INT                0x02
#define ACU_ASN1_BITSTRING          0x03
#define ACU_ASN1_OCTETSTRING        0x04
#define ACU_ASN1_NULL               0x05
#define ACU_ASN1_OBJ_ID             0x06
#define ACU_ASN1_OBJ_DESC           0x07
#define ACU_ASN1_EXTERNAL           0x08
#define ACU_ASN1_REAL               0x09
#define ACU_ASN1_ENUMERATED         0x0a
#define ACU_ASN1_SEQ                0x10
#define ACU_ASN1_SET                0x11
#define ACU_ASN1_CHARSTRING         0x12   /* All of 0x12-0x16 and 0x19-1b */
#define ACU_ASN1_TIME               0x17   /* also 0x18 */

/* We encode ASN.1 tags into a 32bit value with the the first byte in the
 * least significant 8 bits and the entire code in the top 24.
 */

#define ACU_ASN1_MAKE_TAG(clf, cd) \
    ((cd) << 8 | (clf) | ((cd) < ACU_ASN1_TAG_CODE ? (cd) : ACU_ASN1_TAG_CODE))

/* Defines to pull apart 'tag' values */
/* Get the 'code' part of an asn.1 tag value (as encoded above) */
#define ACU_ASN1_GET_TAG_CODE(tag) ((tag) >> 8)
/* Get the 'class' (universal, application, context or private) bits */
#define ACU_ASN1_GET_TAG_CLASS(tag) ((tag) & 0xc0)
/* Get the 'constructed' flag */
#define ACU_ASN1_GET_TAG_CONSTRUCTED(tag) ((tag) & ACU_ASN1_TAG_CONSTRUCTED)

/* Space saving definitions for primitive tags */
#define ACU_ASN1_UNI(code) ACU_ASN1_MAKE_TAG(ACU_ASN1_TAG_UNIVERSAL, (code))
#define ACU_ASN1_APP(code) ACU_ASN1_MAKE_TAG(ACU_ASN1_TAG_APPLICATION, (code))
#define ACU_ASN1_CTX(code) ACU_ASN1_MAKE_TAG(ACU_ASN1_TAG_CONTEXT, (code))
#define ACU_ASN1_PRV(code) ACU_ASN1_MAKE_TAG(ACU_ASN1_TAG_PRIVATE, (code))

/* Space saving definitions for constructed tags */
#define ACU_ASN1_UNI_CONS(code) (ACU_ASN1_UNI(code) | ACU_ASN1_TAG_CONSTRUCTED)
#define ACU_ASN1_APP_CONS(code) (ACU_ASN1_APP(code) | ACU_ASN1_TAG_CONSTRUCTED)
#define ACU_ASN1_CTX_CONS(code) (ACU_ASN1_CTX(code) | ACU_ASN1_TAG_CONSTRUCTED)
#define ACU_ASN1_PRV_CONS(code) (ACU_ASN1_PRV(code) | ACU_ASN1_TAG_CONSTRUCTED)


/* Negative error codes, add new values at the top! */
#define ACU_ASN1_ERRORS(x) \
x(ACU_ASN1_ERROR_NOT_FOUND,          "Item not found") \
x(ACU_ASN1_ERROR_DEFN_MISMATCH,      "Definition doesn't match values") \
x(ACU_ASN1_ERROR_DEFN_ERROR,         "Definition doesn't match message") \
x(ACU_ASN1_ERROR_FIELD_UNEXPECTED,   "Field not in ASN.1 definition") \
x(ACU_ASN1_ERROR_FIELD_MISSING,      "Mandatory field absent") \
x(ACU_ASN1_ERROR_TRUNCATED,          "Buffer too short") \
x(ACU_ASN1_ERROR_BAD_MAGIC,          "Invalid magic number") \
x(ACU_ASN1_ERROR_NOMEMORY,           "Malloc/realloc failed") \
x(ACU_ASN1_ERROR_NOTCONSTRUCTED,     "Not inside constructed item") \
x(ACU_ASN1_ERROR_BAD_CODE,           "Code exceeds buffer length") \
x(ACU_ASN1_ERROR_BAD_LENGTH,         "Invalid length field") \
x(ACU_ASN1_ERROR_BAD_DATA,           "Invalid data for tag/code") \
x(ACU_ASN1_ERROR_TOOCONSTRUCTED,     "Too many level of construction") \
x(ACU_ASN1_ERROR_TOOLONG,            "Data to encode too long") \
x(ACU_ASN1_ERROR_BAD_TAG,            "Invalid tag value") \
x(ACU_ASN1_ERROR_SUCCESS,            "Success")    /* Zero by design */

#define ACU_ASN1_ERROR_ENUM(name, text) name,
#define ACU_ASN1_ERROR_COUNT(name, text) -1
enum {
    ACU_ASN1_ERROR_HIGHEST = ACU_ASN1_ERRORS(ACU_ASN1_ERROR_COUNT),
    ACU_ASN1_ERRORS(ACU_ASN1_ERROR_ENUM)
    ACU_ASN1_ERROR_NUM_ERRMSG = - ACU_ASN1_ERROR_HIGHEST
};
#undef ACU_ASN1_ERROR_ENUM
#undef ACU_ASN1_ERROR_COUNT

#define ACU_ASN1_CMAX                (1<<4)  /* Max we keep offsets for */
#define ACU_ASN1_INDEFINITE_LENGTH   (~0u)
#define ACU_ASN1_MAGIC               ('A' << 24 | 'S' << 16 | 'N' << 8 | '1')

/* For acu_asn1_get/set_bits32() 'bit 0' is 0x80 thru 'bit 7' being 0x00,
 * 'bit 8' is 0x8000 etc. This define does the convertion. */
#define ACU_ASN1_BIT32_VALUE(n) ((1 << (7 - ((n) & 7))) << ((n) & 0x18))

/* Mask of all bits 'n' and below, zero for 'n' outside 0..31 */
#define ACU_ASN1_BIT32_MASK(n) \
   ((n) & ~0x1fu ? 0 : 0xffffffu >> (24 - ((n) & 0x18)) \
       | (0x7f80 >> ((n) & 7) & 0xff) << ((n) & 0x18))

/* State information for ASN.1 BER encoder/decoder.
 * The contents are exposed to applications, but they should treat the
 * contents as readonly.
 * Only a1b_buf, a1b_ptr and the error fields are likely to be useful.
 * Applications must never directly allocate the structure.  One of the
 * correct size (for the current version of the library) should be allocated
 * by calling acu_asn1_buf_init(NULL, ...).
 */
typedef struct {
    unsigned char  *a1b_buf;               /* Base of encode/decode buffer */
    unsigned char  *a1b_end;               /* End of buffer */
    unsigned char  *a1b_ptr;               /* Current read/write postion */
    unsigned int   a1b_flags;
#define ACU_A1BF_REINIT           (1u<<0)  /* Re-initialise structure */
#define ACU_A1BF_LOG_STDERR       (1u<<1)  /* Log errors directly to stderr */
#define ACU_A1BF_DEFINITE_LEN     (1u<<2)  /* Use definite length constructed */
#define ACU_A1BF_A1B_MALLOCED     (1u<<8)  /* This struct malloced */
#define ACU_A1BF_BUF_MALLOCED     (1u<<9)  /* Buffer allocated by malloc */
#define ACU_A1BF_ERR_STR_MALLOCED (1u<<10) /* a1b_error_str malloced */
#define ACU_A1BF_FIELD_ADDED      (1u<<15) /* Non-default field added */
#define ACU_A1BF_TRACE_FLAGS   (0xffu<<16) /* acu_asn1_trace_data() flags */
#define ACU_A1BF_TRACE            (1u<<16) /* acu_asn1_encode_data() traces */
#define ACU_A1BF_TRACE_HEX        (1u<<17) /* trace with full hexdumps */
#define ACU_A1BF_TRACE_UNSET      (1u<<18) /* trace unset fields */
#define ACU_A1BF_TRACE_SEQS       (1u<<19) /* trace all SEQ */
#define ACU_A1BF_TRACE_ENDS       (1u<<20) /* trace end markers */
#define ACU_A1BF_TRACE_NO_REF     (1u<<21) /* don't trace REF/IMPLICIT */
#define ACU_A1BF_TRACE_RAW        (1u<<22) /* hex values of some fields */
/* Saved info about the first error detected */
    unsigned int   a1b_error;              /* First error detected */
    unsigned int   a1b_error_offset;       /* Byte offset of error */
    const char     *a1b_error_function;    /* Function that detected error */
    const char     *a1b_error_format;      /* printf style format string */
    int            a1b_error_val_1;        /* First argument to error format */
    int            a1b_error_val_2;        /* 2nd argument to error format */
    const char     *a1b_error_str;         /* 3rd argument to error format */
/* Applications must not reference fields below here, they may change
 * between releases. */
/* Private data for decoder */
    int            a1b_length;             /* Length of last decoded field */
    int            a1b_tag;                /* actual tag value */
    unsigned char  *a1b_code;              /* and pointer to its tag byte */
/* Private data for processing constructed items */
    unsigned int   a1b_constype;           /* Bitmap [in]def len for decoder */
    int            a1b_depth;              /* Constructed nesting depth */
    int            a1b_min_depth;          /* Lowest we have an offset for */
    int            a1b_cons_offset[ACU_ASN1_CMAX]; /* To cons. length bytes */
    unsigned int   a1b_magic;              /* Structure validation constant */
} acu_asn1_buf_t;

/****************************************************************************/

/* Message definition based ASN.1 BER encoder/decoder */
/* See ss7/sample_code/tcap/asn1 for example definitions */

/* To use the API you don't need to use or understand most of the definitions.
 * here.
 *
 * The defines are used to initialise the data area that describes
 * an asn.1 encoding from the user supplied definition. In effect this
 * data is a program for the encoder/decoder functions.
 *
 * The application reads/writes data from/to a C structure that matches
 * the 'program', to ensure these do match they are generated from the
 * same tokens in the C source file using a lot of preprocessor 'magic'.
 *
 * For each ASN.1 item, the following must be defined:
 * 1) A #define that defines the ASN.1, eg:
 *       #define request_msg(x) \
 *           x(INTEGER, (command, CTX(0), 0)) \
 *           x(OCTETS, (address, OCTETSTRING, OPTIONAL)) \
 *           x(CHOICE, (info, OPTIONAL)) \
 *               x(INTEGER, (int_info, CTX(0), 0)) \
 *               x(OCTETS,  (buf_info, CTX(1), 0)) \
 *           x(END_CHOICE, (info, 0)) \
 *
 * 2) The C 'struct' for the data area.
 *    If the encapsulating SEQ/END_SEQ have been ommited (as above) use:
 *        ACU_A1T_SEQ_TYPE(request_msg);
 *    otherwise use:
 *        ACU_A1T_TYPE(request_msg);
 *
 * 3) The data area defining the message, this will normally be 'const'
 *    and may be 'static' defined as one of:
 *       const ACU_A1T_DEFN(request_msg);
 *       const ACU_A1T_SEQ_DEFN(request_msg);
 *       const ACU_A1T_SEQ_EXT_DEFN(request_msg);
 *       ACU_A1T_EXTERN_DEFN(request_msg);
 *    ACU_A1T_SEQ_EXT_DEFN() is equivalent to ACU_A1T_SEQ_DEFN() but
 *    sets the ACU_A1T_EXTENDABLE flag to allow extensions to the asn.1.
 *    ACU_A1T_EXTERN_DEFN() just generates an 'extern' statement.
 *
 * 4) An actual data item to contain the decoded data:
 *        struct request_msg request;
 *    If the program is itself script driven, the acu_asn1_find_defn()
 *    function can be used to locate the relevant member of 'struct request_msg'
 *    (and its definition) for a named field (or the nth occurrance in a
 *    SEQ_OF).
 *
 * Note that if you mis-spell anything, or miss out a definition, then
 * the compiler will generate a lot of error/warning messages.
 * Ignore all but the first error, the preprocessor output can be informative.
 * If you can, pipe the output of 'cc -E' through sed to break long lines.
 * For example (include in your normal -I <dir> and -D <token> parameters):
 *   cc -E foo.c | sed -e 's/\(}[^;]\)/\1\n    /g;s/\(;\)\(.\)/\1\n    \2/g'
 *
 * The encoder is usually called as:
 *     acu_asn1_buf_t *a1b = acu_asn1_buf_init(NULL, NULL, 0, 0);
 *     struct request_msg request;
 *     memset(&request, 0, sizeof request);
 *
 */    /* Write required values to members of 'request' */
/*     request.command.a1v_integer = command_number;
 *     request.address.a1v_data = &address;
 *     request.address.a1v_length = sizeof address;
 *
 */    /* For a choice, either of the 2 lines below to select the item */
/*     request.info_choice.a1v_data = "int_info";
 *     request.info_choice.a1v_integer = CTX(0);
 *     request.info.int_info.a1v_integer = info_value;
 *
 */    /* Fields can be located by name instead, eg: */
/*     const acu_a1t_defn_t *found_defn;
 *     acu_a1t_value_t *found_value;
 *     acu_asn1_find_defn(request_msg_defn, (void *)&request,
 *             "info", 0, &found_defn, &found_value);
 *     acu_asn1_find_defn(found_defn, found_value,
 *             "int_info", 0, &found_defn, &found_value);
 *     found_value[-1].a1v_integer = ACU_A1T_TAG(found_defn);
 *     found_value->a1v_integer = info_value;
 *
 *     rval = ACU_ASN1_ENCODE_DATA(a1b, request_msg, &request);
 */    /* ASN.1 data now a1b->a1b_base ... a1b->a1b_ptr */
/*
 * The decoder is usually called as:
 *     acu_asn1_buf_t *a1b = acu_asn1_buf_init(NULL, data_ptr, data_len, 0);
 *     struct request_msg request;
 *     rval = ACU_ASN1_DECODE_DATA(a1b, request_msg, &request);
 */    /* 'request' now contains the decodes values. */
/*
 * In either case, in rval is non-zero look at the error fields in the
 * 'a1b' structure (or call acu_asn1_fmt_errmsg()) for further information.
 */
/* The contents of 'request' (in the above) can be converted to a textual
 * form (useful for debug and to find the C structure field names) by
 * calling ACU_ASN1_TRACE_DATA(request_msg, &request, "line_header", flags);
 *
 * The "line_header" string is prepended to every output line.
 *
 * The following flags are defined for ACU_ASN1_TRACE_DATA():
 *     ACU_A1BF_TRACE_HEX    Full hexdump (not just first 16 bytes)
 *     ACU_A1BF_TRACE_UNSET  Trace unset fields
 *     ACU_A1BF_TRACE_SEQS   Trace all SEQ terms
 *     ACU_A1BF_TRACE_ENDS   Trace end markers
 *     ACU_A1BF_TRACE_NO_REF Don't trace REF/IMPLICIT, enables TRACE_SEQS
 *
 * ACU_A1BF_TRACE_UNSET is particularly useful when 'request' is all
 * zero bytes as all the C structure/union field names are output.
 *
 * ACU_A1BF_TRACE_NO_REF traces the entries for actual asn.1, these are
 * the fields that acu_asn1_find_defn() will locate for you.
 *
 * This function executes ACU_ASN1_ENCODE_DATA() with ACU_A1BF_TRACE set.
 * It will append any error from the encoder to the generated string.
 * NB: Remember to acu_asn1_free_trace_data() the returned pointer.
 */

#ifdef ACU_A1T_ANON_UNION         /* Predefine for anonymous unions (C++) */
#define ACU_A1T_ANON_UNION_(x)
#else
#define ACU_A1T_ANON_UNION_(x) x
#endif

#ifdef ACU_A1T_NAMED_STRUCTS      /* Predefine for named sub-structures */
#define ACU_A1T_NAMED_STRUCTS_(x) x
#else
#define ACU_A1T_NAMED_STRUCTS_(x)
#endif

/* Structure to hold the data for a field.
 * An additional field exists at the start of sequence and before choice items,
 * but nothing is defined for 'end'.
 * For CHOICE the fields are part of a union. */
typedef struct {
    const void           *a1v_data;       /* Pointer into data buffer */
    unsigned int         a1v_length;
    int                  a1v_integer;     /* Esp. for INTEGER and UNSIGNED */
} acu_a1t_value_t;

/************************************/

/* Expansions for ACU_A1T_TYPE() and ACU_A1T_DEFN()
 *
 * ACU_A1T_TYPE() generates the C 'struct' that the application uses.
 *
 * The x(INTEGER, (command, CTX(0), 0)) from the example above becomes
 * ACU_A1T_TYPE_INTEGER(command, CTX(0), 0) before being expanded here.
 *
 * The 'field' parameter is used for the C structure member and error
 * messages. Most of the other parameters are ignored by ACU_A1T_TYPE().
 *
 * ACU_A1T_DEFN() generates the data structure that defines the asn.1.
 *
 * The 'tag' has ACU_ASN1_ prepended before final expansion and can
 * be either a 'Universal' type (eg OCTETSTRING) or CTX(n), APP(n) or
 * PRV(n) for 'Context specific', 'Application' or 'Private' respectively.
 * Most tags are either universal or context specific.
 * The 'Constructed' bit is added when required.
 * NB: Use UNI_NULL instead of NULL as the latter gets expanded to (void *)0.
 *
 * The 'flags' will be either '0' or 'OPTIONAL', except for SEQ_END
 * where it may be EXTENDABLE.
 */

/* Simple items are just a single value in the current structure */
#define ACU_A1T_TYPE_INTEGER(field, tag, flags)       acu_a1t_value_t field;
#define ACU_A1T_TYPE_UNSIGNED(field, tag, flags)      acu_a1t_value_t field;
#define ACU_A1T_TYPE_BOOLEAN(field, tag, flags)       acu_a1t_value_t field;
#define ACU_A1T_TYPE_NULL(field, tag, flags)          acu_a1t_value_t field;
#define ACU_A1T_TYPE_BITS32(field, tag, width, flags) acu_a1t_value_t field;
#define ACU_A1T_TYPE_OCTETS(field, tag, flags)        acu_a1t_value_t field;

/* RAW is like OCTETS, but the 'constructed' bit is set */
#define ACU_A1T_TYPE_RAW(field, tag, flags)           acu_a1t_value_t field;

/* ANY is a single piece of ASN.1 including its tag+length bytes */
#define ACU_A1T_TYPE_ANY(field, flags)                acu_a1t_value_t field;

/* REF and IMPLICIT let us refer to another definition.
 * They differ only in where the 'tag' comes from.
 * In both cases we embed the referenced structure */
#define ACU_A1T_TYPE_REF(field, ref, flags)           struct ref field;
#define ACU_A1T_TYPE_IMPLICIT(field, tag, ref, flags) struct ref field;

/* DIR_REF and DIR_IMPL are direct versions of REF and IMPLICIT that
 * encode the data from the referenced type (rather than a reference to it).
 * Thus avoiding a level of C 'struct'.
 * The referenced type must be one of the simple types above,
 * and, for DIR_REF, not ANY or BITS32. */
#define ACU_A1T_TYPE_DIR_REF(field, ref, flags)       acu_a1t_value_t field;
#define ACU_A1T_TYPE_DIR_IMPL(field, tag, ref, flags) acu_a1t_value_t field;

/* SEQ is a nested struct with an initial field describing the whole item */
#define ACU_A1T_TYPE_SEQ(field, tag, flags) \
        struct ACU_A1T_NAMED_STRUCTS_(field) { \
            acu_a1t_value_t field##_seq;  /* Filled with ptr+len of sequence */ \
            /* Fields for the members get added here */

#define ACU_A1T_TYPE_END_SEQ(field, flags) \
        } field;

/* SEQ_OF is the same as SEQ - but is an array with the maximum size */
#define ACU_A1T_TYPE_SEQ_OF(field, tag, flags) \
        ACU_A1T_TYPE_SEQ(field, tag, flags)

#define ACU_A1T_TYPE_END_SEQ_OF(field, count, flags) \
        } field[count];

/* Choice is a field defining the selected member, followed by a union */
#define ACU_A1T_TYPE_CHOICE(field, flags) \
        acu_a1t_value_t field##_choice; \
        union ACU_A1T_NAMED_STRUCTS_(field) { \
            /* Fields for the choices get added here */

#define ACU_A1T_TYPE_END_CHOICE(field, flags) \
        } ACU_A1T_ANON_UNION_(field);

/************************************/

/* Expansions for ACU_A1T_DEFN()
 *
 * These expansions need not be understood to use the API.
 */

/* The structure below defines a single ASN.1 field.
 * Every definition line generates exactly one of these.
 * (The application code should never need to look at this structure.) */
typedef struct acu_a1t_defn acu_a1t_defn_t;
struct acu_a1t_defn {
    unsigned char        a1d_type;      /* Definition type (see below) */
    unsigned char        a1d_flags;
    unsigned char        a1d_val1;
    unsigned char        a1d_val2;
    unsigned int         a1d_tag;       /* Tag value for field */
    unsigned int         a1d_count;     /* Also entry count of REF/IMPLICIT */
    const acu_a1t_defn_t *a1d_defn;     /* For REF and IMPLICIT */
    const char           *a1d_name;     /* Field name for error messages */
};

#define ACU_A1T_TAG(defn) ((defn)->a1d_type == ACU_A1T_REF ? (defn)->a1d_defn->a1d_tag : (defn)->a1d_tag)

/* Definitions of the type of an ASN.1 field (for a1d_type) */
#define ACU_A1T_ENDS       0x00   /* All terminators */
#define ACU_A1T_END_DEF    0x00   /* End of message definition */
#define ACU_A1T_END_SEQ    0x01   /* Of SEQ(UENCE) */
#define ACU_A1T_END_SEQ_OF 0x02   /* Of SEQ_OF (with repeat bounds) */
#define ACU_A1T_END_CHOICE 0x03   /* Of CHOICE */
#define ACU_A1T_END_MAX    0x0f   /* Largest value that is an END */

#define ACU_A1T_LISTS      0x10   /* All constructed/choice items */
                                  /* Terminate with corresponding END */
#define ACU_A1T_SEQ        0x10   /* Constructed tag for following items */
#define ACU_A1T_SEQ_OF     0x11   /* Sequence of following type */
#define ACU_A1T_CHOICE     0x12   /* Choice of following items */

#define ACU_A1T_INDIRECT   0x20   /* All indirect items */
#define ACU_A1T_REF        0x20   /* Reference to another acu_a1t_defn[] */
#define ACU_A1T_IMPLICIT   0x21   /* Ref to an acu_a1t_defn[] but with tag */

#define ACU_A1T_PRIMITIVE  0x40   /* All primitive items */
#define ACU_A1T_INTEGER    0x40   /* Signed integer (max 32 bits) */
#define ACU_A1T_UNSIGNED   0x41   /* Unsigned 1-4 bytes from integer value */
#define ACU_A1T_OCTETS     0x42   /* Any data from pointer+length */
#define ACU_A1T_BOOLEAN    0x43   /* Single byte 0, or 1 */
#define ACU_A1T_NULL       0x44   /* Zero length item */
#define ACU_A1T_BITS32     0x45   /* BITSTRING, max 32 bits */
#define ACU_A1T_ANY        0x4e   /* Any ASN.1 type (data includes outer tag) */
#define ACU_A1T_RAW        0x4f   /* Embedded pre-built ASN.1 */

/* a1d_flags values */
#define ACU_A1T_0             0
#define ACU_A1T_OPTIONAL      1
#define ACU_A1T_EXTENDABLE    2   /* In END_SEQ to ignore extra fields */
#define ACU_A1T_NO_STRUCT     4   /* In SEQ/END_SEQ when no extra C struct  */

/* Generate data structure */
#define ACU_A1T_TYPE(msg_name) \
        struct msg_name { \
            msg_name(ACU_A1T_TYPE_) \
        }

/* Generate message definition */
#define ACU_A1T_DEFN(msg_name) \
        acu_a1t_defn_t msg_name##_defn[] = { \
            msg_name(ACU_A1T_DEFN_) \
            { ACU_A1T_END_DEF, 0xff, 0, 0, 0, 0, NULL, "end_of_" #msg_name} \
        }

/* Equivalent definitions for a SEQ that avoids a level of 'struct' */
#define ACU_A1T_SEQ_TYPE(msg_name) \
        struct msg_name { \
            acu_a1t_value_t msg_name##_seq; \
            msg_name(ACU_A1T_TYPE_) \
        }

#define ACU_A1T_SEQ_DEFN_FL(msg_name, end_flags) \
        acu_a1t_defn_t msg_name##_defn[] = { \
            ACU_A1T_VAL(SEQ, msg_name, SEQ | ACU_ASN1_TAG_CONSTRUCTED, \
                    0, NULL, NO_STRUCT) \
            msg_name(ACU_A1T_DEFN_) \
            ACU_A1T_VAL(END_SEQ, msg_name, BAD(7), \
                    0, NULL, NO_STRUCT | end_flags) \
            { ACU_A1T_END_DEF, 0xff, 0, 0, 0, 0, NULL, "end_of_" #msg_name} \
        }
#define ACU_A1T_SEQ_DEFN(msg_name) ACU_A1T_SEQ_DEFN_FL(msg_name, 0)
#define ACU_A1T_SEQ_EXT_DEFN(msg_name) \
        ACU_A1T_SEQ_DEFN_FL(msg_name, ACU_A1T_EXTENDABLE)

#define ACU_A1T_DECL(msg_name)         acu_a1t_defn_t msg_name##_defn[]
#define ACU_A1T_EXTERN_DEFN(msg_name)  extern ACU_A1T_DECL(msg_name)

/************************************/

/* Impossible tag values (markers for non asn.1 entries) */
#define ACU_ASN1_BAD(n) ((n) << 8)

/* Ensure we save the correct value for 'Universal' tags */
#define ACU_A1T_TAG_CODE(x) \
        ((x) < 0x100 ? ACU_ASN1_MAKE_TAG((x) & 0xe0, (x) & 0x1f) : (x))

#define ACU_A1T_TYPE_OF(type, args) ACU_A1T_##type
#define ACU_A1T_TAG_OF(type, args) ACU_A1T_TAG_ args
#define ACU_A1T_TAG_(field, tag, flags) ACU_ASN1_##tag

#define ACU_A1T_COUNT(ref) (sizeof (struct ref) / sizeof (acu_a1t_value_t))

/* Actual initialiser for acu_a1t_defn_t */
#define ACU_A1T_VAL_(type, field, tag, count, ref, flags) \
        { type, (flags) & 0xff, ((flags) >> 8) & 0xff, ((flags) >> 16) & 0xff, \
            ACU_A1T_TAG_CODE(tag), count, ref, #field },

/* Normal initialiser for acu_a1t_defn_t, prepends to names */
#define ACU_A1T_VAL(type, field, tag, count, ref, flags) \
        ACU_A1T_VAL_(ACU_A1T_##type, field, ACU_ASN1_##tag, \
                count, ref, ACU_A1T_##flags)

/* Shorter lines when count == 0 and ref == NULL */
#define ACU_A1T_VAL_Z(type, field, tag, flags) \
        ACU_A1T_VAL_(ACU_A1T_##type, field, ACU_ASN1_##tag, \
                0, NULL, ACU_A1T_##flags)

/* Expander to generate type-specific initialiser */
#define ACU_A1T_DEFN_(type, args) ACU_A1T_DEFN_##type args

#define ACU_A1T_TYPE_(type, args) ACU_A1T_TYPE_##type args

/* acu_a1t_defn_t initialiser expanders for single items */
#define ACU_A1T_DEFN_INTEGER(field, tag, flags) \
        ACU_A1T_VAL_Z(INTEGER, field, tag, flags)

#define ACU_A1T_DEFN_UNSIGNED(field, tag, flags) \
        ACU_A1T_VAL_Z(UNSIGNED, field, tag, flags)

#define ACU_A1T_DEFN_BOOLEAN(field, tag, flags) \
        ACU_A1T_VAL_Z(BOOLEAN, field, tag, flags)

#define ACU_A1T_DEFN_NULL(field, tag, flags) \
        ACU_A1T_VAL_Z(NULL, field, tag, flags)

#define ACU_A1T_DEFN_BITS32(field, tag, width, flags) \
        ACU_A1T_VAL(BITS32, field, tag, ~ACU_ASN1_BIT32_MASK((width) - 1), \
                NULL, flags | (width) << 8)

#define ACU_A1T_DEFN_OCTETS(field, tag, flags) \
        ACU_A1T_VAL_Z(OCTETS, field, tag, flags)

#define ACU_A1T_DEFN_RAW(field, tag, flags) \
        ACU_A1T_VAL_Z(RAW, field, tag | ACU_ASN1_TAG_CONSTRUCTED, flags)

#define ACU_A1T_DEFN_ANY(field, flags) \
        ACU_A1T_VAL_Z(ANY, field, BAD(1), flags)

#define ACU_A1T_DEFN_SEQ(field, tag, flags) \
        ACU_A1T_VAL_Z(SEQ, field, tag | ACU_ASN1_TAG_CONSTRUCTED, flags)

#define ACU_A1T_DEFN_END_SEQ(field, flags) \
        ACU_A1T_VAL_Z(END_SEQ, field, BAD(2), flags)

#define ACU_A1T_DEFN_SEQ_OF(field, tag, flags) \
        ACU_A1T_VAL_Z(SEQ_OF, field, tag | ACU_ASN1_TAG_CONSTRUCTED, flags)

#define ACU_A1T_DEFN_END_SEQ_OF(field, count, flags) \
        ACU_A1T_VAL(END_SEQ_OF, field, BAD(3), count, NULL, flags)

#define ACU_A1T_DEFN_CHOICE(field, flags) \
        ACU_A1T_VAL_Z(CHOICE, field, BAD(4), flags)

#define ACU_A1T_DEFN_END_CHOICE(field, flags) \
        ACU_A1T_VAL_Z(END_CHOICE, field, BAD(5), flags)

/* REF and IMPLICIT generate an indirection and extra level of 'struct' */
#define ACU_A1T_DEFN_REF(field, ref, flags) \
        ACU_A1T_VAL(REF, field, BAD(6), ACU_A1T_COUNT(ref), \
                ref##_defn, flags)

#define ACU_A1T_DEFN_IMPLICIT(field, tag, ref, flags) \
        ACU_A1T_VAL(IMPLICIT, field, tag, ACU_A1T_COUNT(ref), \
                ref##_defn, flags)

/* For references to primitive items these directly encode the field */
#define ACU_A1T_DEFN_DIR_REF(field, ref, flags) \
        ACU_A1T_VAL_(ref(ACU_A1T_TYPE_OF), field, \
                ref(ACU_A1T_TAG_OF), 0, NULL, ACU_A1T_##flags)

#define ACU_A1T_DEFN_DIR_IMPL(field, tag, ref, flags) \
        ACU_A1T_VAL_(ref(ACU_A1T_TYPE_OF), field, \
                ACU_ASN1_##tag, 0, NULL, ACU_A1T_##flags)


/****** End of message based codec defines *********************************/

/* Sometimes we need to pass &(foo *) to a function that expects &(const foo *).
 * C isn't supposed to allow this and gcc errors it. */
#ifdef __GNUC__
#define ACU_ASN1_CONST_PTR_PTR(x) \
    ((const typeof(**x) **)((char *)(x) - (char *)0))
#else
/* Only microsoft C - which doesn't care */
#define ACU_ASN1_CONST_PTR_PTR(x) (x)
#endif

/* Defines for calling table codec functions.
 * These are useful because the 'value' structure that contains the values is
 * treated as an array by the codec functions. */

#define ACU_ASN1_ENCODE_DATA(a1b, defn, value) \
    acu_asn1_encode_data(a1b, ACU_ASN1_TABLE_DEFN(defn), \
            (const acu_a1t_value_t *)(value), sizeof *(value))

#define ACU_ASN1_DECODE_DATA(a1b, defn, value) \
    acu_asn1_decode_data(a1b, ACU_ASN1_TABLE_DEFN(defn), \
            (acu_a1t_value_t *)(value), sizeof *(value))

#define ACU_ASN1_TRACE_DATA(defn, value, hdr, flags) \
    acu_asn1_trace_data(ACU_ASN1_TABLE_DEFN(defn), \
            (const acu_a1t_value_t *)(value), sizeof *(value), hdr, flags)

#define ACU_ASN1_FIND_DEFN(defn, value, field, count, found_defn, found_value) \
    acu_asn1_find_defn(ACU_ASN1_TABLE_DEFN(defn), (acu_a1t_value_t *)(value), \
            field, count, found_defn, found_value)

#define ACU_ASN1_TABLE_DEFN(defn) defn##_defn

/* ASN.1 Encoder functions */
#define ACU_ASN1_FUNCTIONS(x) \
/* The buffer is declared 'const' even though the encode functions write \
 * to it to allow const pointers to be passed in to the decoder. */ \
x(acu_asn1_buf_t *, acu_asn1_buf_init, (acu_asn1_buf_t *, \
        const unsigned char *, unsigned int, unsigned int)) \
x(void, acu_asn1_buf_free, (acu_asn1_buf_t *)) \
\
x(const char *, acu_asn1_strerror, (int, unsigned int)) \
x(int, acu_asn1_error_1, (acu_asn1_buf_t *, const char *, int, const char *, \
       int, int, const char *)) \
x(int, acu_asn1_error, (acu_asn1_buf_t *, const char *, int, const char *, \
       int, int)) \
x(void, acu_asn1_fmt_errmsg, (acu_asn1_buf_t *, char *, int)) \
\
x(int, acu_asn1_put_constructed, (acu_asn1_buf_t *, unsigned int)) \
x(int, acu_asn1_put_end_constructed, (acu_asn1_buf_t *)) \
x(int, acu_asn1_put_end_all_constructed, (acu_asn1_buf_t *, int)) \
\
x(int, acu_asn1_put_int, (acu_asn1_buf_t *, unsigned int, int)) \
x(int, acu_asn1_put_unsigned, (acu_asn1_buf_t *, unsigned int, unsigned int, \
        unsigned int)) \
x(int, acu_asn1_put_octet_8, (acu_asn1_buf_t *, unsigned int, unsigned int, \
        unsigned int)) \
x(int, acu_asn1_put_octetstring, (acu_asn1_buf_t *, unsigned int, \
        const void *, unsigned int)) \
x(int, acu_asn1_put_bits32, (acu_asn1_buf_t *, unsigned int, unsigned int, \
        unsigned int)) \
x(int, acu_asn1_put_bitstring, (acu_asn1_buf_t *, unsigned int, const void *, \
        unsigned int)) \
x(int, acu_asn1_put_raw_octets, (acu_asn1_buf_t *, const void *, \
        unsigned int)) \
x(void *, acu_asn1_put_space, (acu_asn1_buf_t *, unsigned int)) \
\
x(int, acu_asn1_encode_object_id_str, (unsigned char *, int, const char *)) \
x(int, acu_asn1_encode_object_id_int, (unsigned char *, int, ...)) \
\
/* ASN.1 decoder functions */ \
x(int, acu_asn1_get_tag_len, (acu_asn1_buf_t *, int *)) \
x(int, acu_asn1_get_reference, (acu_asn1_buf_t *, int, const unsigned char **))\
x(int, acu_asn1_get_int, (acu_asn1_buf_t *, int *)) \
x(int, acu_asn1_get_unsigned, (acu_asn1_buf_t *, unsigned int *)) \
x(int, acu_asn1_get_octet_8, (acu_asn1_buf_t *, unsigned int *, \
        unsigned int *)) \
x(int, acu_asn1_get_octetstring, (acu_asn1_buf_t *, void *, unsigned int)) \
x(int, acu_asn1_get_bits32, (acu_asn1_buf_t *, unsigned int *)) \
x(int, acu_asn1_decode_object_id_str, (char *, unsigned int, \
        const unsigned char *, unsigned int)) \
x(int, acu_asn1_decode_object_id_int, (unsigned int *, unsigned int, \
        const unsigned char *, unsigned int)) \
\
/* Table based codec */ \
x(int, acu_asn1_encode_data, (acu_asn1_buf_t *, const acu_a1t_defn_t *, \
        const acu_a1t_value_t *, int)) \
x(int, acu_asn1_decode_data, (acu_asn1_buf_t *, const acu_a1t_defn_t *, \
        acu_a1t_value_t *, int)) \
x(int, acu_asn1_find_defn, (const acu_a1t_defn_t *, const acu_a1t_value_t *, \
        const char *, unsigned int, \
        const acu_a1t_defn_t **, const acu_a1t_value_t **)) \
/* Returned buffer is allocated by malloc(), caller must free() */ \
x(char *, acu_asn1_trace_data, (const acu_a1t_defn_t *, \
        const acu_a1t_value_t *, int, const char *, unsigned int)) \
x(void, acu_asn1_free_trace_data, (char *)) \

/* The ASN.1 functions above are normally expanded with the TCAP functions.
 * If not, then the following define can be used as:
 * ACU_ASN1_FUNCTIONS(ACU_ASN1_FN_DEFS) */
#define ACU_ASN1_FN_DEFS(rslt, name, args) ACU_TCAP_LIB_API rslt name args;

#ifdef __cplusplus
}
#endif

#endif
