#include "oaq_api.h"
#include "parser.h"
#include "reviewer.h"

#include <string.h>

/* These already exist in your analysis.c / submission.c. */
int acronym_score(const Abstract *a);
int symbol_score (const Abstract *a);
int lexile_score (const Abstract *a);

/* Pull conference_tier out of analysis.c into a header, OR duplicate the
 * logic here. For brevity, assume you exposed it via analysis.h. */
const char *conference_tier(int score);

static const char *REVIEWERS[] = {
    "Nicholas Cassarino",
    "Luke Albertson",
    "Justin Bosse",
    "Keegan Merck",
};

int oaq_analyze(const char *text, size_t length, OaqAnalysis *out) {
    if (text == NULL || out == NULL) return 1;

    Abstract a = { .text = (char *)text, .length = length };

    out->acronym_score = acronym_score(&a);
    out->symbol_score  = symbol_score (&a);
    out->lexile_score  = lexile_score (&a);
    out->total_score   = out->acronym_score + out->symbol_score + out->lexile_score;

    const char *venue = conference_tier(out->total_score);
    strncpy(out->venue, venue, sizeof(out->venue) - 1);
    out->venue[sizeof(out->venue) - 1] = '\0';
    return 0;
}

int oaq_submit(const char *text, size_t length, OaqSubmission *out) {
    if (text == NULL || out == NULL) return 1;

    Abstract a = { .text = (char *)text, .length = length };

    out->n_reviews = 4;
    out->n_accepts = 0;

    for (int i = 0; i < 4; i++) {
        Review r;
        reviewer_review(REVIEWERS[i], &a, &r);

        strncpy(out->reviews[i].name, r.name, sizeof(out->reviews[i].name) - 1);
        out->reviews[i].name[sizeof(out->reviews[i].name) - 1] = '\0';
        out->reviews[i].reader     = (int)r.reader;
        out->reviews[i].accept_pct = r.accept_pct;
        out->reviews[i].verdict    = (int)r.verdict;
        out->reviews[i].confidence = r.confidence;

        if (r.verdict == VERDICT_ACCEPT) out->n_accepts++;
    }

    const char *decision;
    if      (out->n_accepts >= 3) decision = "ACCEPT (congrats)";
    else if (out->n_accepts == 2) decision = "MAJOR REVISION (AC coin flip pending)";
    else if (out->n_accepts == 1) decision = "REJECT (resubmit to a workshop)";
    else                          decision = "DESK REJECT (gg)";

    strncpy(out->decision, decision, sizeof(out->decision) - 1);
    out->decision[sizeof(out->decision) - 1] = '\0';
    return 0;
}

int oaq_analyze_file(const char *path, OaqAnalysis *out) {
    Abstract a = {0};
    if (abstract_load(path, &a) != 0) return 1;
    int rc = oaq_analyze(a.text, a.length, out);
    abstract_free(&a);
    return rc;
}

int oaq_submit_file(const char *path, OaqSubmission *out) {
    Abstract a = {0};
    if (abstract_load(path, &a) != 0) return 1;
    int rc = oaq_submit(a.text, a.length, out);
    abstract_free(&a);
    return rc;
}
