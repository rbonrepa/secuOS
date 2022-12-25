/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <string.h>

extern info_t *info;
void userland();

void tp() {
  printf("Hello TP3\n");
  printf("Edit GDT\n");
  gdt_reg_t gdtr;

  // Création de notre propre table gdt
  seg_desc_t *new_table = 0x0;

  // Segment vide rempli de 0, indice 0
  for (uint32_t i = 0; i < sizeof(seg_desc_t); i++) {
    *((uint8_t *)new_table + i) = 0;
  }

  new_table++;

 // Code, 32 bits RX, flat, ring 0, indice 1
  new_table->limit_1 = 0xffff;
  new_table->base_1 = 0x0;
  new_table->base_2 = 0x0;
  new_table->type = SEG_DESC_CODE_XR;
  new_table->s = 1;
  new_table->dpl = 0;
  new_table->p = 1;
  new_table->limit_2 = 0xf;
  new_table->avl = 0;
  new_table->l = 0;
  new_table->d = 1;
  new_table->g = 1;
  new_table->base_3 = 0x00;

  new_table++;

  // Données, 32 bits RW, flat, ring 0, indice 2
  new_table->limit_1 = 0xffff;
  new_table->base_1 = 0x0000;
  new_table->base_2 = 0x0000;
  new_table->type = SEG_DESC_DATA_RW;
  new_table->s = 1;
  new_table->dpl = 3;
  new_table->p = 1;
  new_table->limit_2 = 0xf;
  new_table->avl = 0;
  new_table->l = 0;
  new_table->d = 1;
  new_table->g = 1;
  new_table->base_3 = 0x00;


  // Code, 32 bits RX, flat, ring 3, indice 3
  new_table->limit_1 = 0xffff;
  new_table->base_1 = 0x0;
  new_table->base_2 = 0x0;
  new_table->type = SEG_DESC_CODE_XR;
  new_table->s = 1;
  new_table->dpl = 3;
  new_table->p = 1;
  new_table->limit_2 = 0xf;
  new_table->avl = 0;
  new_table->l = 0;
  new_table->d = 1;
  new_table->g = 1;
  new_table->base_3 = 0x00;

  new_table++;

  // Données, 32 bits RW, flat, ring 3, indice 4
  new_table->limit_1 = 0xffff;
  new_table->base_1 = 0x0000;
  new_table->base_2 = 0x0000;
  new_table->type = SEG_DESC_DATA_RW;
  new_table->s = 1;
  new_table->dpl = 3;
  new_table->p = 1;
  new_table->limit_2 = 0xf;
  new_table->avl = 0;
  new_table->l = 0;
  new_table->d = 1;
  new_table->g = 1;
  new_table->base_3 = 0x00;

  new_table -= 5;

  gdtr.limit = 3 * sizeof(seg_desc_t) - 1;
  gdtr.desc = new_table;

  set_gdtr(gdtr);

  printf("GDTR edited\n");

  printf("Loading DS\n");
  /*seg_sel_t ds;
  ds.rpl = 0;
  ds.ti = 0;
  ds.index = 2;
*/
  set_ds(gdt_seg_sel(2, 0)) ;
  set_gs(gdt_seg_sel(2, 0)) ;
  set_fs(gdt_seg_sel(2, 0)) ;
  set_es(gdt_seg_sel(2, 0)) ;
  set_cs(gdt_seg_sel(1, 0)) ;

  printf("Sélecteurs en ring 0\n\n");
  set_ds(gdt_seg_sel(4, 3)) ;
  set_gs(gdt_seg_sel(4, 3)) ;
  set_fs(gdt_seg_sel(4, 3)) ;
  set_es(gdt_seg_sel(4, 3)) ;
  set_cs(gdt_seg_sel(3, 3)) ;

  printf("Sélecteurs en ring 3\n\n");

}

void userland() { asm volatile("mov %eax, %cr0"); }