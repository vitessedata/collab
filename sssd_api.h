#ifndef SSSD_API_H
#define SSSD_API_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <memory>
#include "xrg.h"

namespace sssd_engine {
enum expr_type_t {
    EXPR_UNKNOWN,
    EXPR_LIT_FIELD, // the column data
    EXPR_LIT_STRING,
    EXPR_LIT_I8,
    EXPR_LIT_I16,
    EXPR_LIT_I32,
    EXPR_LIT_I64,
    EXPR_LIT_I128,
    EXPR_LIT_FP32,
    EXPR_LIT_FP64,
    EXPR_LIT_DEC64,
    EXPR_LIT_DEC128,
    EXPR_LIT_DATE,
    EXPR_LIT_TIME,
    EXPR_LIT_TIMESTAMP,
    EXPR_LIT_INTERVAL,
    EXPR_CAST_STRING,
    EXPR_CAST_I8,
    EXPR_CAST_I16,
    EXPR_CAST_I32,
    EXPR_CAST_I64,
    EXPR_CAST_I128,
    EXPR_CAST_FP32,
    EXPR_CAST_FP64,
    EXPR_CAST_BYTEA,
    EXPR_CAST_DEC64,
    EXPR_CAST_DEC128,
    EXPR_CAST_DATE,
    EXPR_CAST_TIME,
    EXPR_CAST_TIMESTAMP,
    // op expects the same types on LHS/RHS, except (timestamp x interval)
    EXPR_OP_ADD,   // A+B
    EXPR_OP_SUB,   // A-B
    EXPR_OP_MINUS, // -A
    EXPR_OP_MUL,   // A*B
    EXPR_OP_DIV,   // A/B
    EXPR_OP_MOD,   // A%B
    EXPR_OP_POW,   // Pow(A, B)
    EXPR_OP_LOG,   // Log(A, B)
    // logical op
    EXPR_OP_AND,
    EXPR_OP_OR,
    EXPR_OP_IN,
    // compare expects the same types on LHS/RHS
    EXPR_CMP_EQ,
    EXPR_CMP_LT,
    EXPR_CMP_LE,
    EXPR_CMP_GT,
    EXPR_CMP_GE,
    EXPR_CMP_NE,
};

class expr_t {
   public:
    expr_type_t type;
    // std::vector<std::unique_ptr<expr_t>> arg;
    std::vector<expr_t*> arg;
    virtual ~expr_t() = default;
};

typedef struct lit_data_t {
    union {
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        __int128_t i128;
        float fp32;
        double fp64;
    };
    std::vector<char> bytea;
    union {
        struct {
            // for decimal lit only
            int16_t scale;
            int16_t precision;
        };
    };
} lit_data_t;

class lit_expr_t : public expr_t {
   public:
    lit_data_t data;
};

class cast_expr_t : public expr_t {
   public:
    struct {
        // for decimal cast only
        int16_t scale;
        int16_t precision;
    };
};

// -----------------------------------------------------------------
/* example:

for Field < '2020-01-01' and the field is a date

       (EXPR_CMP_LT
           (EXPR_LIT_FIELD)
           (EXPR_LIT_DATE 2020-01-01))

for Field < '2020-01-01' and the field is a timestamp

       (EXPR_CMP_LT
           (EXPR_CAST_DATE (EXPR_LIT_FIELD))
           (EXPR_LIT_DATE 2020-01-01))


OR   ????????????

        (EXPR_CMP_LT
                (EXPR_LIT_FIELD)
                (EXPR_CAST_TIMESTAMP
                        (EXPR_LIT_DATE 2020-01-01)))



for field between 1 and 10 where field is an int32

        (EXPR_OP_AND
            (EXPR_CMP_GE
                (EXPR_LIT_FIELD)
                (EXPR_LIT_I32 1))
            (EXPR_CMP_LE
                (EXPR_LIT_FIELD)
                (EXPR_LIT_I32 10)))


 */

typedef enum {
    APPEND_FV_VV = 0,
    APPEND_DV = 1,
    READ_FV_VV = 2,
    READ_DV = 3, /* 8-bit */
} request_type_t;

// if filter feature is not supported, then return data without doing filtering and rc = SSSD_FILTER_NOTDO
typedef enum {
    SSSD_SUCCESS = 0,
    SSSD_CFG_ERR = 1,
    SSSD_DEV_ERR = 2,
    SSSD_MEM_ERR = 3,
    SSSD_FILE_ERR = 4,
    SSSD_FILTER_NOTDO = 5
} sssd_rc_t;

class request_t {
   public:
    int request_type;
    std::string path;
    virtual ~request_t() = default;
};
class read_request_t : public request_t {
   public:
    int32_t cno;
    // std::unique_ptr<expr_t> filter;
    expr_t* filter;
};
class append_request_t : public request_t {
   public:
    std::vector<int8_t> columndata;
};
class reply_t {
   public:
    int reply_type;
    int rc;
    std::string errmsg;
    virtual ~reply_t() = default;
};
class read_reply_t : public reply_t {
   public:
    std::vector<int8_t> data;
};
class append_reply_t : public reply_t {
   public:
    int64_t offset;
};

typedef struct task_t {
    std::unique_ptr<request_t> request;
    std::unique_ptr<reply_t> reply;
} task_t;
typedef void* sssd_t;
/**
 * Finalize
 */
void sssd_final(sssd_t handle);

/**
 * Initialize with input device-ID and mount paths.
 * The format of disk[] should be: "INTEGER_ID:/ABSOLUTE/PATH", e.g. "0:/mnt/disk0"
 */
sssd_t sssd_init(int ndisk, const char* disk[], char* errmsg, int errsz);

/**
 * Returns the mountpoint of sssd[0], ...
 * If idx < 0 or if idx >= #devices, returns NULL.
 * Note: the caller should call free() on the returned pointer.
 */
const char* sssd_mountpoint(sssd_t sssd, int idx);
/**
 *  Submit a task to sssd; returns 0 on success, -1 otherwise.
 *  The task will be queued to run in the future.
 *  Tasks are malloc-ed by caller, and pass to sssd.
 */
int sssd_submit(sssd_t sssd, task_t* task, char* errmsg, int errsz);

/**
 *  Await the completion of tasks. Returns #tasks completed, or -1
 *  otherwise. This call is blocking if nonblocking == 0.
 *
 *  A completed task may have finished successfully or not.
 *
 *  For example, the caller may await 5 tasks, out of which only 2
 *  tasks are completed when this call returns. The caller shall check
 *  the status[] to determine which of the 5 tasks have completed.
 *  e.g. if tasks 2 and 5 are completed, then status will be set to
 *  [0, 1, 0, 0, 1]. The caller then checks the rc code in the task[1]
 *  and task[4] to determine if the particular task was completed
 *  successfully or not. The caller should remove and free the
 *  completed tasks and not resubmit them to sssd_await() in the next
 *  iteration.
 *
 *  If nonblocking is true, this function may return 0.
 */
int sssd_await(
    sssd_t sssd, int ntask, task_t* task[], bool status[], int nonblocking, char* errmsg, int errsz);

/** Check the filter condition, return true if it is supported by sssd, false otherwise
 *
 */
bool sssd_is_expr_supported(sssd_t sssd, int ptyp, int ltyp, const expr_t* filter);

} // end of namespace
#endif
