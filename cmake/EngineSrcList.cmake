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

# This file contains a listing of all of the source files used in the engine
# build target. Populates a list called PGLOCAL_SOURCES_LIST

# Preferred method of adding source items is through the Python script in:
# `util/GenerateEngineSrcList.py`

# This function appends the provided string list to PGLOCAL_SOURCES_LIST
set(PGLOCAL_SOURCES_LIST "")
foreach(fname 

"addons/Addons.cpp"
"math/MathUtil.cpp"
"math/Quate.cpp"
"math/Spharm.cpp"
"math/Vec2.cpp"
"math/Vec3.cpp"
"misc/Engine.cpp"
"misc/Events.cpp"
"misc/GameLayer.cpp"
"misc/GameLayerMachine.cpp"
"misc/Input.cpp"
"misc/Logger.cpp"
"misc/MiscResource.cpp"
"misc/MissionGameLayer.cpp"
"misc/ReferenceCounted.cpp"
"misc/StreamStuff.cpp"
"misc/StringResource.cpp"
"resources/Resource.cpp"
"resources/Resources.cpp"
"resources/ResourcesUtil.cpp"
"scripts/ScriptResource.cpp"
"scripts/Scripts.cpp"
"sound/SineWaveform.cpp"
"sound/SoundContext.cpp"
"sound/SoundEndpoint.cpp"
"sound/SoundMedium.cpp"
"sound/SoundReceiver.cpp"
"sound/SoundResource.cpp"
"sound/SoundSource.cpp"
"sound/SoundWaveform.cpp"
"video/Camera.cpp"
"video/FontResource.cpp"
"video/Geometry.cpp"
"video/GeometryResourceOpenGL.cpp"
"video/GeometryResourceVulkan.cpp"
"video/Image.cpp"
"video/ImageResource.cpp"
"video/Material.cpp"
"video/MaterialResource.cpp"
"video/Model.cpp"
"video/ModelInstance.cpp"
"video/ModelResource.cpp"
"video/Renderable.cpp"
"video/ShaderProgramResource.cpp"
"video/ShaderResource.cpp"
"video/ShoRendererOpenGL.cpp"
"video/ShoRendererVulkan.cpp"
"video/SimpleScenegraph.cpp"
"video/Texture.cpp"
"video/TextureResource.cpp"
"video/Video.cpp"
"video/VulkanUtils.cpp"

)
list(APPEND PGLOCAL_SOURCES_LIST "${PGLOCAL_SOURCE_DIR}/${fname}")
endforeach()

# This function appends the provided string list to PGLOCAL_INCLUDE_DIRS
foreach(dirname 

"addons"
"math"
"misc"
"resources"
"scripts"
"sound"
"video"

)
list(APPEND PGLOCAL_INCLUDE_DIRS "${PGLOCAL_SOURCE_DIR}/${dirname}")
endforeach()
