#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

typedef uint32 can_cpartn_t;

#define CANCPARTNGetDatum(x)     Int32GetDatum(x)
#define DatumGetCANCPARTN(x)     DatumGetInt32(x)
#define PG_GETARG_CANCPARTN(n) DatumGetCANCPARTN(PG_GETARG_DATUM(n))
#define PG_RETURN_CANCPARTN(x) return CANCPARTNGetDatum(x)


// 30 bits store 9 full digits (0-1,073,741,823), 2 bits store the format (0-3).
#define FORMAT_TYPE_INVALID     0
#define FORMAT_TYPE_PAPER       1
#define FORMAT_TYPE_ELECTRONIC  2

Datum can_cpartn_in(PG_FUNCTION_ARGS);
Datum can_cpartn_out(PG_FUNCTION_ARGS);
Datum can_cpartn_to_text(PG_FUNCTION_ARGS);
Datum text_to_can_cpartn(PG_FUNCTION_ARGS);
Datum can_cpartn_send(PG_FUNCTION_ARGS);
Datum can_cpartn_recv(PG_FUNCTION_ARGS);
Datum can_cpartn_lt(PG_FUNCTION_ARGS);
Datum can_cpartn_le(PG_FUNCTION_ARGS);
Datum can_cpartn_eq(PG_FUNCTION_ARGS);
Datum can_cpartn_ne(PG_FUNCTION_ARGS);
Datum can_cpartn_ge(PG_FUNCTION_ARGS);
Datum can_cpartn_gt(PG_FUNCTION_ARGS);
Datum can_cpartn_cmp(PG_FUNCTION_ARGS);

static can_cpartn_t cstring_to_can_cpartn(char *can_cpartn_string);
static char *can_cpartn_to_cstring(can_cpartn_t can_cpartn);



/* generic input/output functions */
PG_FUNCTION_INFO_V1(can_cpartn_in);
Datum
can_cpartn_in(PG_FUNCTION_ARGS)
{
    can_cpartn_t    result;

    char   *can_cpartn_str = PG_GETARG_CSTRING(0);
    result = cstring_to_can_cpartn(can_cpartn_str);

    PG_RETURN_CANCPARTN(result);
}

PG_FUNCTION_INFO_V1(can_cpartn_out);
Datum
can_cpartn_out(PG_FUNCTION_ARGS)
{
    can_cpartn_t    packed_can_cpartn;
    char   *can_cpartn_string;

    packed_can_cpartn = PG_GETARG_CANCPARTN(0);
    can_cpartn_string = can_cpartn_to_cstring(packed_can_cpartn);

    PG_RETURN_CSTRING(can_cpartn_string);
}

/* type to/from text conversion routines */
PG_FUNCTION_INFO_V1(can_cpartn_to_text);
Datum
can_cpartn_to_text(PG_FUNCTION_ARGS)
{
    char    *can_cpartn_string;
    text    *can_cpartn_text;

    can_cpartn_t    packed_can_cpartn =  PG_GETARG_CANCPARTN(0);

    can_cpartn_string = can_cpartn_to_cstring(packed_can_cpartn);
    can_cpartn_text = DatumGetTextP(DirectFunctionCall1(textin, CStringGetDatum(can_cpartn_string)));

    PG_RETURN_TEXT_P(can_cpartn_text);
}

PG_FUNCTION_INFO_V1(text_to_can_cpartn);
Datum
text_to_can_cpartn(PG_FUNCTION_ARGS)
{
    text  *can_cpartn_text = PG_GETARG_TEXT_P(0);
    char  *can_cpartn_str = DatumGetCString(DirectFunctionCall1(textout, PointerGetDatum(can_cpartn_text)));
    can_cpartn_t can_cpartn = cstring_to_can_cpartn(can_cpartn_str);

    PG_RETURN_CANCPARTN(can_cpartn);
}

/* Functions to convert to/from bytea */
PG_FUNCTION_INFO_V1(can_cpartn_send);
Datum
can_cpartn_send(PG_FUNCTION_ARGS)
{
    StringInfoData buffer;
    can_cpartn_t can_cpartn = PG_GETARG_CANCPARTN(0);

    pq_begintypsend(&buffer);
    pq_sendint(&buffer, (int32)can_cpartn, 4);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buffer));
}

PG_FUNCTION_INFO_V1(can_cpartn_recv);
Datum can_cpartn_recv(PG_FUNCTION_ARGS)
{
    can_cpartn_t    can_cpartn;
    StringInfo    buffer = (StringInfo) PG_GETARG_POINTER(0);

    can_cpartn = pq_getmsgint(buffer, 4);
    PG_RETURN_CANCPARTN(can_cpartn);
}

/* functions to support btree opclass */
PG_FUNCTION_INFO_V1(can_cpartn_lt);
Datum
can_cpartn_lt(PG_FUNCTION_ARGS)
{
    can_cpartn_t a = PG_GETARG_CANCPARTN(0);
    can_cpartn_t b = PG_GETARG_CANCPARTN(1);
    PG_RETURN_BOOL(a < b);
}

PG_FUNCTION_INFO_V1(can_cpartn_le);
Datum
can_cpartn_le(PG_FUNCTION_ARGS)
{
    can_cpartn_t a = PG_GETARG_CANCPARTN(0);
    can_cpartn_t b = PG_GETARG_CANCPARTN(1);
    PG_RETURN_BOOL (a <= b);
}

PG_FUNCTION_INFO_V1(can_cpartn_eq);
Datum
can_cpartn_eq(PG_FUNCTION_ARGS)
{
    can_cpartn_t a = PG_GETARG_CANCPARTN(0);
    can_cpartn_t b = PG_GETARG_CANCPARTN(1);
    PG_RETURN_BOOL(a == b);
}

PG_FUNCTION_INFO_V1(can_cpartn_ne);
Datum
can_cpartn_ne(PG_FUNCTION_ARGS)
{
    can_cpartn_t a = PG_GETARG_CANCPARTN(0);
    can_cpartn_t b = PG_GETARG_CANCPARTN(1);
    PG_RETURN_BOOL(a != b);
}

PG_FUNCTION_INFO_V1(can_cpartn_ge);
Datum
can_cpartn_ge(PG_FUNCTION_ARGS)
{
    can_cpartn_t a = PG_GETARG_CANCPARTN(0);
    can_cpartn_t b = PG_GETARG_CANCPARTN(1);
    PG_RETURN_BOOL(a >= b);
}

PG_FUNCTION_INFO_V1(can_cpartn_gt);
Datum
can_cpartn_gt(PG_FUNCTION_ARGS)
{
    can_cpartn_t a = PG_GETARG_CANCPARTN(0);
    can_cpartn_t b = PG_GETARG_CANCPARTN(1);
    PG_RETURN_BOOL(a > b);
}

static int32
can_cpartn_cmp_internal(can_cpartn_t a, can_cpartn_t b)
{
    return a - b;

    /*if (a < b)
        return -1;
    else if (a > b)
        return 1;

    return 0;*/
}

PG_FUNCTION_INFO_V1(can_cpartn_cmp);
Datum
can_cpartn_cmp(PG_FUNCTION_ARGS)
{
    can_cpartn_t a = PG_GETARG_CANCPARTN(0);
    can_cpartn_t b = PG_GETARG_CANCPARTN(1);

    PG_RETURN_INT32(can_cpartn_cmp_internal(a, b));
}

/*****************************************************************************
 * Aggregate functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(can_cpartn_smaller);

Datum
can_cpartn_smaller(PG_FUNCTION_ARGS)
{
   can_cpartn_t left  = PG_GETARG_CANCPARTN(0);
   can_cpartn_t right = PG_GETARG_CANCPARTN(1);
   int32 result;

   result = can_cpartn_cmp_internal(left, right) < 0 ? left : right;
   PG_RETURN_CANCPARTN(result);
}

PG_FUNCTION_INFO_V1(can_cpartn_larger);

Datum
can_cpartn_larger(PG_FUNCTION_ARGS)
{
   can_cpartn_t left  = PG_GETARG_CANCPARTN(0);
   can_cpartn_t right = PG_GETARG_CANCPARTN(1);
   int32 result;

   result = can_cpartn_cmp_internal(left, right) > 0 ? left : right;
   PG_RETURN_CANCPARTN(result);
}


static can_cpartn_t
cstring_to_can_cpartn(char *can_cpartn_str)
{
    char            *c;
    can_cpartn_t    result;
    uint32          format_type = FORMAT_TYPE_INVALID;
    uint32          ndigits = 0;
    uint32          digits[8];//9
    uint32          number;

    for (c = can_cpartn_str; *c != 0; c++)
    {
        if (ndigits >= 8)
            ereport(ERROR,
                    (errmsg("Invalid number of digits (%d) in input data \"%s\".", ndigits, can_cpartn_str),
                    errhint("A valid CPA routing transit number consists of 8 digits.")));

        if (*c == '0' && c == can_cpartn_str)
        {   // The 0 is not stored.  If > 0 is added later, increase the digits array and store the new digit in digits[0].
            // Then the format should be assumed to be electronic unless the "-" is present in the proper location.
            format_type = FORMAT_TYPE_ELECTRONIC;
        }
        else if (*c >= '0' && *c <= '9')
        {
            digits[ndigits] = *c - '0';
            ndigits++;
        }
        else if (*c == '-' && ndigits == 5 && format_type == FORMAT_TYPE_INVALID)
            format_type = FORMAT_TYPE_PAPER; // N0000-000 where N => 1?
        else
            ereport(ERROR,
                    (errmsg("Unexpected character '%c' in input data \"%s\".", *c, can_cpartn_str),
                     errhint("A valid CPA routing transit number consists of digits only.")));
    }

    switch (format_type) {
        case FORMAT_TYPE_PAPER:
            number =
                digits[5] * 10000000 +
                digits[6] * 1000000 +
                digits[7] * 100000 +
                digits[0] * 10000 +
                digits[1] * 1000 +
                digits[2] * 100 +
                digits[3] * 10 +
                digits[4];
        break;
        case FORMAT_TYPE_ELECTRONIC:
            number =
                digits[0] * 10000000 +
                digits[1] * 1000000 +
                digits[2] * 100000 +
                digits[3] * 10000 +
                digits[4] * 1000 +
                digits[5] * 100 +
                digits[6] * 10 +
                digits[7];
        break;
        default:
            ereport(ERROR,
                    (errmsg("Invalid format in input data \"%s\".", can_cpartn_str),
                    errhint("A valid CPA routing transit number uses a BBBBB-III or 0IIIBBBBB format.")));
        break;
    }

    result = (number << 2) + format_type;

    PG_RETURN_CANCPARTN(result);
}

static char *
can_cpartn_to_cstring(can_cpartn_t can_cpartn)
{
    int32   remainder = can_cpartn;
    int32   digit0;
    int32   digit1;
    int32   digit2;
    int32   digit3;
    int32   digit4;
    int32   digit5;
    int32   digit6;
    int32   digit7;
    int32   digit8;
    int32   format_type;
    char   *can_cpartn_str;

    format_type = remainder & 3;
    //remainder = (remainder & 0xFFFFFFFC) >> 2;
    remainder = remainder >> 2;

    /*digit8 = remainder % 10;
    remainder *= .1;
    digit7 = remainder % 10;
    remainder *= .1;
    digit6 = remainder % 10;
    remainder *= .1;
    digit5 = remainder % 10;
    remainder *= .1;
    digit4 = remainder % 10;
    remainder *= .1;
    digit3 = remainder % 10;
    remainder *= .1;
    digit2 = remainder % 10;
    remainder *= .1;
    digit1 = remainder % 10;
    remainder *= .1;
    digit0 = remainder % 10;*/

    if (remainder > 999999999 || remainder < 0)
        ereport(ERROR,
                (errmsg("Invalid data."),
                errhint("Was there data corruption or was the can_cpartn extension reverted to an older version?")));

    digit0 = remainder * .00000001;
    remainder = remainder - digit0 * 100000000;
    digit1 = remainder * .0000001;
    remainder = remainder - digit1 * 10000000;
    digit2 = remainder * .000001;
    remainder = remainder - digit2 * 1000000;
    digit3 = remainder * .00001;
    remainder = remainder - digit3 * 100000;
    digit4 = remainder * .0001;
    remainder = remainder - digit4 * 10000;
    digit5 = remainder * .001;
    remainder = remainder - digit5 * 1000;
    digit6 = remainder * .01;
    remainder = remainder - digit6 * 100;
    digit7 = remainder * .1;
    digit8 = remainder - digit7 * 10;

    switch (format_type) {
        case FORMAT_TYPE_PAPER:
            can_cpartn_str = palloc(10);
            can_cpartn_str[0] = '0' + digit4;
            can_cpartn_str[1] = '0' + digit5;
            can_cpartn_str[2] = '0' + digit6;
            can_cpartn_str[3] = '0' + digit7;
            can_cpartn_str[4] = '0' + digit8;
            can_cpartn_str[5] = '-';
            can_cpartn_str[6] = '0' + digit1;
            can_cpartn_str[7] = '0' + digit2;
            can_cpartn_str[8] = '0' + digit3;
            can_cpartn_str[9] = '\0';
        break;
        case FORMAT_TYPE_ELECTRONIC:
            can_cpartn_str = palloc(10);
            can_cpartn_str[0] = '0' + digit0;
            can_cpartn_str[1] = '0' + digit1;
            can_cpartn_str[2] = '0' + digit2;
            can_cpartn_str[3] = '0' + digit3;
            can_cpartn_str[4] = '0' + digit4;
            can_cpartn_str[5] = '0' + digit5;
            can_cpartn_str[6] = '0' + digit6;
            can_cpartn_str[7] = '0' + digit7;
            can_cpartn_str[8] = '0' + digit8;
            can_cpartn_str[9] = '\0';
        break;
        default:
            ereport(ERROR,
                    (errmsg("Invalid format."),
                    errhint("Was there data corruption or was the can_cpartn extension reverted to an older version?")));
        break;
    }

    return can_cpartn_str;
}
