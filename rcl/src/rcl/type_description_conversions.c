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

#include "./type_description_conversions.h"

#include "rosidl_runtime_c/string_functions.h"
#include "rosidl_runtime_c/type_description/field__functions.h"
#include "rosidl_runtime_c/type_description/individual_type_description__functions.h"
#include "rosidl_runtime_c/type_description/type_description__functions.h"
#include "type_description_interfaces/msg/detail/field__functions.h"
#include "type_description_interfaces/msg/individual_type_description.h"

#ifdef __cplusplus
extern "C" {
#endif

static bool individual_type_description_runtime_to_msg(
    const rosidl_runtime_c__type_description__IndividualTypeDescription *in,
    type_description_interfaces__msg__IndividualTypeDescription *out) {
  const size_t nFields = in->fields.size;
  const bool success =
      type_description_interfaces__msg__IndividualTypeDescription__init(out) &&
      rosidl_runtime_c__String__copy(&in->type_name, &out->type_name) &&
      type_description_interfaces__msg__Field__Sequence__init(&out->fields,
                                                              nFields);
  if (!success) {
    goto error;
  }

  for (size_t i = 0; i < in->fields.size; ++i) {
    if (!rosidl_runtime_c__String__copy(&(in->fields.data[i].name),
                                        &(out->fields.data[i].name))) {
      goto error;
    }

    if (in->fields.data[i].default_value.size) {
      if (!rosidl_runtime_c__String__copy(
              &(in->fields.data[i].default_value),
              &(out->fields.data[i].default_value))) {
        goto error;
      }
    }

    // type_id
    out->fields.data[i].type.type_id = in->fields.data[i].type.type_id;
    // capacity
    out->fields.data[i].type.capacity = in->fields.data[i].type.capacity;
    // string_capacity
    out->fields.data[i].type.string_capacity =
        in->fields.data[i].type.string_capacity;

    // nested_type_name
    if (in->fields.data[i].type.nested_type_name.size) {
      if (!rosidl_runtime_c__String__copy(
              &(in->fields.data[i].type.nested_type_name),
              &(out->fields.data[i].type.nested_type_name))) {
        goto error;
      }
    }
  }

  return true;

error:
  type_description_interfaces__msg__IndividualTypeDescription__fini(out);
  return false;
}

static bool individual_type_description_msg_to_runtime(
    const type_description_interfaces__msg__IndividualTypeDescription *in,
    rosidl_runtime_c__type_description__IndividualTypeDescription *out) {
  const size_t nFields = in->fields.size;
  const bool success =
      rosidl_runtime_c__type_description__IndividualTypeDescription__init(
          out) &&
      rosidl_runtime_c__String__copy(&in->type_name, &out->type_name) &&
      rosidl_runtime_c__type_description__Field__Sequence__init(&out->fields,
                                                                nFields);
  if (!success) {
    goto error;
  }

  for (size_t i = 0; i < in->fields.size; ++i) {
    if (!rosidl_runtime_c__String__copy(&(in->fields.data[i].name),
                                        &(out->fields.data[i].name))) {
      goto error;
    }

    if (in->fields.data[i].default_value.size) {
      if (!rosidl_runtime_c__String__copy(
              &(in->fields.data[i].default_value),
              &(out->fields.data[i].default_value))) {
        goto error;
      }
    }

    // type_id
    out->fields.data[i].type.type_id = in->fields.data[i].type.type_id;
    // capacity
    out->fields.data[i].type.capacity = in->fields.data[i].type.capacity;
    // string_capacity
    out->fields.data[i].type.string_capacity =
        in->fields.data[i].type.string_capacity;

    // nested_type_name
    if (in->fields.data[i].type.nested_type_name.size) {
      if (!rosidl_runtime_c__String__copy(
              &(in->fields.data[i].type.nested_type_name),
              &(out->fields.data[i].type.nested_type_name))) {
        goto error;
      }
    }
  }

  return true;

error:
  rosidl_runtime_c__type_description__IndividualTypeDescription__init(out);
  return false;
}

type_description_interfaces__msg__TypeDescription *
rcl_convert_type_description_runtime_to_msg(
    const rosidl_runtime_c__type_description__TypeDescription *in) {
  // Create the object
  type_description_interfaces__msg__TypeDescription *out =
      type_description_interfaces__msg__TypeDescription__create();
  if (NULL == out) {
    return NULL;
  }

  // init type_description
  if (!type_description_interfaces__msg__IndividualTypeDescription__init(
          &out->type_description)) {
    type_description_interfaces__msg__TypeDescription__fini(out);
    return NULL;
  }

  // init referenced_type_descriptions with the correct size
  if (!type_description_interfaces__msg__IndividualTypeDescription__Sequence__init(
          &out->referenced_type_descriptions,
          in->referenced_type_descriptions.size)) {
    type_description_interfaces__msg__TypeDescription__fini(out);
    return NULL;
  }

  bool success = individual_type_description_runtime_to_msg(
      &in->type_description, &out->type_description);

  for (size_t i = 0; success && i < in->referenced_type_descriptions.size;
       ++i) {
    success &= individual_type_description_runtime_to_msg(
        &in->referenced_type_descriptions.data[i],
        &out->referenced_type_descriptions.data[i]);
  }

  if (!success) {
    type_description_interfaces__msg__TypeDescription__fini(out);
    return NULL;
  }

  return out;
}

rosidl_runtime_c__type_description__TypeDescription *
rcl_convert_type_description_msg_to_runtime(
    const type_description_interfaces__msg__TypeDescription *in) {
  // Create the object
  rosidl_runtime_c__type_description__TypeDescription *out =
      rosidl_runtime_c__type_description__TypeDescription__create();
  if (NULL == out) {
    return NULL;
  }

  // init type_description
  if (!rosidl_runtime_c__type_description__IndividualTypeDescription__init(
          &out->type_description)) {
    rosidl_runtime_c__type_description__IndividualTypeDescription__fini(&out->type_description);
    return NULL;
  }

  // init referenced_type_descriptions with the correct size
  if (!rosidl_runtime_c__type_description__IndividualTypeDescription__Sequence__init(
          &out->referenced_type_descriptions,
          in->referenced_type_descriptions.size)) {
    rosidl_runtime_c__type_description__TypeDescription__fini(out);
    return NULL;
  }

  bool success = individual_type_description_msg_to_runtime(
      &in->type_description, &out->type_description);

  for (size_t i = 0; success && i < in->referenced_type_descriptions.size;
       ++i) {
    success &= individual_type_description_msg_to_runtime(
        &in->referenced_type_descriptions.data[i],
        &out->referenced_type_descriptions.data[i]);
  }

  if (!success) {
    rosidl_runtime_c__type_description__TypeDescription__fini(out);
    return NULL;
  }

  return out;
}

#ifdef __cplusplus
}
#endif
