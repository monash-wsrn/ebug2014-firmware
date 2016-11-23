#ifndef `$INSTANCE_NAME`_H
#define `$INSTANCE_NAME`_H
#include <project.h>

void `$INSTANCE_NAME`_Start();
void `$INSTANCE_NAME`_SetOffsets(uint8 X1,uint8 Y1,uint8 X2,uint8 Y2);
void `$INSTANCE_NAME`_Sync();
int `$INSTANCE_NAME`_LeftSpeed();
int `$INSTANCE_NAME`_RightSpeed();

#define `$INSTANCE_NAME`_LeftCount (*(volatile int16*)`$INSTANCE_NAME`_Quadrature_L_dp_u0__A0_A1_REG)
#define `$INSTANCE_NAME`_RightCount (*(volatile int16*)`$INSTANCE_NAME`_Quadrature_R_dp_u0__A0_A1_REG)

#endif
