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
#include "rcl/type_description_conversions.h"

#include "rcl/error_handling.h"
#include "rcutils/logging_macros.h"
#include "rcutils/types/hash_map.h"

#include "./context_impl.h"
#include "./node_impl.h"

typedef struct rcl_type_info_with_registration_count_t {
  /// Counter to keep track of registrations
  size_t numRegistrations;

  /// The actual type info.
  rcl_type_info_t type_info;
} rcl_type_info_with_registration_count_t;

rcl_ret_t rcl_node_type_cache_init(const rcl_node_t *node) {
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);

  rcutils_ret_t ret = rcutils_hash_map_init(
      &node->impl->registered_types_by_type_hash, 2, sizeof(const char *),
      sizeof(rcl_type_info_with_registration_count_t),
      rcutils_hash_map_string_hash_func, rcutils_hash_map_string_cmp_func,
      &node->context->impl->allocator);

  if (RCUTILS_RET_OK != ret) {
    RCL_SET_ERROR_MSG("Failed to initialize type cache hash map");
    return RCL_RET_ERROR;
  }

  return RCL_RET_OK;
}

rcl_ret_t rcl_node_type_cache_fini(const rcl_node_t *node) {
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);

  rcutils_ret_t rcutils_ret =
      rcutils_hash_map_fini(&node->impl->registered_types_by_type_hash);

  return RCUTILS_RET_OK == rcutils_ret ? RCL_RET_OK : RCL_RET_ERROR;
}

rcl_ret_t rcl_node_type_cache_get_type_info(const rcl_node_t *node,
                                            const char *type_hash,
                                            rcl_type_info_t *type_info) {
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(type_hash, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(type_info, RCL_RET_INVALID_ARGUMENT);

  rcl_type_info_with_registration_count_t type_info_with_registrations;

  rcutils_ret_t ret =
      rcutils_hash_map_get(&node->impl->registered_types_by_type_hash,
                           &type_hash, &type_info_with_registrations);
  if (RCUTILS_RET_OK == ret) {
    *type_info = type_info_with_registrations.type_info;
  }

  return RCUTILS_RET_OK == ret ? RCL_RET_OK : RCL_RET_ERROR;
}

rcl_ret_t rcl_node_type_cache_register_type(
    const rcl_node_t *node, const rosidl_type_hash_t *type_hash,
    const rosidl_runtime_c__type_description__TypeDescription
        *type_description) {
  char *type_hash_str = NULL;
  rcl_type_info_with_registration_count_t type_info;

  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(type_hash, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(type_description, RCL_RET_INVALID_ARGUMENT);

  // Convert hash to string
  if (RCUTILS_RET_OK !=
      rosidl_stringify_type_hash(type_hash, rcutils_get_default_allocator(),
                                 &type_hash_str)) {
    RCL_SET_ERROR_MSG("Failed to stringify type hash");
    return RCL_RET_ERROR;
  }

  // TODO(achim-k): Remove log statement
  const char *typeName = type_description->type_description.type_name.data;
  RCUTILS_LOG_INFO_NAMED(ROS_PACKAGE_NAME, "R: %s (%s)", type_hash_str,
                         typeName);

  // If the type already exists, we only have to increment the registration
  // count.
  if (rcutils_hash_map_key_exists(&node->impl->registered_types_by_type_hash,
                                  &type_hash_str)) {
    if (RCUTILS_RET_OK !=
        rcutils_hash_map_get(&node->impl->registered_types_by_type_hash,
                             &type_hash_str, &type_info)) {
      RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
          "Failed to retrieve type info for type '%s'", type_hash_str);
      return RCL_RET_ERROR;
    }

    type_info.numRegistrations++;
  } else {
    // First registration of this type
    type_info.numRegistrations = 1;

    // Convert type description struct to type description message struct.
    type_info.type_info.type_description =
        rcl_convert_type_description_runtime_to_msg(type_description);
    RCL_CHECK_FOR_NULL_WITH_MSG(type_info.type_info.type_description,
                                "converting type description struct failed",
                                return RCL_RET_ERROR);
  }

  // Update the hash map entry.
  if (RCUTILS_RET_OK !=
      rcutils_hash_map_set(&node->impl->registered_types_by_type_hash,
                           &type_hash_str, &type_info)) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Failed to update type info for type '%s'", type_hash_str);
    return RCL_RET_ERROR;
  }

  return RCL_RET_OK;
}

rcl_ret_t rcl_node_type_cache_unregister_type(
    const rcl_node_t *node, const rosidl_type_hash_t *type_hash) {
  char *type_hash_str = NULL;
  rcl_type_info_with_registration_count_t type_info;

  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(type_hash, RCL_RET_INVALID_ARGUMENT);

  // Convert hash to string
  if (RCUTILS_RET_OK !=
      rosidl_stringify_type_hash(type_hash, rcutils_get_default_allocator(),
                                 &type_hash_str)) {
    RCL_SET_ERROR_MSG("Failed to stringify type hash");
    return RCL_RET_ERROR;
  }

  // TODO(achim-k): Remove log statement
  RCUTILS_LOG_INFO_NAMED(ROS_PACKAGE_NAME, "U: %s", type_hash_str);

  if (RCUTILS_RET_OK !=
      rcutils_hash_map_get(&node->impl->registered_types_by_type_hash,
                           &type_hash_str, &type_info)) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING("Failed to deregister type '%s'",
                                         type_hash_str);
    return RCL_RET_ERROR;
  }

  if (--type_info.numRegistrations > 0) {
    if (RCUTILS_RET_OK !=
        rcutils_hash_map_set(&node->impl->registered_types_by_type_hash,
                             &type_hash_str, &type_info)) {
      RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
          "Failed to update type info for type '%s'", type_hash_str);
      return RCL_RET_ERROR;
    }
  } else {
    if (RCUTILS_RET_OK !=
        rcutils_hash_map_unset(&node->impl->registered_types_by_type_hash,
                               &type_hash_str)) {
      RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
          "Failed to unregister type info for type '%s'", type_hash_str);
      return RCL_RET_ERROR;
    }
  }

  return RCL_RET_OK;
}

#ifdef __cplusplus
}
#endif
