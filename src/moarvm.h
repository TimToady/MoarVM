#include <stdlib.h>

/* Pull in the APR. */
#define APR_DECLARE_STATIC 1
#include <apr_general.h>
#include <apr_atomic.h>
#include <apr_hash.h>
#include <apr_file_info.h>
#include <apr_file_io.h>
#include <apr_mmap.h>

/* Configuration. */
#include "gen/config.h"

/* Headers for APIs for various other data structures and APIs. */
#include "6model/6model.h"
#include "core/threadcontext.h"
#include "core/instance.h"
#include "core/args.h"
#include "core/exceptions.h"
#include "core/interp.h"
#include "core/frame.h"
#include "core/validation.h"
#include "core/compunit.h"
#include "core/bytecode.h"
#include "core/ops.h"
#include "6model/reprs.h"
#include "6model/bootstrap.h"
#include "gc/allocation.h"
#include "gc/nursery.h"
#include "gc/roots.h"
#include "gc/wb.h"
#include "strings/ascii.h"
#include "strings/utf8.h"
#include "strings/ops.h"

/* Top level VM API functions. */
MVMInstance * MVM_vm_create_instance(void);
void MVM_vm_run_file(MVMInstance *instance, char *filename);
void MVM_vm_destroy_instance(MVMInstance *instance);
