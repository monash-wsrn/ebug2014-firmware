#ifndef `$INSTANCE_NAME`_H
#define `$INSTANCE_NAME`_H
#include <project.h>
#define `$INSTANCE_NAME`_lengthA `$INSTANCE_NAME`_Bit_Counter_lengthA
#define `$INSTANCE_NAME`_lengthB `$INSTANCE_NAME`_Bit_Counter_lengthB
#define `$INSTANCE_NAME`_state0A `$INSTANCE_NAME`_Bit_Predictor_A_state0
#define `$INSTANCE_NAME`_state1A `$INSTANCE_NAME`_Bit_Predictor_A_state1
#define `$INSTANCE_NAME`_state0B `$INSTANCE_NAME`_Bit_Predictor_B_state0
#define `$INSTANCE_NAME`_state1B `$INSTANCE_NAME`_Bit_Predictor_B_state1
#define `$INSTANCE_NAME`_Start() { `$INSTANCE_NAME`_FSK_RX_A_Start(); `$INSTANCE_NAME`_FSK_RX_B_Start(); }
#endif
