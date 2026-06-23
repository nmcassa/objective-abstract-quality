#include "parser.h"
#include "reviewer.h"
#include "analysis.h"
#include "submission.h"

typedef enum {
    MODE_UNKNOWN,
    MODE_SUBMISSION,
    MODE_ANALYSIS
} Mode;

static void print_usage(const char *prog) {
    fprintf(stderr,
        "objective-abstract-quality\n"
        "\n"
        "usage: %s <mode> <path-to-abstract.txt>\n"
        "\n"
        "modes:\n"
        "  submission   four highly qualified reviewers will objectively\n"
        "               evaluate your work (results may vary, by design)\n"
        "  analysis     our advanced AI computes lexile, symbol density,\n"
        "               and acronym metrics to predict conference quality\n",
        prog);
}

static Mode parse_mode(const char *arg) {
    if (strcmp(arg, "submission") == 0) return MODE_SUBMISSION;
    if (strcmp(arg, "analysis")   == 0) return MODE_ANALYSIS;
    return MODE_UNKNOWN;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    Mode mode = parse_mode(argv[1]);
    if (mode == MODE_UNKNOWN) {
        fprintf(stderr, "error: unknown mode '%s'\n\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }

    Abstract abstract = {0};
    if (abstract_load(argv[2], &abstract) != 0) {
        return 1;
    }

    int rc = 0;
    switch (mode) {
        case MODE_SUBMISSION: rc = run_submission(&abstract); break;
        case MODE_ANALYSIS:   rc = run_analysis(&abstract);   break;
        default:              rc = 1; break;
    }

    abstract_free(&abstract);
    return rc;
}
