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

#ifndef RCL__TYPE_DESCRIPTION_CONVERSIONS_H_
#define RCL__TYPE_DESCRIPTION_CONVERSIONS_H_

#include "rosidl_runtime_c/type_description/type_description__struct.h"
#include "type_description_interfaces/msg/type_description.h"

#ifdef __cplusplus
extern "C" {
#endif

type_description_interfaces__msg__TypeDescription *
rcl_convert_type_description_runtime_to_msg(
    const rosidl_runtime_c__type_description__TypeDescription *in);

#ifdef __cplusplus
}
#endif

#endif  // RCL__TYPE_DESCRIPTION_CONVERSIONS_H_
