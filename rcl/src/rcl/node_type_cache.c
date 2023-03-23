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

#ifdef __cplusplus
extern "C" {
#endif

#include "rcl/node_type_cache.h"

#include "rcl/error_handling.h"
#include "rcutils/logging_macros.h"
#include "rcutils/types/hash_map.h"

#include "./context_impl.h"
#include "./node_impl.h"

rcl_ret_t rcl_node_type_cache_init(const rcl_node_t *node) {
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);

  if (NULL != node->impl->registered_types_by_type_hash) {
    RCL_SET_ERROR_MSG("Type cache already initialized");
    return RCL_RET_ALREADY_INIT;
  }

  node->impl->registered_types_by_type_hash =
      node->context->impl->allocator.allocate(
          sizeof(rcutils_hash_map_t), node->context->impl->allocator.state);
  RCL_CHECK_FOR_NULL_WITH_MSG(node->impl->registered_types_by_type_hash,
                              "allocating memory failed",
                              return RCL_RET_BAD_ALLOC);

  *node->impl->registered_types_by_type_hash =
      rcutils_get_zero_initialized_hash_map();

  rcl_ret_t ret = rcutils_hash_map_init(
      node->impl->registered_types_by_type_hash, 2, sizeof(const char *),
      sizeof(rcl_node_type_cache_type_info_t),
      rcutils_hash_map_string_hash_func, rcutils_hash_map_string_cmp_func,
      &node->context->impl->allocator);

  if (RCUTILS_RET_OK != ret) {
    RCL_SET_ERROR_MSG("Failed to initialize type cache hash map");
    node->context->impl->allocator.deallocate(
        node->impl->registered_types_by_type_hash,
        node->context->impl->allocator.state);
    node->impl->registered_types_by_type_hash = NULL;
    return RCL_RET_ERROR;
  }

  return RCL_RET_OK;
}
rcl_ret_t rcl_node_type_cache_fini(const rcl_node_t *node) {
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(node->impl->registered_types_by_type_hash,
                              "type cache not initialized",
                              return RCL_RET_NOT_INIT);

  rcutils_ret_t rcutils_ret =
      rcutils_hash_map_fini(node->impl->registered_types_by_type_hash);
  node->context->impl->allocator.deallocate(
      node->impl->registered_types_by_type_hash,
      node->context->impl->allocator.state);

  return RCUTILS_RET_OK == rcutils_ret ? RCL_RET_OK : RCL_RET_ERROR;
}

rcl_ret_t rcl_node_type_cache_register_msg_type(
    const rcl_node_t *node, const rosidl_message_type_support_t *type_support) {
  rcl_node_type_cache_type_info_t type_info;
  char *type_hash = NULL;

  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(type_support, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(node->impl->registered_types_by_type_hash,
                              "type cache not initialized",
                              return RCL_RET_NOT_INIT);
  if (RCUTILS_RET_OK !=
      rosidl_stringify_type_hash(type_support->type_hash,
                                 rcutils_get_default_allocator(), &type_hash)) {
    RCL_SET_ERROR_MSG("Failed to stringify type hash");
    return RCL_RET_ERROR;
  }

  // TODO(achim-k): Remove log statement
  RCUTILS_LOG_INFO_NAMED(ROS_PACKAGE_NAME, "Registering type with hash %s",
                         type_hash);

  if (rcutils_hash_map_key_exists(node->impl->registered_types_by_type_hash,
                                  &type_hash)) {
    if (RCUTILS_RET_OK !=
        rcutils_hash_map_get(node->impl->registered_types_by_type_hash,
                             &type_hash, &type_info)) {
      RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
          "Failed to retrieve type info for type '%s'", type_hash);
      return RCL_RET_ERROR;
    }
    type_info.numRegistrations++;
  } else {
    type_info.numRegistrations = 1;
    // TODO(achim-k): Populate struct from type_support here.
    // type_info.type_description = ...;
  }

  if (RCUTILS_RET_OK !=
      rcutils_hash_map_set(node->impl->registered_types_by_type_hash,
                           &type_hash, &type_info)) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Failed to update type info for type '%s'", type_hash);
    return RCL_RET_ERROR;
  }

  return RCL_RET_OK;
}

rcl_ret_t rcl_node_type_cache_unregister_msg_type(
    const rcl_node_t *node, const rosidl_message_type_support_t *type_support) {
  rcl_node_type_cache_type_info_t type_info;
  char *type_hash = NULL;
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(type_support, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(node->impl->registered_types_by_type_hash,
                              "type cache not initialized",
                              return RCL_RET_NOT_INIT);

  if (RCUTILS_RET_OK !=
      rosidl_stringify_type_hash(type_support->type_hash,
                                 rcutils_get_default_allocator(), &type_hash)) {
    RCL_SET_ERROR_MSG("Failed to stringify type hash");
    return RCL_RET_ERROR;
  }

  if (RCUTILS_RET_OK !=
      rcutils_hash_map_get(node->impl->registered_types_by_type_hash,
                           &type_hash, &type_info)) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING("Failed to deregister type '%s'",
                                         type_hash);
    return RCL_RET_ERROR;
  }

  if (--type_info.numRegistrations > 0) {
    if (RCUTILS_RET_OK !=
        rcutils_hash_map_set(node->impl->registered_types_by_type_hash,
                             &type_hash, &type_info)) {
      RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
          "Failed to update type info for type '%s'", type_hash);
      return RCL_RET_ERROR;
    }
  } else {
    if (RCUTILS_RET_OK !=
        rcutils_hash_map_unset(node->impl->registered_types_by_type_hash,
                               &type_hash)) {
      RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
          "Failed to unregister type info for type '%s'", type_hash);
      return RCL_RET_ERROR;
    }
  }

  return RCL_RET_OK;
}

rcl_ret_t rcl_node_type_cache_get_type_info(
    const rcl_node_t *node, const char *type_hash,
    rcl_node_type_cache_type_info_t *type_info) {
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(type_hash, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(type_info, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(node->impl->registered_types_by_type_hash,
                              "type cache not initialized",
                              return RCL_RET_NOT_INIT);

  rcutils_ret_t ret = rcutils_hash_map_get(
      node->impl->registered_types_by_type_hash, &type_hash, type_info);
  return RCUTILS_RET_OK == ret ? RCL_RET_OK : RCL_RET_ERROR;
}

#ifdef __cplusplus
}
#endif
