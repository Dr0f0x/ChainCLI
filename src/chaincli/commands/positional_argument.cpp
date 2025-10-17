// Copyright 2025 Dominik Czekai
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

#include "positional_argument.h"
#include "docwriting/docwriting.h"

#define inline_t

namespace cli::commands
{
inline_t std::string PositionalArgumentBase::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

inline_t std::string PositionalArgumentBase::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}
} // namespace cli::commands