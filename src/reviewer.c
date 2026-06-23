#include "reviewer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- FNV-1a 64-bit ----------------------------------------------------
 * Cheap, dependency-free, deterministic. "Long hash" enough for satire. */
static unsigned long long fnv1a64(const void *data, size_t len, unsigned long long seed) {
    const unsigned char *p = (const unsigned char *)data;
    unsigned long long h = 1469598103934665603ULL ^ seed;
    for (size_t i = 0; i < len; i++) {
        h ^= p[i];
        h *= 1099511628211ULL;
    }
    return h;
}

/* Map a 64-bit hash to an integer in [lo, hi]. */
static int hash_range(unsigned long long h, int lo, int hi) {
    int span = hi - lo + 1;
    return lo + (int)(h % (unsigned long long)span);
}

/* Decide who actually reviewed the paper. */
static Reader pick_reader(unsigned long long h) {
    int roll = hash_range(h, 1, 100);
    if (roll <= 40) return READER_REVIEWER;
    if (roll <= 75) return READER_STUDENT;
    return READER_NOBODY;
}

void reviewer_review(const char *reviewer_name,
                     const Abstract *a,
                     Review *out)
{
    /* Seed the hash with the reviewer's name so each reviewer gets a
     * different "objective" score from the same abstract. */
    unsigned long long name_seed =
        fnv1a64(reviewer_name, strlen(reviewer_name), 0);

    unsigned long long h_main =
        fnv1a64(a->text, a->length, name_seed);

    /* Derive several independent-ish numbers by re-hashing with salts. */
    unsigned long long h_pct  = fnv1a64("accept", 6, h_main);
    unsigned long long h_coin = fnv1a64("coin",   4, h_main);
    unsigned long long h_conf = fnv1a64("conf",   4, h_main);
    unsigned long long h_who  = fnv1a64("who",    3, h_main);

    int accept_pct = hash_range(h_pct, 1, 99);
    int coin       = hash_range(h_coin, 1, 100);
    int confidence = hash_range(h_conf, 1, 5);
    Reader reader  = pick_reader(h_who);

    out->name        = reviewer_name;
    out->reader      = reader;
    out->accept_pct  = accept_pct;
    out->verdict     = (coin <= accept_pct) ? VERDICT_ACCEPT : VERDICT_REJECT;
    out->confidence  = confidence;
}

static const char *reader_label(Reader r) {
    switch (r) {
        case READER_REVIEWER: return "reviewer (read it carefully)";
        case READER_STUDENT:  return "grad student (skimmed during lab meeting)";
        case READER_NOBODY:   return "nobody (verdict generated on the flight)";
    }
    return "unknown";
}

void reviewer_print(const Review *r) {
    printf("  reviewer:    %s\n", r->name);
    printf("  read by:     %s\n", reader_label(r->reader));
    printf("  accept odds: %d%% \n", r->accept_pct);
    printf("  verdict:     %s\n",
           r->verdict == VERDICT_ACCEPT ? "ACCEPT" : "REJECT");
    printf("  confidence:  %d/5\n", r->confidence);
}
