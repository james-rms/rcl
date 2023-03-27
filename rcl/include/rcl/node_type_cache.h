// Copyright 2023 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCL__NODE_TYPE_CACHE_H_
#define RCL__NODE_TYPE_CACHE_H_

#include "rcl/node.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_runtime_c/action_type_support_struct.h"
#include "type_description_interfaces/msg/type_description.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rcl_type_info_t {
  size_t numRegistrations;

  // TODO(achim-k): Add remaining fields
  const type_description_interfaces__msg__TypeDescription* type_description;
} rcl_type_info_t;

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_init(const rcl_node_t* node);

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_fini(const rcl_node_t* node);

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_register_msg_type(
    const rcl_node_t* node, const rosidl_message_type_support_t* type_support);

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_unregister_msg_type(
    const rcl_node_t* node, const rosidl_message_type_support_t* type_support);

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_register_srv_type(
    const rcl_node_t* node, const rosidl_service_type_support_t* type_support);

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_unregister_srv_type(
    const rcl_node_t* node, const rosidl_service_type_support_t* type_support);

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_register_action_type(
    const rcl_node_t* node, const rosidl_action_type_support_t* type_support);

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_unregister_action_type(
    const rcl_node_t* node, const rosidl_action_type_support_t* type_support);

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t rcl_node_type_cache_get_type_info(const rcl_node_t* node,
                                            const char* type_hash,
                                            rcl_type_info_t* type_info);

#ifdef __cplusplus
}
#endif

#endif  // RCL__NODE_TYPE_CACHE_H_
