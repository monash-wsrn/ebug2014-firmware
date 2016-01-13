#ifndef `$INSTANCE_NAME`_H
#define `$INSTANCE_NAME`_H
#include "cyfitter.h"
#include "cytypes.h"

#define `$INSTANCE_NAME`_TX (*(reg8*)`$INSTANCE_NAME`_dp_u0__F0_REG)
#define `$INSTANCE_NAME`_RX (*(reg8*)`$INSTANCE_NAME`_dp_u0__F1_REG)

void `$INSTANCE_NAME`_Start();

#endif
