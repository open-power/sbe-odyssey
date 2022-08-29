/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/security/securityalgo.C $           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2019,2024                        */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

#include "securityalgo.H"
#include <stddef.h>

// Helper function to figure out the number of shifts required for the mask
constexpr uint32_t get_shift_len(uint32_t mask, uint8_t shifts = 0)
{
    return ((mask>>shifts) & 0x01) ? (shifts) : (get_shift_len(mask, ++shifts));
}

template <typename Func>
map_t binary_search(
                    const uint32_t search_key,
                    range_t x_range,
                    Func get_element)
{
    map_t ret = {false, 0}; // found=false

    while((x_range.start <= x_range.end) &&
          (ret.key == false))
    {
        int32_t midpoint = (x_range.start + x_range.end) / 2;
        uint32_t ele = get_element(midpoint);
        if(search_key == ele)
        {
            ret.key = true;
            ret.value = midpoint;
        }
        else if(search_key < ele)
        {
            x_range.end = midpoint - 1;
        }
        else
        {
            x_range.start = midpoint + 1;
        }
    }
    return ret;
}

//----------------------------------------------------------------------------
//  @brief Public function used for init all allow, deny and partial list table
//  data, Allowlist/ Denylist Tables [T1, T2 and T3] and Partial list [T1]
//----------------------------------------------------------------------------
bool _is_present( const _t1_table_t &table1,
                           const _t2_table_t &table2,
                           const _t3_table_t &table3,
                           const uint32_t i_addr)
{
    for(size_t i = 0; i < table1.size; i++)
    {
        uint32_t search_key = (i_addr & table1.mask) >> get_shift_len(table1.mask);
        if((table1.table[i].key_start <= search_key) &&
           (table1.table[i].key_end >= search_key))
        {
            search_key = (i_addr & table2.mask) >> get_shift_len(table2.mask);
            range_t search_range = {};
            search_range.start = i ? table1.table[i-1].value : 0;
            search_range.end = table1.table[i].value - 1;
            map_t search_result =
                binary_search(
                        search_key,
                        search_range,
                        [&table2](int32_t midpoint) -> uint32_t {
                            return table2.table[midpoint].key;
                        });
            if(search_result.key == true)
            {
                // Found the key
                search_range.start = (search_result.value ?
                                table2.table[search_result.value-1].value : 0);
                search_range.end =
                                table2.table[search_result.value].value - 1;
                search_key = (i_addr & table3.mask) >>
                                               get_shift_len(table3.mask);
                search_result = binary_search(
                                            search_key,
                                            search_range,
                                            [&table3](int32_t midpoint) -> uint32_t {
                                                return table3.table[midpoint].value;
                                            });
                if(search_result.key == true)
                {
                    // Found the number
                    return true;
                }
            }
        }
    }
    return false;
}

//----------------------------------------------------------------------------
//  @brief Look up tables to find if the given address with mask is present
//  on Partial allow list Table [T1]
//----------------------------------------------------------------------------
bool _is_present(const _pl_t1_table_t &table1,
                 const uint32_t i_addr,
                 const uint64_t i_mask)
{
    bool ret = false;
    for(size_t i = 0; i < table1.size; i++)
    {
        // Not using mask in table for search
        if((table1.table[i].key ==  i_addr) &&
           (( i_mask & (~table1.table[i].value)) == 0 ))
        {
            ret = true;
            break;
        }
    }
    return ret;
}