// Copyright 2017 Google Inc. All rights reserved.
// Copyright 2018 Baidu X-Lab. Yunhan Jia <jiayunhan@baidu.com>
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

#ifndef SRC_UTF8_FIX_H_
#define SRC_UTF8_FIX_H_

#include <string>

#include "src/random.h"

namespace protobuf_mutator {

void FixUtf8String(std::string* str, RandomEngine* random);

}  // namespace protobuf_mutator

#endif  // SRC_UTF8_FIX_H_
