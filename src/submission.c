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

    int accepts = 0;
    for (size_t i = 0; i < N_REVIEWERS; i++) {
        Review r;
        reviewer_review(REVIEWERS[i], a, &r);

        printf("---- Review %zu ----\n", i + 1);
        reviewer_print(&r);
        printf("\n");

        if (r.verdict == VERDICT_ACCEPT) accepts++;
    }

    printf("=========================================\n");
    printf(" final tally: %d accept / %zu reject\n",
           accepts, N_REVIEWERS - accepts);

    const char *decision;
    if      (accepts >= 3) decision = "ACCEPT (congratulations)";
    else if (accepts == 2) decision = "MAJOR REVISION (the AC will decide on a coin flip of their own)";
    else if (accepts == 1) decision = "REJECT (resubmit to a workshop)";
    else                   decision = "DESK REJECT";
    printf(" area chair says: %s\n", decision);
    printf("=========================================\n");

    return 0;
}
