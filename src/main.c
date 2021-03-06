#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <moar.h>

#if MVM_TRACING
#  define TRACING_OPT "[--tracing] "
#  define TRACING_USAGE "\n    --tracing  output a line to stderr on every interpreter instr"
#else
#  define TRACING_OPT ""
#  define TRACING_USAGE ""
#endif

/* flags need to be sorted alphabetically */

enum {
    NOT_A_FLAG = -2,
    UNKNOWN_FLAG = -1,

    FLAG_CRASH,
    FLAG_DUMP,
    FLAG_HELP,
    FLAG_TRACING,
    FLAG_VERSION,

    OPT_LIBPATH
};

static const char *const FLAGS[] = {
    "--crash",
    "--dump",
    "--help",
    "--tracing",
    "--version",
};

static const char USAGE[] = "\
USAGE: moar [--dump] [--crash] [--libpath=...] " TRACING_OPT "input.moarvm [program args]\n\
       moar [--help]\n\
\n\
    --help     display this message\n\
    --dump     dump the bytecode to stdout instead of executing\n\
    --crash    abort instead of exiting on unhandled exception\n\
    --libpath  specify path loadbytecode should search in\n\
    --version  show version information"
    TRACING_USAGE;

static int cmp_flag(const void *key, const void *value)
{
    return strcmp(key, *(char **)value);
}

static int starts_with(const char *str, const char *want) {
    size_t str_len  = strlen(str);
    size_t want_len = strlen(want);
    return str_len < want_len
        ? 0
        : strncmp(str, want, want_len) == 0;
}

static int parse_flag(const char *arg)
{
    const char *const *found;

    if (!arg || arg[0] != '-')
        return NOT_A_FLAG;

    found = bsearch(arg, FLAGS, sizeof FLAGS / sizeof *FLAGS, sizeof *FLAGS, cmp_flag);

    if (found)
        return (int)(found - FLAGS);
    else if (starts_with(arg, "--libpath="))
        return OPT_LIBPATH;
    else
        return UNKNOWN_FLAG;
}

int main(int argc, char *argv[])
{
    MVMInstance *instance;
    const char  *input_file;
    const char  *lib_path[8];

    int dump = 0;
    int argi = 1;
    int flag;
    int lib_path_i = 0;

    for (; (flag = parse_flag(argv[argi])) != NOT_A_FLAG; ++argi) {
        switch (flag) {
            case FLAG_CRASH:
            MVM_crash_on_error();
            continue;

            case FLAG_DUMP:
            dump = 1;
            continue;

            case FLAG_HELP:
            puts(USAGE);
            return EXIT_SUCCESS;

#if MVM_TRACING
            case FLAG_TRACING:
            MVM_interp_enable_tracing();
            continue;
#endif

            case OPT_LIBPATH:
            if (lib_path_i == 7) { /* 0..7 == 8 */
                fprintf(stderr, "ERROR: Only up to eight --libpath options are allowed.\n");
                return EXIT_FAILURE;
            }

            lib_path[lib_path_i++] = argv[argi] + strlen("--libpath=");
            continue;

            case FLAG_VERSION:
            printf("This is MoarVM version %s\n", MVM_VERSION);
            return EXIT_SUCCESS;

            default:
            fprintf(stderr, "ERROR: Unknown flag %s.\n\n%s\n", argv[argi], USAGE);
            return EXIT_FAILURE;
        }
    }

    lib_path[lib_path_i] = NULL;

    if (argi >= argc) {
        fprintf(stderr, "ERROR: Missing input file.\n\n%s\n", USAGE);
        return EXIT_FAILURE;
    }

    instance   = MVM_vm_create_instance();
    input_file = argv[argi++];

    /* stash the rest of the raw command line args in the instance */
    instance->num_clargs = argc - argi;
    instance->raw_clargs = argv + argi;
    instance->prog_name  = input_file;
    for( argi = 0; argi < lib_path_i; argi++)
        instance->lib_path[argi] = lib_path[argi];

    if (dump) MVM_vm_dump_file(instance, input_file);
    else MVM_vm_run_file(instance, input_file);

    MVM_vm_destroy_instance(instance);

    return EXIT_SUCCESS;
}
