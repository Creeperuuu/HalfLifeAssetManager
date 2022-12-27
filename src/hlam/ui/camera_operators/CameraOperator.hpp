#pragma once

#include <cassert>

#include <QMouseEvent>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QWheelEvent>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "graphics/Camera.hpp"

#include "settings/GeneralSettings.hpp"

class QWidget;

class CameraOperator : public QObject, public graphics::ICameraOperator
{
public:
	virtual ~CameraOperator() = default;

	graphics::Camera* GetCamera() override final { return &_camera; }

	virtual void MouseEvent(QMouseEvent& event) = 0;

	virtual void WheelEvent(QWheelEvent& event)
	{
		event.ignore();
	}

protected:
	graphics::Camera _camera;
};

class SceneCameraOperator : public CameraOperator
{
	Q_OBJECT

public:
	SceneCameraOperator(GeneralSettings* generalSettings)
		: _generalSettings(generalSettings)
	{
		assert(_generalSettings);
	}

	virtual QString GetName() const = 0;

	virtual QWidget* CreateEditWidget() = 0;

	virtual void CenterView(const glm::vec3& targetOrigin, const glm::vec3& cameraOrigin, float pitch, float yaw) = 0;

	virtual void SaveView() = 0;

	virtual void RestoreView() = 0;

protected:
	float GetMouseXValue(float value)
	{
		if (_generalSettings->ShouldInvertMouseX())
		{
			value = -value;
		}

		value *= _generalSettings->GetNormalizedMouseSensitivity();

		return value;
	}

	float GetMouseYValue(float value)
	{
		if (_generalSettings->ShouldInvertMouseY())
		{
			value = -value;
		}

		value *= _generalSettings->GetNormalizedMouseSensitivity();

		return value;
	}

signals:
	void CameraPropertiesChanged();

protected:
	const GeneralSettings* const _generalSettings;

	glm::vec2 _oldCoordinates{0.f};
	Qt::MouseButtons _trackedButtons{};
};

Q_DECLARE_METATYPE(SceneCameraOperator*)
