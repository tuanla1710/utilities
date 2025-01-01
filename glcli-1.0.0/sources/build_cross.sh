# /*
#  * PICOCOM PROPRIETARY INFORMATION
#  *
#  * This software is supplied under the terms of a license agreement or
#  * nondisclosure agreement with PICOCOM and may not be copied
#  * or disclosed except in accordance with the terms of that agreement.
#  *
#  * Copyright PICOCOM.
#  */

#!/bin/bash
# set -x
source /opt/picocom/tools/pc805_common/sdk/4.0.13/environment-setup-riscv32-poky-linux
# env | grep CC

make clean
make all -j8
