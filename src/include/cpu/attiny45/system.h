#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#define interruptDisable() asm volatile("cli")
#define interruptEnable() asm volatile("sei")


#endif /* __SYSTEM_H__ */
