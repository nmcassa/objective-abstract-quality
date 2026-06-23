#include "parser.h"

typedef enum {
    VERDICT_ACCEPT,
    VERDICT_REJECT
} Verdict;

typedef enum {
    READER_REVIEWER,   /* the assigned reviewer actually read it */
    READER_STUDENT,    /* a grad student got volun-told */
    READER_NOBODY      /* nobody read it; verdict generated anyway */
} Reader;

typedef struct {
    const char *name;     /* reviewer's name */
    Reader      reader;   /* who actually "read" the paper */
    int         accept_pct;  /* 1..99 */
    Verdict     verdict;
    int         confidence;  /* 1..5 */
} Review;

void reviewer_review(const char *reviewer_name,
                     const Abstract *a,
                     Review *out);

void reviewer_print(const Review *r);
