#include "parser.h"
#include "reviewer.h"

static const char *REVIEWERS[] = {
    "Nicholas Cassarino",
    "Luke Albertson",
    "Justin Bosse",
    "Keegan Merck",
};
#define N_REVIEWERS (sizeof(REVIEWERS) / sizeof(REVIEWERS[0]))

int run_submission(const Abstract *a) {
    printf("[submission mode] loaded abstract (%zu bytes)\n", a->length);
    printf("notifying four reviewers...\n\n");

    double accept_conf = 0;
    int accept_count = 0;
    for (size_t i = 0; i < N_REVIEWERS; i++) {
        Review r;
        reviewer_review(REVIEWERS[i], a, &r);

        printf("---- Review %zu ----\n", i + 1);
        reviewer_print(&r);
        printf("\n");

        if (r.verdict == VERDICT_ACCEPT) {
            accept_conf += ((double) r.confidence / 5); 
            accept_count++;
        } else
            accept_conf -= ((double) r.confidence / 5);
    }

    printf("=========================================\n");
    printf(" final tally: %d accept / %zu reject\n",
           accept_count, N_REVIEWERS - accept_count);
    printf(" accepts * confidence: %.2f\n", accept_conf);

    const char *decision;
    if      (accept_conf >= 1.0) decision = "ACCEPT (congratulations)";
    else if (accept_conf >= 0.5) decision = "MAJOR REVISION (the AC will decide on a coin flip of their own)";
    else if (accept_conf >= -0.5) decision = "REJECT (resubmit to a workshop)";
    else                   decision = "DESK REJECT";
    printf(" area chair says: %s\n", decision);
    printf("=========================================\n");

    return 0;
}
