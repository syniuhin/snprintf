#include "snprintf.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define STATE_DEFAULT 0
#define STATE_FLAGS   1
#define STATE_MIN     2
#define STATE_DOT     3
#define STATE_MAX     4
#define STATE_MOD     5
#define STATE_CONV    6
#define STATE_DONE    7

#define FLAG_MINUS      (1 << 0)
#define FLAG_PLUS       (1 << 1)
#define FLAG_SPACE      (1 << 2)
#define FLAG_NUM        (1 << 3)
#define FLAG_ZERO       (1 << 4)
#define FLAG_UP         (1 << 5)
#define FLAG_UNSIGNED   (1 << 6)

static void print_char (char *buffer, size_t *currlen,
        size_t maxlen, char c) {
    if (*currlen < maxlen)
        buffer[(*currlen)++] = c;
}

static void fmtint (char *buffer, size_t *currlen, size_t maxlen,
            long value, int base, int min, int max, int flags) {
    int signvalue = 0;
    unsigned long uvalue;

    const int MAXLEN = 50;
    char convert[MAXLEN];
    int place = 0;
    int spadlen = 0; /* amount to space pad */
    int zpadlen = 0; /* amount to zero pad */
    int caps = 0;

    if (max < 0)
        max = 0;

    uvalue = value;

    if(!(flags & FLAG_UNSIGNED)) {
        if( value < 0 ) {
            signvalue = '-';
            uvalue = -value;
        } else if (flags & FLAG_PLUS) { /* Do a sign (+/i) */
            signvalue = '+';
        } else if (flags & FLAG_SPACE) {
            signvalue = ' ';
        }
    }

    if (flags & FLAG_UP) caps = 1; /* Should characters be upper case? */

    do {
        convert[place++] =
            (caps
                ? "0123456789ABCDEF"
                :"0123456789abcdef") [uvalue % (unsigned) base];
        uvalue = (uvalue / (unsigned) base);
    } while (uvalue && (place < MAXLEN));
    if (place == MAXLEN) place--;
    convert[place] = 0;

    zpadlen = max - place;
    spadlen = min - (max > place ? max : place) - (signvalue ? 1 : 0);
    if (zpadlen < 0) zpadlen = 0;
    if (spadlen < 0) spadlen = 0;
    if (flags & FLAG_ZERO) {
        zpadlen = (zpadlen > spadlen ? zpadlen : spadlen);
        spadlen = 0;
    }
    if (flags & FLAG_MINUS)
        spadlen = -spadlen; /* Left Justifty */

    /* Spaces */
    while (spadlen > 0) {
        print_char (buffer, currlen, maxlen, ' ');
        --spadlen;
    }

    /* Sign */
    if (signvalue)
        print_char (buffer, currlen, maxlen, signvalue);

    /* Zeros */
    if (zpadlen > 0) {
        while (zpadlen > 0) {
            print_char (buffer, currlen, maxlen, '0');
            --zpadlen;
        }
    }

    /* Digits */
    while (place > 0)
        print_char (buffer, currlen, maxlen, convert[--place]);

    /* Left Justified spaces */
    while (spadlen < 0) {
        print_char (buffer, currlen, maxlen, ' ');
        ++spadlen;
    }
}

static void fmtstr (char *buffer, size_t *currlen, size_t maxlen,
            char *value, int flags, int min, int max) {
    int padlen, strln;         /* amount to pad */
    int cnt = 0;

    if (value == 0) {
        value = "<NULL>";
    }

    for (strln = 0; value[strln]; ++strln); /* strlen */
    padlen = min - strln;
    if (padlen < 0)
        padlen = 0;
    if (flags & FLAG_MINUS)
        padlen = -padlen; /* Left Justify */

    while ((padlen > 0) && (cnt < max)) {
        print_char (buffer, currlen, maxlen, ' ');
        --padlen;
        ++cnt;
    }
    while (*value && (cnt < max)) {
        print_char (buffer, currlen, maxlen, *value++);
        ++cnt;
    }
    while ((padlen < 0) && (cnt < max)) {
        print_char (buffer, currlen, maxlen, ' ');
        ++padlen;
        ++cnt;
    }
}


static long round (long double value) {
    long intpart;

    intpart = value;
    value = value - intpart;
    if (value >= 0.5)
        intpart++;

    return intpart;
}

static long double pow10 (int exp) {
    long double result = 1;
    while (exp) {
        result *= 10;
        exp--;
    }
    return result;
}

/**
 * Formats floating point number
 * @arg min : min width
 * @arg max : max precision
 */
static void fmtfp (char *buffer, size_t *currlen, size_t maxlen,
           long double fvalue, int min, int max, int flags) {
    int signvalue = 0;
    long double ufvalue;

    const int MAXLEN = 50;
    char iconvert[MAXLEN];
    char fconvert[MAXLEN];
    int iplace = 0;
    int fplace = 0;
    int padlen = 0; /* amount to pad */
    int zpadlen = 0;
    int caps = 0;
    long intpart;
    long fracpart;

    /*
     * AIX manpage says the default is 0, but Solaris says the default
     * is 6, and sprintf on AIX defaults to 6
     */
    if (max < 0)
        max = 6;

    ufvalue = (fvalue >= 0 ? fvalue : -fvalue);

    if (fvalue < 0)
        signvalue = '-';
    else if (flags & FLAG_PLUS)    /* Do a sign (+/i) */
        signvalue = '+';
    else if (flags & FLAG_SPACE)
        signvalue = ' ';

    intpart = ufvalue;

    /**
     * CHEAT
     */
//    if (max > 9)
//        max = 9;
    fracpart = round ((pow10(max)) * (ufvalue - intpart));
//    long double ffracpart = 10 * (ufvalue - intpart);
//    int zafterp = 0;
//    while (!(int)ffracpart) {
//        zafterp++;
//        ffracpart *= 10;
//    }

    if (fracpart >= pow10 (max)) {
        intpart++;
        fracpart -= pow10 (max);
    }

    /* Convert integer part */
    do {
        iconvert[iplace++] =
            (caps
                ? "0123456789ABCDEF"
                : "0123456789abcdef")[intpart % 10];
        intpart = (intpart / 10);
    } while (intpart && (iplace < MAXLEN));
    if (iplace == MAXLEN) iplace--;
    iconvert[iplace] = 0;

    /* Convert fractional part */
    //do {
    //    fconvert[fplace++] =
    //        (caps
    //            ? "0123456789ABCDEF"
    //            : "0123456789abcdef")[fracpart % 10];
    //    fracpart = (fracpart / 10);
    //} while (fracpart && (fplace < MAXLEN));

    //while (zafterp-- && fplace < MAXLEN && fplace < max)
    //    fconvert[fplace++] = '0';
    ufvalue -= (int) ufvalue;
    ufvalue *= 10;
    for (int i = 0; i < max; ++i) {
        fconvert[fplace++] = (int) ufvalue + '0';
        ufvalue = ufvalue - (int) ufvalue;
        ufvalue *= 10;
    }

    if (fplace == MAXLEN) fplace--;
    fconvert[fplace] = 0;

    /* Round */
//    if (ufvalue > 5) {
//        int find = fplace;
//        --find;
//        int carry = 1;
//        while (carry && find + 1) {
//            fconvert[find]++;
//            if (fconvert[find] > '9') {
//                fconvert[find] = '0';
//                carry = 1;
//            } else {
//                carry = 0;
//            }
//            --find;
//        }
//        if (carry) {
//            int iind = iplace;
//            --iind;
//            while (carry && iind + 1) {
//                iconvert[iind]++;
//                if (iconvert[iind] > '9') {
//                    iconvert[iind] = '0';
//                    carry = 1;
//                } else {
//                    carry = 0;
//                }
//                --iind;
//            }
//        }
//        if (carry) {
//            /* move num */
//            for (int r = (iplace + 1 < MAXLEN - 1)
//                    ? iplace + 1 : MAXLEN - 1; r >= 0; r--)
//                iconvert[iplace] = iconvert[iplace - 1];
//            iconvert[0] = '1';
//        }
//    }

    /* -1 for decimal point, another -1 if we are printing a sign */
    padlen = min - iplace - max - 1 - ((signvalue) ? 1 : 0);
    zpadlen = max - fplace;
    if (zpadlen < 0)
        zpadlen = 0;
    if (padlen < 0)
        padlen = 0;
    if (flags & FLAG_MINUS)
        padlen = -padlen; /* Left Justifty */

    if ((flags & FLAG_ZERO) && (padlen > 0)) {
        if (signvalue) {
            print_char (buffer, currlen, maxlen, signvalue);
            --padlen;
            signvalue = 0;
        }
        while (padlen > 0) {
            print_char (buffer, currlen, maxlen, '0');
            --padlen;
        }
    }
    while (padlen > 0) {
        print_char (buffer, currlen, maxlen, ' ');
        --padlen;
    }
    if (signvalue)
        print_char (buffer, currlen, maxlen, signvalue);

    while (iplace > 0)
        print_char (buffer, currlen, maxlen, iconvert[--iplace]);

    if (max > 0) {
        print_char (buffer, currlen, maxlen, '.');

        for (int i = 0; i < fplace; ++i)
            print_char(buffer, currlen, maxlen, fconvert[i]);
//        while (fplace > 0)
//            print_char (buffer, currlen, maxlen, fconvert[--fplace]);
    }

    while (zpadlen > 0) {
        print_char (buffer, currlen, maxlen, '0');
        --zpadlen;
    }

    while (padlen < 0) {
        print_char (buffer, currlen, maxlen, ' ');
        ++padlen;
    }
}

static void dooo(char * s, size_t n, const char * format, va_list args) {
    long value;
    long double fvalue;
    char *strvalue;

    int state = STATE_DEFAULT;
    size_t currlen = 0;
    int flags = 0;
    int cflags = 0;
    int min = 0;
    int max = -1;
    char ch = *format++;

    while (state != STATE_DONE) {
        if ('\0' == ch || currlen > n)
            state = STATE_DONE;

        switch (state) {
            case STATE_DEFAULT:
                if ('%' == ch)
                    state = STATE_FLAGS;
                else
                    print_char(s, &currlen, n, ch);
                ch = *format++;
                break;
            case STATE_FLAGS:
                switch (ch) {
                    case '-':
                        flags |= FLAG_MINUS;
                        ch = *format++;
                        break;
                    case '+':
                        flags |= FLAG_PLUS;
                        ch = *format++;
                        break;
                    case ' ':
                        flags |= FLAG_SPACE;
                        ch = *format++;
                        break;
                    case '#':
                        flags |= FLAG_NUM;
                        ch = *format++;
                        break;
                    case '0':
                        flags |= FLAG_ZERO;
                        ch = *format++;
                        break;
                    default:
                        state = STATE_MIN;
                        break;
                }
                break;
            case STATE_MIN:
                if (isdigit(ch)) {
                    min = 10 * min + (ch - '0');
                    ch = *format++;
                } else if ('*' == ch) {
                    min = va_arg (args, int);
                    ch = *format++;
                    state = STATE_DOT;
                } else {
                    state = STATE_DOT;
                }
                break;
            case STATE_DOT:
                if ('.' == ch) {
                    state = STATE_MAX;
                    ch = *format++;
                } else {
                    state = STATE_MOD;
                }
                break;
            case STATE_MAX:
                if (isdigit(ch)) {
                    if (max < 0)
                        max = 0;
                    max = 10 * max + (ch - '0');
                    ch = *format++;
                } else if ('*' == ch) {
                    max = va_arg(args, int);
                    ch = *format++;
                    state = STATE_MOD;
                } else {
                    state = STATE_MOD;
                }
                break;
            case STATE_MOD:
                //TODO: support this
                state = STATE_CONV;
                break;
            case STATE_CONV:
                switch (ch) {
                    case 'd':
                    case 'i':
                        value = va_arg (args, int);
                        fmtint (s, &currlen, n, value, 10, min, max, flags);
                        break;
                    case 'O':
                    case 'o':
                        flags |= FLAG_UNSIGNED;
                        value = va_arg (args, unsigned int);
                        fmtint (s, &currlen, n, value, 8, min, max, flags);
                        break;
                    case 'X':
                        flags |= FLAG_UP;
                    case 'x':
                        flags |= FLAG_UNSIGNED;
                        value = va_arg (args, unsigned int);
                        fmtint (s, &currlen, n, value, 16, min, max, flags);
                        break;
                    case 'f':
                        fvalue = va_arg (args, double);
                        /* um, floating point? */
                        fmtfp (s, &currlen, n, fvalue, min, max, flags);
                        break;
                    case 'c':
                        print_char(s, &currlen, n, va_arg(args, int));
                        break;
                    case 's':
                        strvalue = va_arg (args, char *);
                        if (max < 0)
                            max = n; /* ie, no max */
                        fmtstr (s, &currlen, n, strvalue, flags, min, max);
                        break;
                    case '%':
                        print_char (s, &currlen, n, ch);
                        break;
                    default:
                        /* Unknown, skip */
                        break;
                }
                ch = *format++;
                state = STATE_DEFAULT;
                flags = cflags = min = 0;
                max = -1;
                break;
            case STATE_DONE:
                break;
            default:
                break;
        }
    }
    if (currlen < n - 1)
        s[currlen] = '\0';
    else
        s[n - 1] = '\0';
}

int my_snprintf(char * s, size_t n, const char * format, ...) {
    va_list args;
    va_start(args, format);
    dooo(s, n, format, args);
    return 0;
}
