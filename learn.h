/*
 * Copyright (c) 2015 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __included_learn_h__
#define __included_learn_h__

#include <vnet/vnet.h>
#include <vnet/ip/ip.h>
#include <vnet/ethernet/ethernet.h>

#include <vppinfra/hash.h>
#include <vppinfra/error.h>
#include <vppinfra/elog.h>

#define learn_elog_addr(_str, _addr)                 \
do                                                   \
  {                                                  \
    ELOG_TYPE_DECLARE (e) =                          \
      {                                              \
        .format = _str " %d.%d.%d.%d",               \
        .format_args = "i1i1i1i1",                   \
      };                                             \
    CLIB_PACKED(struct                               \
      {                                              \
        u8 oct1;                                     \
        u8 oct2;                                     \
        u8 oct3;                                     \
        u8 oct4;                                     \
      }) *ed;                                        \
    ed = ELOG_DATA (&vlib_global_main.elog_main, e); \
    ed->oct1 = nat_elog_addr >> 24 & 0xff;           \
    ed->oct2 = nat_elog_addr >> 16 & 0xff;           \
    ed->oct3 = nat_elog_addr >> 8 & 0xff;            \
    ed->oct4 = nat_elog_addr & 0xff;                 \
  } while (0);

typedef struct {
    /* API message ID base */
    u16 msg_id_base;

    /* convenience */
    vnet_main_t * vnet_main;
} learn_main_t;

extern learn_main_t learn_main;

extern vlib_node_registration_t learn_node;

#define LEARN_PLUGIN_BUILD_VER "1.0"

#endif /* __included_learn_h__ */
