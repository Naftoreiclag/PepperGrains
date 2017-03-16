# This file contains a listing of all of the source files used in the engine
# build target. Populates a list called PGLOCAL_SOURCES_LIST
# Prefer to not use GLOB, since that causes headaches

# This function automatically appends the source directory to the given list
set(PGLOCAL_SOURCES_LIST "")
foreach(fname 
### ADD SOURCE FILES BELOW ###

"Addons.cpp"
"Camera.cpp"
"Engine.cpp"
"Events.cpp"
"FontResource.cpp"
"Gamelayer.cpp"
"GamelayerMachine.cpp"
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
"MissionGamelayer.cpp"
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

### END SOURCE FILE LIST ###
)
list(APPEND PGLOCAL_SOURCES_LIST "${PGLOCAL_SOURCE_DIR}/${fname}")
endforeach()
