
### Input-output correlations for scom filtering and invalid address check feature

| **_Sl<br>No_** | **_Input via<br>Secure Header_** |        **_Input via <br>ATTR_OCMB_BOOT_FLAGS_**       |           **_Input via <br>ATTR_OCMB_BOOT_FLAGS_**           |      **_Output_**      |          **_Output_**          |
|:--------------:|:--------------------------------:|:-----------------------------------------------------:|:------------------------------------------------------------:|:----------------------:|:------------------------------:|
|                |      **Production<br>Mode**      | **Disable  Scom <br>Filtering <br>(Scratch11-bit11)** | **Disable Invalid  <br>Address Check <br>(scratch11-bit12)** | **Scom <br>Filtering** | **Invalid Address  <br>Check** |
|     **_1_**    |                 1                |                           x                           |                               x                              |        _Enabled_       |            _Enabled_           |
|     **_2_**    |                 0                |                           1                           |                               1                              |       _Disabled_       |           _Disabled_           |
|     **_3_**    |                 0                |                           0                           |                               0                              |        _Enabled_       |            _Enabled_           |


### Input-output correlations for ATTR_SECURITY_LEVEL

| **_Sl<br>No_** | **_Input via<br>OTPROM <br>Fuse_** | **_Input via <br>ATTR_OCMB_BOOT_FLAGS<br>_** |     **_Input via<br>ATTR_OCMB_BOOT_FLAGS_**    | **_Input via<br>Secure <br>Header_** |      **_Output _**      |
|:--------------:|:----------------------------------:|:--------------------------------------------:|:----------------------------------------------:|:------------------------------------:|:-----------------------:|
|                |               **SAB**              |    **Enable<br>Security<br>scratch11-bit4**   | **Enable<br>Production Mode<br>scratch11-bit28** |      **Production<br>Mode<br>**      | **ATTR_SECURITY_LEVEL** |
|     **_1_**    |                  1                 |                       x                      |                        x                       |                   1                  |        _Enforcing_        |
|     **_2_**    |                  1                 |                       x                      |                        0                       |                   0                  |        _Permisive_        |
|     **_3_**    |                  1                 |                       x                      |                        1                       |                   x                  |        _Enforcing_        |
|     **_4_**    |                  0                 |                       1                      |                        1                       |                   x                  |        _Enforcing_        |
|     **_5_**    |                  0                 |                       1                      |                        0                       |                   0                  |        _Permisive_        |
|     **_6_**    |                  0                 |                       1                      |                        x                       |                   1                  |        _Enforcing_        |
|     **_7_**    |                  0                 |                       0                      |                        x                       |                   x                  |         _Disabled_        |