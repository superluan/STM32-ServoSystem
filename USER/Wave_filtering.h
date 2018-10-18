#ifndef __Wave_filtering_H
#define	__Wave_filtering_H

#include "stm32f10x.h"
#define uchar unsigned char
u16 Middle(void);
u16 ArithmeticalAverageValueFilter(void);
u16 Gilde(uchar Data[]);
unsigned int filter(void);


#endif /* __Wave_filtering_H */
