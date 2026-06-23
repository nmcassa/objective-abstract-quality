#ifndef OAQ_API_H
#define OAQ_API_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Analysis result returned to Python. All ints; no allocations to free. */
typedef struct {
    int acronym_score;
    int symbol_score;
    int lexile_score;
    int total_score;
    /* Fixed-size buffer so Python doesn't have to free anything. */
    char venue[128];
} OaqAnalysis;

/* Per-reviewer result. */
typedef struct {
    char name[64];
    int  reader;        /* 0=reviewer, 1=student, 2=nobody */
    int  accept_pct;    /* 1..99 */
    int  verdict;       /* 0=accept, 1=reject */
    int  confidence;    /* 1..5 */
} OaqReview;

typedef struct {
    OaqReview reviews[4];
    int       n_reviews;     /* always 4, but explicit */
    int       n_accepts;
    char      decision[128];
} OaqSubmission;

/* Analyze raw abstract bytes. Returns 0 on success. */
int oaq_analyze(const char *text, size_t length, OaqAnalysis *out);

/* Submit raw abstract bytes. Returns 0 on success. */
int oaq_submit (const char *text, size_t length, OaqSubmission *out);

int oaq_analyze_file(const char *path, OaqAnalysis   *out);
int oaq_submit_file (const char *path, OaqSubmission *out);

#ifdef __cplusplus
}
#endif

#endif
