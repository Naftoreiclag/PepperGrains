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
# `tool/GenerateEngineSrcList.py`

# This function appends the provided string list to PGLOCAL_SOURCES_LIST
set(PGLOCAL_SOURCES_LIST "")
foreach(fname 

"Addons.cpp"
"Camera.cpp"
"Engine.cpp"
"Events.cpp"
"FontResource.cpp"
"GameLayer.cpp"
"GameLayerMachine.cpp"
"Geometry.cpp"
"GeometryResourceOpenGL.cpp"
"GeometryResourceVulkan.cpp"
"Image.cpp"
"ImageResource.cpp"
"Input.cpp"
"Logger.cpp"
"Material.cpp"
"MaterialResource.cpp"
"MathUtil.cpp"
"MiscResource.cpp"
"MissionGameLayer.cpp"
"Model.cpp"
"ModelInstance.cpp"
"ModelResource.cpp"
"Quate.cpp"
"ReferenceCounted.cpp"
"Renderable.cpp"
"Resource.cpp"
"Resources.cpp"
"ResourcesUtil.cpp"
"ScriptResource.cpp"
"Scripts.cpp"
"ShaderProgramResource.cpp"
"ShaderResource.cpp"
"ShoRendererOpenGL.cpp"
"ShoRendererVulkan.cpp"
"SimpleScenegraph.cpp"
"SineWaveform.cpp"
"SoundContext.cpp"
"SoundEndpoint.cpp"
"SoundMedium.cpp"
"SoundReceiver.cpp"
"SoundResource.cpp"
"SoundSource.cpp"
"SoundWaveform.cpp"
"Spharm.cpp"
"StreamStuff.cpp"
"StringResource.cpp"
"Texture.cpp"
"TextureResource.cpp"
"Vec2.cpp"
"Vec3.cpp"
"Video.cpp"
"VulkanUtils.cpp"

)
list(APPEND PGLOCAL_SOURCES_LIST "${PGLOCAL_SOURCE_DIR}/${fname}")
endforeach()
