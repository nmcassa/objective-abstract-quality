#include "parser.h"
#include "analysis.h"

const char *conference_tier(int score) {
    if (score >= 55) return "SC (Supercomputing) — best paper trajectory";
    if (score >= 40) return "SC / IPDPS (top tier)";
    if (score >= 25) return "PPoPP / ISC / HPDC (solid A-tier)";
    if (score >= 10) return "PACT / ICPP (reputable)";
    if (score >= 5) return "Cluster / CCGrid workshop track";
    return "student poster session";
}

// Score based on both length of acronyms
//   and total count of acronyms
int acronym_score(const Abstract *a) {
    int count = 0;
    int sum_length = 0;
    int run = 0;

    for (size_t i = 0; i < a->length; i++) {
        int is_upper = isupper((unsigned char)a->text[i]);

        if (is_upper)
            run++;
        else {
            if (run > 1) {
                count++;
                sum_length += run;
            }   
            run = 0;
        } 
    }

    if (a->length < 100) 
        return count * 3 + sum_length;

    return (count * 3 + sum_length) / ((int) a->length / 100);
}

// Score based on the density of numeric digits and symbol characters.
// Symbols = anything that isn't a letter, digit, or whitespace
//   (e.g. = + - * / % ^ < > ( ) [ ] { } \ | _ ~ etc.)
int symbol_score(const Abstract *a) {
    int digits  = 0;
    int symbols = 0;

    for (size_t i = 0; i < a->length; i++) {
        unsigned char c = (unsigned char)a->text[i];

        if (isdigit(c)) {
            digits++;
        } else if (!isalpha(c) && !isspace(c)) {
            symbols++;
        }
    }

    /* Same normalization shape as acronym_score: per ~100 bytes. */
    if (a->length < 100) {
        return digits * 2 + symbols * 3;
    }
    return (digits * 2 + symbols * 3) / ((int)a->length / 100);
}

// Score based on reading difficulty, approximated via the
// Automated Readability Index (ARI):
//   ARI = 4.71 * (chars/words) + 0.5 * (words/sentences) - 21.43
// Higher ARI ~ harder to read ~ more "scholarly".
int lexile_score(const Abstract *a) {
    int chars     = 0;
    int words     = 0;
    int sentences = 0;

    int in_word = 0;

    for (size_t i = 0; i < a->length; i++) {
        unsigned char c = (unsigned char)a->text[i];

        /* "chars" per ARI = letters + digits, no spaces or punctuation. */
        if (isalnum(c)) {
            chars++;
            if (!in_word) {
                words++;
                in_word = 1;
            }
        } else {
            in_word = 0;
            if (c == '.' || c == '!' || c == '?') {
                sentences++;
            }
        }
    }

    /* Guard against malformed input (e.g. an abstract with no punctuation,
     * which, frankly, would not be the strangest thing a reviewer has seen). */
    if (words == 0)     words = 1;
    if (sentences == 0) sentences = 1;

    double ari = 4.71 * ((double)chars / words)
               + 0.5  * ((double)words / sentences)
               - 21.43;

    if (ari < 0.0) ari = 0.0;

    return (int)ari;
}

int run_analysis(const Abstract *a) {
    printf("[analysis mode] loaded abstract (%zu bytes)\n", a->length);

    printf("-- acronyms --\n");
    int acr = acronym_score(a);
    printf("acronym score:  %d\n\n", acr);

    printf("-- symbols & numerics --\n");
    int sym = symbol_score(a);
    printf("symbol score:   %d\n\n", sym);

    printf("-- lexile (ARI) --\n");
    int lex = lexile_score(a);
    printf("lexile score:   %d\n\n", lex);

    int total = acr + sym + lex;

    printf("=========================================\n");
    printf(" objective abstract quality score: %d\n", total);
    printf(" predicted venue: %s\n", conference_tier(total));
    printf("=========================================\n");

    return 0;
}
