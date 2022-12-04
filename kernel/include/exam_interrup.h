#ifndef EXAM_INTERRUP
#define EXAM_INTERRUP
#include <cr.h>
#include <intr.h>
#include <info.h>

void init_all_interrup();
void __regparm__(1) kernel_handler(int_ctx_t * ctx);
void user_handler();

#endif