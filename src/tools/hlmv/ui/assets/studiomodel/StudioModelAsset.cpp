#include <stdexcept>

#include <QAction>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

#include "entity/CHLMVStudioModelEntity.h"
#include "game/entity/CBaseEntity.h"
#include "game/entity/CBaseEntityList.h"
#include "game/entity/CEntityManager.h"

#include "graphics/Scene.hpp"

#include "ui/EditorContext.hpp"
#include "ui/FullscreenWidget.hpp"
#include "ui/SceneWidget.hpp"

#include "ui/assets/studiomodel/StudioModelAsset.hpp"
#include "ui/assets/studiomodel/StudioModelEditWidget.hpp"

#include "ui/camera_operators/ArcBallCameraOperator.hpp"
#include "ui/camera_operators/CameraOperator.hpp"

#include "ui/settings/StudioModelSettings.hpp"

namespace ui::assets::studiomodel
{
StudioModelAsset::StudioModelAsset(QString&& fileName,
	EditorContext* editorContext, const StudioModelAssetProvider* provider, std::unique_ptr<studiomdl::CStudioModel>&& studioModel)
	: Asset(std::move(fileName))
	, _editorContext(editorContext)
	, _provider(provider)
	, _studioModel(std::move(studioModel))
	, _scene(std::make_unique<graphics::Scene>(editorContext->GetSoundSystem()))
{
	PushInputSink(this);

	//TODO: need to initialize the background color to its default value here, as specified in the options dialog
	SetBackgroundColor({63, 127, 127});
	_scene->FloorLength = _provider->GetSettings()->GetFloorLength();

	auto entity = static_cast<CHLMVStudioModelEntity*>(_scene->GetEntityContext()->EntityManager->Create("studiomodel", _scene->GetEntityContext(),
		glm::vec3(), glm::vec3(), false));

	if (nullptr != entity)
	{
		entity->SetModel(GetStudioModel());

		entity->Spawn();

		_scene->SetEntity(entity);
	}

	_cameraOperator = std::make_unique<camera_operators::ArcBallCameraOperator>();

	connect(_provider->GetSettings(), &settings::StudioModelSettings::FloorLengthChanged, this, &StudioModelAsset::OnFloorLengthChanged);
}

StudioModelAsset::~StudioModelAsset()
{
	PopInputSink();
}

void StudioModelAsset::PopulateAssetMenu(QMenu* menu)
{
	menu->addAction("Edit QC File...", []
		{
			const QString fileName{QFileDialog::getOpenFileName(nullptr, "Select QC File", {}, "QC files (*.qc);;All Files (*.*)")};

			if (!fileName.isEmpty())
			{
				if (!QDesktopServices::openUrl(QUrl::fromLocalFile(fileName)))
				{
					QMessageBox::critical(nullptr, "Error", "Unable to start default program\nMake sure the .qc extension is associated with a program");
				}
			}
		});
}

QWidget* StudioModelAsset::CreateEditWidget(EditorContext* editorContext)
{
	auto editWidget = new StudioModelEditWidget(editorContext, this);

	editWidget->connect(editWidget->GetSceneWidget(), &SceneWidget::MouseEvent, this, &StudioModelAsset::OnSceneWidgetMouseEvent);

	return editWidget;
}

void StudioModelAsset::SetupFullscreenWidget(EditorContext* editorContext, FullscreenWidget* fullscreenWidget)
{
	const auto sceneWidget = new SceneWidget(GetScene(), fullscreenWidget);

	fullscreenWidget->setCentralWidget(sceneWidget->GetContainer());

	sceneWidget->connect(editorContext, &EditorContext::Tick, sceneWidget, &SceneWidget::requestUpdate);
	sceneWidget->connect(sceneWidget, &SceneWidget::MouseEvent, this, &StudioModelAsset::OnSceneWidgetMouseEvent);

	//Filter key events on the scene widget so we can capture exit even if it has focus
	sceneWidget->installEventFilter(fullscreenWidget);
}

void StudioModelAsset::Save(const QString& fileName)
{
	_provider->Save(fileName, *this);
}

void StudioModelAsset::OnMouseEvent(QMouseEvent* event)
{
	_cameraOperator->MouseEvent(*_editorContext->GetGeneralSettings(), *_scene->GetCamera(), *event);
}

void StudioModelAsset::OnSceneWidgetMouseEvent(QMouseEvent* event)
{
	if (!_inputSinks.empty())
	{
		_inputSinks.top()->OnMouseEvent(event);
	}
}

void StudioModelAsset::OnFloorLengthChanged(int length)
{
	_scene->FloorLength = length;
}

bool StudioModelAssetProvider::CanLoad(const QString& fileName) const
{
	//TODO:
	return true;
}

std::unique_ptr<Asset> StudioModelAssetProvider::Load(EditorContext* editorContext, const QString& fileName) const
{
	//TODO: this throws specific exceptions. They need to be generalized so the caller can handle them
	auto studioModel = studiomdl::LoadStudioModel(fileName.toStdString().c_str());

	return std::make_unique<StudioModelAsset>(QString{fileName}, editorContext, this, std::move(studioModel));
}

void StudioModelAssetProvider::Save(const QString& fileName, Asset& asset) const
{
	//TODO:
	if (asset.GetAssetType() == GetAssetType())
	{
		Save(fileName, static_cast<StudioModelAsset&>(asset));
	}
	else
	{
		//TODO: maybe allow conversion from other asset types to this one, otherwise remove this method from the provider API
		throw std::runtime_error("Cannot convert asset type to studiomodel");
	}
}

void StudioModelAssetProvider::Save(const QString& fileName, StudioModelAsset& asset) const
{
	//TODO:
}
}
