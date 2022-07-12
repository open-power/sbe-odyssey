/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/metadata_base.H $                     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
/* [+] International Business Machines Corp.                              */
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
#pragma once

/**
 * @brief Image metadata system base structures
 *
 * This defines the project independent parts of the metadata system:
 * The machinery to declare metadata types and values, and to retrieve
 * these values.
 *
 * The system uses a tag-length-value (TLV) scheme for the metadata:
 * All metadata consists of a chain of fields which are preceded by
 * a header that contains a three-letter tag which identifies the
 * field, and the length of the field. This way, given the beginning
 * of metadata, it is simple to locate a given metadata field without
 * having to know all possible field types, making the metadata simple
 * to extend and update without having to adapt all tooling.
 */

#include <stdint.h>

#define PACKED __attribute__((packed))

/**
 * @brief Header of a TLV field
 */
struct PACKED ImageMetadataHeader {
    uint32_t tag :24;  ///< Tag that identifies the field
    uint32_t len :8;   ///< Length of field (not including header) in 32bit words
};

/**
 * @brief Helper function to locate a metadata entry
 * @param start Beginning of metadata structure
 * @param tag   Tag to search for
 * @return Pointer to metdatada value, or NULL if the tag was not found
 */
const void *_get_metadata(const void *start, uint32_t tag);

/**
 * @brief Helper class to detect duplicate use of tag values
 *
 * Each DECLARE_METADATA(_STR) declares a specialization of this template,
 * and the compiler will yell if any two metadata declarations declare
 * the same specialization because they use the same int_tag.
 */
template<uint32_t int_tag> struct __META_INT_TAG_USAGE;

/**
 * @brief Declare a struct-based metadata field
 *
 * This declares a struct-based metadata field with fixed size, including a
 * tailored GET_META_<tag> function that takes a pointer to the start of
 * metadata and returns a pointer to the field, or NULL if the field was not
 * found.
 *
 * This macro must be followed by the struct definition including curly braces.
 *
 * @param tag     The three-character tag to use for the field
 * @param tag_int The integer representation of the tag: echo -n TAG | xxd -p
 */
#define DECLARE_METADATA(tag, tag_int)                                    \
    static_assert(tag_int <= 0xFFFFFF, #tag " tag exceeds 24-bit range");  \
    template<> struct __META_INT_TAG_USAGE<tag_int> {};                   \
    static const uint32_t META_##tag = tag_int;                           \
    struct META_##tag##_t;                                                \
    static inline const META_##tag##_t *GET_META_##tag(const void *start) \
    {                                                                     \
        return (META_##tag##_t *)_get_metadata(start, META_##tag);        \
    }                                                                     \
    struct PACKED META_##tag##_t

/**
 * @brief Define the value for a struct-based metadata field
 * @param tag    The three-character tag
 * @param value  A complete struct initializer, including curly braces
 */
#define METADATA(tag, value...)                            \
    static_assert(sizeof(META_##tag##_t) < 0x400, #tag " structure size exceeds 1KB"); \
    struct PACKED {                                        \
        ImageMetadataHeader _header;                       \
        META_##tag##_t _value;                             \
        char padding[-sizeof(META_##tag##_t) & 3];         \
    } _##tag = {                                           \
        { META_##tag, (sizeof(META_##tag##_t) + 3) >> 2 }, \
        value                                              \
    }

/**
 * @brief Declare a string metadata field
 *
 * This is similar to DECLARE_METADATA, but the type is a string of
 * arbitrary length (maximum 1KB).
 */
#define DECLARE_METADATA_STR(tag, tag_int)                          \
    static_assert(tag_int <= 0xFFFFFF, #tag " tag exceeds 24-bit range"); \
    template<> struct __META_INT_TAG_USAGE<tag_int> {};             \
    static const uint32_t META_##tag = tag_int;                     \
    static inline const char *GET_META_##tag(const void *start)     \
    {                                                               \
        return (char *)_get_metadata(start, META_##tag);            \
    }

/**
 * @brief Define the value for a string metadata field
 * @param tag   The three-character tag
 * @param value The string as a string literal
 */
#define METADATA_STR(tag, value)                           \
    static_assert(sizeof(value) < 0x400, #tag " string literal exceeds 1KB"); \
    struct PACKED {                                        \
        ImageMetadataHeader _header;                       \
        const char _value[(sizeof(value) + 3) & ~3];       \
    } _##tag = {                                           \
        { META_##tag, (sizeof(value) + 3) >> 2 },          \
        value                                              \
    }
