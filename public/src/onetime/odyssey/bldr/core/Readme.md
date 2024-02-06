
### Input-output correlations for ATTR_SECURITY_LEVEL

| **_Sl<br>No_** | **_Input via<br>OTPROM <br>Fuse_** | **_Input via <br>ATTR_OCMB_BOOT_FLAGS<br>_** |     **_Input via<br>ATTR_OCMB_BOOT_FLAGS_**    | **_Input via<br>Secure <br>Header_** |      **_Output_**      |
|:--------------:|:----------------------------------:|:--------------------------------------------:|:----------------------------------------------:|:------------------------------------:|:-----------------------:|
|                |               **SAB**              |    **Emulate<br>SAB Set<br>scratch11-bit4**  | **Enable<br>Production Mode<br>scratch11-bit28** |      **Production<br>Mode<br>**      | **ATTR_SECURITY_LEVEL** |
|     **_1_**    |                  1                 |                       x                      |                        x                       |                   1                  |        _Enforcing_        |
|     **_2_**    |                  1                 |                       x                      |                        0                       |                   0                  |        _Permisive_        |
|     **_3_**    |                  1                 |                       x                      |                        1                       |                   x                  |        _Enforcing_        |
|     **_4_**    |                  0                 |                       1                      |                        1                       |                   x                  |        _Enforcing_        |
|     **_5_**    |                  0                 |                       1                      |                        0                       |                   0                  |        _Permisive_        |
|     **_6_**    |                  0                 |                       1                      |                        x                       |                   1                  |        _Enforcing_        |
|     **_7_**    |                  0                 |                       0                      |                        x                       |                   x                  |         _Disabled_        |

#### Note

 - For ATTR_OCMB_BOOT_FLAGS definition refer public/src/import/public/hwp/odyssey/xml/attribute_info/ody_perv_attributes.xml

### Input-output correlations for scom filtering and invalid address check feature

#### Below decision is made in SPPE Boot Loader code and values are latched into LFR reg

| **_Sl<br>No_** | **_Input via<br>Secure Header_** |                          **_Input via<br>ATTR_OCMB_BOOT_FLAGS_<br>(Scratch 11)**                         |                                            **_Output via<br>LFR_**                                           |
|:--------------:|:--------------------------------:|:--------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------:|
|                |   **_Is production <br>Mode_**   | **_Disable Scom Filtering - bit11<br>Disable Invalid Addr Check - bit12<br>Allow Attr Override - bit7_** | **_Disable Scom Filtering - bit36<br>Disable Invalid Address Check - bit37<br>Allow Attr Override - bit39_** |
|      **1**     |                 1                |                                                     x                                                    |                                                       0                                                      |
|      **2**     |                 0                |                                                     0                                                    |                                                       0                                                      |
|      **3**     |                 0                |                                                     1                                                    |                                                       1                                                      |

#### Below decision is made in SPPE Runtime code. Only Values from LFR set by Boot Loader code are referenced here for decision making.

| **_Sl No_** | **_Input via <br>ATTR_SECURITY_LEVEL_** |                                            **_Input via LFR_**                                            |     **_Output_**     |         **_Output_**         |         **_Output_**         |
|:-----------:|:---------------------------------------:|:---------------------------------------------------------------------------------------------------------:|:--------------------:|:----------------------------:|:----------------------------:|
|             |                                         | **_Disable Scom Filtering - bit36<br>Disable Invalid Addr Check - bit37<br>Allow Attr Override - bit39_** | **_Scom Filtering_** | **_Invalid Addr <br>Check_** | **_Allow Attr<br>Override_** |
|   **_1_**   |               _Permisive_               |                                                     1                                                     |       *Disabled      |           Disabled           |           *Disabled          |
|   **_2_**   |               _Permisive_               |                                                     0                                                     |        Enabled       |            Enabled           |            Enabled           |
|   **_3_**   |               _Enforcing_               |                                                     x                                                     |        Enabled       |            Enabled           |            Enabled           |
|   **_4_**   |                _Disabled_               |                                                     x                                                     |       Disabled       |           Disabled           |           Disabled           |

#### Note

 - *If user has requested to disable scom filtering via **_Disable Scom Filtering (Scratch11-bit11)_**, SPPE would still perform a deny list scom requested by the user but would return RC.<br>
Primary RC      : *SBE_PRI_UNSECURE_ACCESS_DENIED*<br>
Secondary RC    : *SBE_SEC_DENYLIST_REG_ACCESS*<br>
This would allow requested user (HB/FSP/BMC/Cronus) to log a info error log to let user know the requested scom is blocked and has to be added into the allow deny list if needed.

 - If user has not requested to disable scom filtering via **_Disable Scom Filtering (Scratch11-bit11)_**, SPPE would completely block the scom requested by the user and would return RC.<br>
Primary RC      : *SBE_PRI_UNSECURE_ACCESS_DENIED*<br>
Secondary RC    : *SBE_SEC_DENYLIST_REG_ACCESS_BLOCKED*<br>

 - **_Disable Scom Filtering (Scratch11-bit11)_** will be used for disabling other soft security checks (istep enforcing, chipop fencing etc) including scom filtering  if security level of attr **_ATTR_SECURITY_LEVEL_** is set to _Permisive_.<br>