#   Copyright 2017 James Fong
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

# Populates:
# - GLFW_FOUND
# - GLFW_INCLUDE_DIR
# - GLFW_LIBRARY
# - GLFW_VERSION_MAJOR
# - GLFW_VERSION_MINOR
# - GLFW_VERSION_PATCH
# - GLFW_VERSION_STRING

set(GLFW_FOUND FALSE)
find_path(GLFW_INCLUDE_DIR NAMES "GLFW/glfw3.h")
set(GLFW_VERSION_MAJOR "GLFW_VERSION_MAJOR-NOTFOUND"
    CACHE STRING "GLFW major version")
set(GLFW_VERSION_MINOR "GLFW_VERSION_MINOR-NOTFOUND"
    CACHE STRING "GLFW minor version")
set(GLFW_VERSION_PATCH "GLFW_VERSION_PATCH-NOTFOUND"
    CACHE STRING "GLFW patch version")
find_library(GLFW_LIBRARY NAMES "glfw3")

if(GLFW_INCLUDE_DIR)
    set(GLFW_FOUND TRUE)
endif()

set(GLFW_VERSION_STRING
    "${GLFW_VERSION_MAJOR}.${GLFW_VERSION_MINOR}.${GLFW_VERSION_PATCH}")

mark_as_advanced(
    GLFW_INCLUDE_DIR
    GLFW_VERSION_MAJOR
    GLFW_VERSION_MINOR
    GLFW_VERSION_PATCH
)

