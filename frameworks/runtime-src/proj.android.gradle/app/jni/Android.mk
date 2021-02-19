LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dlua_shared

LOCAL_MODULE_FILENAME := libcocos2dlua

LOCAL_SRC_FILES := \
../../../Classes/AppDelegate.cpp \
../../../Classes/RadLayer.cpp \
../../../Classes/RadMath.cpp \
../../../Classes/SpriteWithHue.cpp \
../../../Classes/nanovg/nanonode/NVGDrawNode.cpp \
../../../Classes/nanovg/nanonode/NVGNode.cpp \
../../../Classes/reader/CreatorReader.cpp \
../../../Classes/reader/animation/AnimateClip.cpp \
../../../Classes/reader/animation/AnimationClip.cpp \
../../../Classes/reader/animation/AnimationManager.cpp \
../../../Classes/reader/animation/Bezier.cpp \
../../../Classes/reader/animation/Easing.cpp \
../../../Classes/reader/collider/Collider.cpp \
../../../Classes/reader/collider/ColliderManager.cpp \
../../../Classes/reader/collider/Contract.cpp \
../../../Classes/reader/collider/Intersection.cpp \
../../../Classes/reader/dragonbones/animation/Animation.cpp \
../../../Classes/reader/dragonbones/animation/AnimationState.cpp \
../../../Classes/reader/dragonbones/animation/TimelineState.cpp \
../../../Classes/reader/dragonbones/animation/WorldClock.cpp \
../../../Classes/reader/dragonbones/armature/Armature.cpp \
../../../Classes/reader/dragonbones/armature/Bone.cpp \
../../../Classes/reader/dragonbones/armature/Slot.cpp \
../../../Classes/reader/dragonbones/cocos2dx/CCArmatureDisplay.cpp \
../../../Classes/reader/dragonbones/cocos2dx/CCFactory.cpp \
../../../Classes/reader/dragonbones/cocos2dx/CCSlot.cpp \
../../../Classes/reader/dragonbones/cocos2dx/CCTextureData.cpp \
../../../Classes/reader/dragonbones/core/BaseObject.cpp \
../../../Classes/reader/dragonbones/events/EventObject.cpp \
../../../Classes/reader/dragonbones/factories/BaseFactory.cpp \
../../../Classes/reader/dragonbones/model/AnimationData.cpp \
../../../Classes/reader/dragonbones/model/ArmatureData.cpp \
../../../Classes/reader/dragonbones/model/DragonBonesData.cpp \
../../../Classes/reader/dragonbones/model/FrameData.cpp \
../../../Classes/reader/dragonbones/model/TimelineData.cpp \
../../../Classes/reader/dragonbones/parsers/DataParser.cpp \
../../../Classes/reader/dragonbones/parsers/JSONDataParser.cpp \
../../../Classes/reader/dragonbones/textures/TextureData.cpp \
../../../Classes/reader/lua-bindings/creator_reader_bindings.cpp \
../../../Classes/reader/lua-bindings/dragonbones/lua_dragonbones_auto.cpp \
../../../Classes/reader/lua-bindings/dragonbones/lua_dragonbones_manual.cpp \
../../../Classes/reader/lua-bindings/reader/lua_creator_reader_auto.cpp \
../../../Classes/reader/lua-bindings/reader/lua_creator_reader_manual.cpp \
../../../Classes/reader/ui/PageView.cpp \
../../../Classes/reader/ui/RichtextStringVisitor.cpp \
../../../Classes/sunmap/AppDelegate.cpp \
../../../Classes/sunmap/MapScene.cpp \
../../../Classes/sunmap/CacheMaker/CacheMaker.cpp \
../../../Classes/sunmap/Core/BitmapCache.cpp \
../../../Classes/sunmap/Core/FileOperation.cpp \
../../../Classes/sunmap/Core/GeoUtils.cpp \
../../../Classes/sunmap/Core/MapControlLayer.cpp \
../../../Classes/sunmap/Core/PhysicMap.cpp \
../../../Classes/sunmap/Core/PublicMsgHandler.cpp \
../../../Classes/sunmap/Core/RawTile.cpp \
../../../Classes/sunmap/Core/Singleton.cpp \
../../../Classes/sunmap/Core/SqliteLocalStorage.cpp \
../../../Classes/sunmap/Core/Style.cpp \
../../../Classes/sunmap/Core/SunFile.cpp \
../../../Classes/sunmap/Core/TileLoader.cpp \
../../../Classes/sunmap/Core/TileResolver.cpp \
../../../Classes/sunmap/Core/TileResolver_android.cpp \
../../../Classes/sunmap/Core/tools.cpp \
../../../Classes/sunmap/Core/VisibleRect.cpp \
../../../Classes/sunmap/Layer/ControlLayer.cpp \
../../../Classes/sunmap/Layer/Feature.cpp \
../../../Classes/sunmap/Layer/LayerSwitcher.cpp \
../../../Classes/sunmap/Layer/LocalTileManager.cpp \
../../../Classes/sunmap/Layer/MarkerLayer.cpp \
../../../Classes/sunmap/Layer/VectorLayer.cpp \
../../../Classes/sunmap/MapPrivoder/AMapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/ArcGisMapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/BingMapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/BMapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/GoogleSatelliteMapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/GoogleTerrainMapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/GoogleVectorMapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/MapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/MapStrategyFactory.cpp \
../../../Classes/sunmap/MapPrivoder/OpenStreetMapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/SkyDreamMap.cpp \
../../../Classes/sunmap/MapPrivoder/SuperMapCloudMapStrategy.cpp \
../../../Classes/sunmap/MapPrivoder/TiandituMapStrategy.cpp \
../../../Classes/nanovg/nanovg/nanovg.c \
hellolua/main.cpp \


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes \
$(LOCAL_PATH)/../../../Classes/nanovg \
$(LOCAL_PATH)/../../../Classes/reader \
$(LOCAL_PATH)/../../../Classes/sunmap \
$(LOCAL_PATH)/../../../Classes/nanovg/nanonode \
$(LOCAL_PATH)/../../../Classes/nanovg/nanovg \
$(LOCAL_PATH)/../../../Classes/reader/animation \
$(LOCAL_PATH)/../../../Classes/reader/collider \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones \
$(LOCAL_PATH)/../../../Classes/reader/flatbuffers \
$(LOCAL_PATH)/../../../Classes/reader/lua-bindings \
$(LOCAL_PATH)/../../../Classes/reader/ui \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/animation \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/armature \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/cocos2dx \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/core \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/events \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/factories \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/geom \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/model \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/parsers \
$(LOCAL_PATH)/../../../Classes/reader/dragonbones/textures \
$(LOCAL_PATH)/../../../Classes/reader/lua-bindings/dragonbones \
$(LOCAL_PATH)/../../../Classes/reader/lua-bindings/reader \
$(LOCAL_PATH)/../../../Classes/sunmap/CacheMaker \
$(LOCAL_PATH)/../../../Classes/sunmap/Core \
$(LOCAL_PATH)/../../../Classes/sunmap/Layer \
$(LOCAL_PATH)/../../../Classes/sunmap/MapPrivoder \
$(LOCAL_PATH)/../../../Classes/sunmap/CacheMaker/google \
$(LOCAL_PATH)/../../../Classes/sunmap/CacheMaker/google/protobuf \
$(LOCAL_PATH)/../../../Classes/sunmap/CacheMaker/google/protobuf/compiler \
$(LOCAL_PATH)/../../../Classes/sunmap/CacheMaker/google/protobuf/io \
$(LOCAL_PATH)/../../../Classes/sunmap/CacheMaker/google/protobuf/stubs \
$(LOCAL_PATH)/../../../Classes/sunmap/CacheMaker/google/protobuf/compiler/cpp \

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_STATIC_LIBRARIES := cocos2d_lua_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, $(LOCAL_PATH)/../../../../cocos2d-x)
$(call import-module, cocos/scripting/lua-bindings/proj.android)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
