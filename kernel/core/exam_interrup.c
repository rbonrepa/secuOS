#include <exam_interrup.h>
#include <exam_segment.h>
#include <exam_task.h>
#include <exam_layout.h>
#include <debug.h>

extern tss_t * TSS;
extern int current_task_index;
extern task_t tasks[NB_TASKS];

// Syscall pour afficher la valeur du compteur
// Interface Noyau
void __regparm__(1) kernel_handler(int_ctx_t * ctx)
{
    uint32_t* compteur = (uint32_t*) ctx->gpr.esi.raw;
    debug(">> Counter = %p\n", compteur);
}

// Syscall pour changer de task
__attribute__((naked)) void user_handler()
{
    set_ds(d3_sel);
    set_es(d3_sel);
    set_fs(d3_sel);
    set_gs(d3_sel);

    task_t *task = &tasks[current_task_index];

    // Sauvegarder contexte ?
    asm volatile(
        "mov (%%esp), %0      \n"
        "mov (%%ebp), %1      \n"
        "mov 4(%%ebp), %2     \n"
        : "=r"(task->esp_kernel), "=r"(task->ebp), "=r"(task->eip));

    current_task_index = (current_task_index + 1) % 2;
    task = &tasks[current_task_index];

    asm volatile(
        "mov %0, (%%esp)      \n"
        "mov %1, (%%ebp)      \n"
        :: "r"(task->esp_kernel), "r"(task->ebp));
    
    TSS->s0.esp = task->esp_kernel;
    // set_esp(task->esp_kernel);
    set_cr3(task->pgd);

    asm volatile(
        "push %0          \n" 
        "push %1          \n" 
        "pushf            \n"
        "push %2          \n"
        "push %%ebx       \n"
        "iret             \n"
        ::"i"(d3_sel), "r"(task->esp_user),"i"(c3_sel),"b"((void*)task->eip));

    // asm volatile("popa");     
    // asm volatile("add $8, %esp"); // skip int number end error code
    // asm volatile("iret");
}

void init_interrup(int num_inter, int privilege, offset_t handler)
{
    idt_reg_t idtr;
    get_idtr(idtr);

    int_desc_t *iface_dsc = &idtr.desc[num_inter];
    int_desc(iface_dsc, c0_sel, handler);
    iface_dsc->type = SEG_DESC_SYS_TRAP_GATE_32;
    iface_dsc->dpl = privilege;
}

void init_all_interrup()
{
    init_interrup(32, 0, (offset_t)user_handler);
    init_interrup(80, 3, (offset_t)kernel_handler);
}