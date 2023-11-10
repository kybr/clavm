
/******************************************************************************
 *                                                                            *
 *                              Math Functions                                *
 *                                                                            *
 ******************************************************************************/

extern float acosf(float);
extern double acos(double);
extern long double acosl(long double);

extern float asinf(float);
extern double asin(double);
extern long double asinl(long double);

extern float atanf(float);
extern double atan(double);
extern long double atanl(long double);

extern float atan2f(float, float);
extern double atan2(double, double);
extern long double atan2l(long double, long double);

extern float cosf(float);
extern double cos(double);
extern long double cosl(long double);

extern float sinf(float);
extern double sin(double);
extern long double sinl(long double);

extern float tanf(float);
extern double tan(double);
extern long double tanl(long double);

extern float acoshf(float);
extern double acosh(double);
extern long double acoshl(long double);

extern float asinhf(float);
extern double asinh(double);
extern long double asinhl(long double);

extern float atanhf(float);
extern double atanh(double);
extern long double atanhl(long double);

extern float coshf(float);
extern double cosh(double);
extern long double coshl(long double);

extern float sinhf(float);
extern double sinh(double);
extern long double sinhl(long double);

extern float tanhf(float);
extern double tanh(double);
extern long double tanhl(long double);

extern float expf(float);
extern double exp(double);
extern long double expl(long double);

extern float exp2f(float);
extern double exp2(double);
extern long double exp2l(long double);

extern float expm1f(float);
extern double expm1(double);
extern long double expm1l(long double);

extern float logf(float);
extern double log(double);
extern long double logl(long double);

extern float log10f(float);
extern double log10(double);
extern long double log10l(long double);

extern float log2f(float);
extern double log2(double);
extern long double log2l(long double);

extern float log1pf(float);
extern double log1p(double);
extern long double log1pl(long double);

extern float logbf(float);
extern double logb(double);
extern long double logbl(long double);

extern float modff(float, float *);
extern double modf(double, double *);
extern long double modfl(long double, long double *);

extern float ldexpf(float, int);
extern double ldexp(double, int);
extern long double ldexpl(long double, int);

extern float frexpf(float, int *);
extern double frexp(double, int *);
extern long double frexpl(long double, int *);

extern int ilogbf(float);
extern int ilogb(double);
extern int ilogbl(long double);

extern float scalbnf(float, int);
extern double scalbn(double, int);
extern long double scalbnl(long double, int);

extern float scalblnf(float, long int);
extern double scalbln(double, long int);
extern long double scalblnl(long double, long int);

extern float fabsf(float);
extern double fabs(double);
extern long double fabsl(long double);

extern float cbrtf(float);
extern double cbrt(double);
extern long double cbrtl(long double);

extern float hypotf(float, float);
extern double hypot(double, double);
extern long double hypotl(long double, long double);

extern float powf(float, float);
extern double pow(double, double);
extern long double powl(long double, long double);

extern float sqrtf(float);
extern double sqrt(double);
extern long double sqrtl(long double);

extern float erff(float);
extern double erf(double);
extern long double erfl(long double);

extern float erfcf(float);
extern double erfc(double);
extern long double erfcl(long double);

/*	lgammaf, lgamma, and lgammal are not thread-safe. The thread-safe
    variants lgammaf_r, lgamma_r, and lgammal_r are made available if
    you define the _REENTRANT symbol before including <math.h>                */
extern float lgammaf(float);
extern double lgamma(double);
extern long double lgammal(long double);

extern float tgammaf(float);
extern double tgamma(double);
extern long double tgammal(long double);

extern float ceilf(float);
extern double ceil(double);
extern long double ceill(long double);

extern float floorf(float);
extern double floor(double);
extern long double floorl(long double);

extern float nearbyintf(float);
extern double nearbyint(double);
extern long double nearbyintl(long double);

extern float rintf(float);
extern double rint(double);
extern long double rintl(long double);

extern long int lrintf(float);
extern long int lrint(double);
extern long int lrintl(long double);

extern float roundf(float);
extern double round(double);
extern long double roundl(long double);

extern long int lroundf(float);
extern long int lround(double);
extern long int lroundl(long double);

/*  long long is not part of C90. Make sure you are passing -std=c99 or
    -std=gnu99 or higher if you need these functions returning long longs     */
#if !(__DARWIN_NO_LONG_LONG)
extern long long int llrintf(float);
extern long long int llrint(double);
extern long long int llrintl(long double);

extern long long int llroundf(float);
extern long long int llround(double);
extern long long int llroundl(long double);
#endif /* !(__DARWIN_NO_LONG_LONG) */

extern float truncf(float);
extern double trunc(double);
extern long double truncl(long double);

extern float fmodf(float, float);
extern double fmod(double, double);
extern long double fmodl(long double, long double);

extern float remainderf(float, float);
extern double remainder(double, double);
extern long double remainderl(long double, long double);

extern float remquof(float, float, int *);
extern double remquo(double, double, int *);
extern long double remquol(long double, long double, int *);

extern float copysignf(float, float);
extern double copysign(double, double);
extern long double copysignl(long double, long double);

extern float nanf(const char *);
extern double nan(const char *);
extern long double nanl(const char *);

extern float nextafterf(float, float);
extern double nextafter(double, double);
extern long double nextafterl(long double, long double);

extern double nexttoward(double, long double);
extern float nexttowardf(float, long double);
extern long double nexttowardl(long double, long double);

extern float fdimf(float, float);
extern double fdim(double, double);
extern long double fdiml(long double, long double);

extern float fmaxf(float, float);
extern double fmax(double, double);
extern long double fmaxl(long double, long double);

extern float fminf(float, float);
extern double fmin(double, double);
extern long double fminl(long double, long double);

extern float fmaf(float, float, float);
extern double fma(double, double, double);
extern long double fmal(long double, long double, long double);

// extern double j0(double) __API_AVAILABLE(macos(10.0), ios(3.2));
// extern double j1(double) __API_AVAILABLE(macos(10.0), ios(3.2));
// extern double jn(int, double) __API_AVAILABLE(macos(10.0), ios(3.2));
// extern double y0(double) __API_AVAILABLE(macos(10.0), ios(3.2));
// extern double y1(double) __API_AVAILABLE(macos(10.0), ios(3.2));
// extern double yn(int, double) __API_AVAILABLE(macos(10.0), ios(3.2));
// extern double scalb(double, double);
// extern int signgam;

/*  Even though these might be more useful as long doubles, POSIX requires
    that they be double-precision literals.                                   */
#define M_E 2.71828182845904523536028747135266250        /* e              */
#define M_LOG2E 1.44269504088896340735992468100189214    /* log2(e)        */
#define M_LOG10E 0.434294481903251827651128918916605082  /* log10(e)       */
#define M_LN2 0.693147180559945309417232121458176568     /* loge(2)        */
#define M_LN10 2.30258509299404568401799145468436421     /* loge(10)       */
#define M_PI 3.14159265358979323846264338327950288       /* pi             */
#define M_PI_2 1.57079632679489661923132169163975144     /* pi/2           */
#define M_PI_4 0.785398163397448309615660845819875721    /* pi/4           */
#define M_1_PI 0.318309886183790671537767526745028724    /* 1/pi           */
#define M_2_PI 0.636619772367581343075535053490057448    /* 2/pi           */
#define M_2_SQRTPI 1.12837916709551257389615890312154517 /* 2/sqrt(pi)     */
#define M_SQRT2 1.41421356237309504880168872420969808    /* sqrt(2)        */
#define M_SQRT1_2 0.707106781186547524400844362104849039 /* 1/sqrt(2)      */

#define MAXFLOAT 0x1.fffffep+127f

/* The values returned by `ilogb' for 0 and NaN respectively. */
#define FP_ILOGB0 (-2147483647 - 1)
#define FP_ILOGBNAN (-2147483647 - 1)

/* Bitmasks for the math_errhandling macro.  */
#define MATH_ERRNO 1     /* errno set by math functions.  */
#define MATH_ERREXCEPT 2 /* Exceptions raised by math functions.  */

#define HUGE_VAL 1e500
#define HUGE_VALF 1e50f
#define HUGE_VALL 1e5000L
#define NAN __nan()

#define INFINITY HUGE_VALF

/******************************************************************************
 *      Taxonomy of floating point data types                                 *
 ******************************************************************************/

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5
