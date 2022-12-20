#pragma once

#include <memory>
#include <vector>

#include "formats/studiomodel/EditableStudioModel.hpp"

#include "graphics/GraphicsConstants.hpp"

#include "ui/StateSnapshot.hpp"
#include "ui/assets/Assets.hpp"
#include "ui/assets/studiomodel/StudioModelAssetProvider.hpp"

#include "utility/mathlib.hpp"

class AxesEntity;
class BackgroundEntity;
class BoundingBoxEntity;
class CameraOperators;
class ClippingBoxEntity;
class CrosshairEntity;
class EntityContext;
class GroundEntity;
class GuidelinesEntity;
class HLMVStudioModelEntity;
class PlayerHitboxEntity;
class SceneCameraOperator;
class TextureCameraOperator;
class TextureEntity;

namespace graphics
{
class IGraphicsContext;
class Scene;
class TextureLoader;
}

namespace studiomdl
{
class IStudioModelRenderer;
}

namespace studiomodel
{
class StudioModelData;

enum class Pose
{
	Sequences = 0,
	Skeleton
};

class StudioModelAsset final : public Asset
{
	Q_OBJECT

public:
	StudioModelAsset(QString&& fileName,
		EditorContext* editorContext, StudioModelAssetProvider* provider,
		std::unique_ptr<studiomdl::EditableStudioModel>&& editableStudioModel);

	~StudioModelAsset();
	StudioModelAsset(const StudioModelAsset&) = delete;
	StudioModelAsset& operator=(const StudioModelAsset&) = delete;

	const StudioModelAssetProvider* GetProvider() const override { return _provider; }

	void PopulateAssetMenu(QMenu* menu) override;

	QWidget* GetEditWidget() override;

	void SetupFullscreenWidget(FullscreenWidget* fullscreenWidget) override;

	void Save() override;

	void TryRefresh() override;

	EditorContext* GetEditorContext() { return _editorContext; }

	studiomdl::EditableStudioModel* GetEditableStudioModel() { return _editableStudioModel.get(); }

	StudioModelData* GetModelData() { return _modelData; }

	graphics::IGraphicsContext* GetGraphicsContext();

	graphics::TextureLoader* GetTextureLoader();

	studiomdl::IStudioModelRenderer* GetStudioModelRenderer() const;

	const std::vector<graphics::Scene*>& GetScenes() { return _scenes; }

	graphics::Scene* GetCurrentScene() const { return _currentScene; }

	void SetCurrentScene(graphics::Scene* scene);

	graphics::Scene* GetScene() { return _scene.get(); }

	CameraOperators* GetCameraOperators() const { return _cameraOperators; }

	void AddUndoCommand(QUndoCommand* command)
	{
		GetUndoStack()->push(command);
	}

	HLMVStudioModelEntity* GetEntity() { return _modelEntity.get(); }

	AxesEntity* GetAxesEntity() { return _axesEntity.get(); }

	BackgroundEntity* GetBackgroundEntity() { return _backgroundEntity.get(); }

	GroundEntity* GetGroundEntity() { return _groundEntity.get(); }

	PlayerHitboxEntity* GetPlayerHitboxEntity() { return _playerHitboxEntity.get(); }

	BoundingBoxEntity* GetBoundingBoxEntity() { return _boundingBoxEntity.get(); }

	ClippingBoxEntity* GetClippingBoxEntity() { return _clippingBoxEntity.get(); }

	CrosshairEntity* GetCrosshairEntity() { return _crosshairEntity.get(); }

	GuidelinesEntity* GetGuidelinesEntity() { return _guidelinesEntity.get(); }

	graphics::Scene* GetTextureScene() { return _textureScene.get(); }

	TextureEntity* GetTextureEntity() { return _textureEntity.get(); }

	TextureCameraOperator* GetTextureCameraOperator() { return _textureCameraOperator.get(); }

	Pose GetPose() const { return _pose; }

	bool CameraIsFirstPerson() const;

	void OnActivated();
	void OnDeactivated();

private:
	void CreateMainScene();
	void CreateTextureScene();

	void SaveEntityToSnapshot(StateSnapshot* snapshot);
	void LoadEntityFromSnapshot(StateSnapshot* snapshot);

signals:
	void Tick();

	void SaveSnapshot(StateSnapshot* snapshot);

	void LoadSnapshot(StateSnapshot* snapshot);

	void PoseChanged(Pose pose);

public slots:
	void SetPose(Pose pose)
	{
		_pose = pose;
		emit PoseChanged(pose);
	}

private slots:
	void OnTick();

	void OnIsActiveChanged(bool value);

	void OnResizeTexturesToPowerOf2Changed();

	void OnTextureFiltersChanged();

	void OnSceneIndexChanged(int index);

	void OnSceneWidgetMouseEvent(QMouseEvent* event);

	void OnSceneWidgetWheelEvent(QWheelEvent* event);

	void OnCameraChanged(SceneCameraOperator* previous, SceneCameraOperator* current);

	void OnPreviousCamera();
	void OnNextCamera();

	void OnCenterView(Axis axis, bool positive);
	void OnSaveView();
	void OnRestoreView();

	void OnFlipNormals();

	void OnDumpModelInfo();

	void OnTakeScreenshot();

public:
	RenderMode CurrentRenderMode = RenderMode::TEXTURE_SHADED;

	bool ShowHitboxes = false;
	bool ShowBones = false;
	bool ShowAttachments = false;
	bool ShowEyePosition = false;

	bool EnableBackfaceCulling = true;
	bool ShowWireframeOverlay = false;
	bool DrawShadows = false;
	bool FixShadowZFighting = false;
	bool ShowNormals = false;

	int DrawSingleBoneIndex = -1;
	int DrawSingleAttachmentIndex = -1;
	int DrawSingleHitboxIndex = -1;

	bool PlaySequence = true;

private:
	EditorContext* const _editorContext;
	StudioModelAssetProvider* const _provider;
	std::unique_ptr<studiomdl::EditableStudioModel> _editableStudioModel;
	StudioModelData* _modelData;
	const std::unique_ptr<EntityContext> _entityContext;

	std::vector<graphics::Scene*> _scenes;

	graphics::Scene* _currentScene{};

	std::unique_ptr<graphics::Scene> _scene;

	CameraOperators* _cameraOperators;

	SceneCameraOperator* _firstPersonCamera;

	QWidget* _editWidget{};

	std::shared_ptr<HLMVStudioModelEntity> _modelEntity;
	std::shared_ptr<AxesEntity> _axesEntity;
	std::shared_ptr<BackgroundEntity> _backgroundEntity;
	std::shared_ptr<GroundEntity> _groundEntity;
	std::shared_ptr<PlayerHitboxEntity> _playerHitboxEntity;
	std::shared_ptr<BoundingBoxEntity> _boundingBoxEntity;
	std::shared_ptr<ClippingBoxEntity> _clippingBoxEntity;
	std::shared_ptr<CrosshairEntity> _crosshairEntity;
	std::shared_ptr<GuidelinesEntity> _guidelinesEntity;

	std::unique_ptr<graphics::Scene> _textureScene;

	std::shared_ptr<TextureEntity> _textureEntity;

	std::unique_ptr<TextureCameraOperator> _textureCameraOperator;

	StateSnapshot _snapshot;

	//TODO: this is temporarily put here, but needs to be put somewhere else eventually
	Pose _pose = Pose::Sequences;

	QMetaObject::Connection _tickConnection;
};
}
