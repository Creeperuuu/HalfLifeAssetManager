#pragma once

#include <QObject>
#include <QSettings>
#include <QString>

#include "graphics/TextureLoader.hpp"

enum class GuidelinesAspectRatio
{
	FourThree,
	SixteenNine,
	SixteenTen
};

class ApplicationSettings final : public QObject
{
	Q_OBJECT

public:
	static constexpr bool DefaultUseSingleInstance{true};
	static constexpr bool DefaultPauseAnimationsOnTimelineClick{true};
	static constexpr bool DefaultTransparentScreenshots{false};
	static constexpr bool DefaultAllowTabCloseWithMiddleClick{false};
	static constexpr bool DefaultOneAssetAtATime{false};
	static constexpr bool DefaultPromptExternalProgramLaunch{true};

	static constexpr int DefaultTickRate{60};
	static constexpr int MinimumTickRate{1};
	static constexpr int MaximumTickRate{1000};

	static constexpr int DefaultMouseSensitivity{5};
	static constexpr int MinimumMouseSensitivity{1};
	static constexpr int MaximumMouseSensitivity{20};

	static constexpr int DefaultMouseWheelSpeed{5};
	static constexpr int MinimumMouseWheelSpeed{1};
	static constexpr int MaximumMouseWheelSpeed{2048};

	static constexpr bool DefaultEnableAudioPlayback{true};
	static constexpr bool DefaultPlaySounds{true};
	static constexpr bool DefaultFramerateAffectsPitch{false};

	static constexpr bool DefaultEnableVSync{true};
	static constexpr bool DefaultPowerOf2Textures{false};

	static constexpr int DefaultMSAALevel{0};

	static constexpr graphics::TextureFilter DefaultMinFilter{graphics::TextureFilter::Linear};
	static constexpr graphics::TextureFilter DefaultMagFilter{graphics::TextureFilter::Linear};
	static constexpr graphics::MipmapFilter DefaultMipmapFilter{graphics::MipmapFilter::None};

	static constexpr GuidelinesAspectRatio DefaultGuidelinesAspectRatio{GuidelinesAspectRatio::SixteenNine};

	static const inline QString StudiomdlCompilerFileNameKey{
		QStringLiteral("ExternalPrograms/StudioMdlCompilerFileName")};
	static const inline QString StudiomdlDecompilerFileNameKey{
		QStringLiteral("ExternalPrograms/StudioMdlDecompilerFileName")};
	static const inline QString XashModelViewerFileNameKey{
		QStringLiteral("ExternalPrograms/XashModelViewerFileName")};
	static const inline QString Quake1ModelViewerFileNameKey{
		QStringLiteral("ExternalPrograms/Quake1ModelViewerFileName")};
	static const inline QString Source1ModelViewerFileNameKey{
		QStringLiteral("ExternalPrograms/Source1ModelViewerFileName")};

	explicit ApplicationSettings(QSettings* settings)
		: _settings(settings)
	{
	}

	void LoadSettings()
	{
		_settings->beginGroup("General");
		PauseAnimationsOnTimelineClick = _settings->value("PauseAnimationsOnTimelineClick", DefaultPauseAnimationsOnTimelineClick).toBool();
		OneAssetAtATime = _settings->value("OneAssetAtATime", DefaultOneAssetAtATime).toBool();
		PromptExternalProgramLaunch = _settings->value(
			"PromptExternalProgramLaunch", DefaultPromptExternalProgramLaunch).toBool();
		_tickRate = std::clamp(_settings->value("TickRate", DefaultTickRate).toInt(), MinimumTickRate, MaximumTickRate);

		GuidelinesAspectRatio = static_cast<::GuidelinesAspectRatio>(_settings->value(
			"GuidelinesAspectRatio", static_cast<int>(GuidelinesAspectRatio::SixteenNine)).toInt());

		GuidelinesAspectRatio = std::clamp(
			GuidelinesAspectRatio, ::GuidelinesAspectRatio::FourThree, ::GuidelinesAspectRatio::SixteenTen);
		_settings->endGroup();

		_settings->beginGroup("Mouse");
		_invertMouseX = _settings->value("InvertMouseX", false).toBool();
		_invertMouseY = _settings->value("InvertMouseY", false).toBool();
		_mouseSensitivity = std::clamp(_settings->value("MouseSensitivity", DefaultMouseSensitivity).toInt(), MinimumMouseSensitivity, MaximumMouseSensitivity);
		_mouseWheelSpeed = std::clamp(_settings->value("MouseWheelSpeed", DefaultMouseWheelSpeed).toInt(), MinimumMouseWheelSpeed, MaximumMouseWheelSpeed);
		_settings->endGroup();

		_settings->beginGroup("Audio");
		_enableAudioPlayback = _settings->value("EnableAudioPlayback", DefaultEnableAudioPlayback).toBool();
		PlaySounds = _settings->value("PlaySounds", DefaultPlaySounds).toBool();
		FramerateAffectsPitch = _settings->value("FramerateAffectsPitch", DefaultFramerateAffectsPitch).toBool();
		_settings->endGroup();

		_settings->beginGroup("Graphics");

		_powerOf2Textures = _settings->value("PowerOf2Textures", DefaultPowerOf2Textures).toBool();

		_settings->beginGroup("TextureFilters");
		_minFilter = static_cast<graphics::TextureFilter>(std::clamp(
			_settings->value("Min", static_cast<int>(DefaultMinFilter)).toInt(),
			static_cast<int>(graphics::TextureFilter::First),
			static_cast<int>(graphics::TextureFilter::Last)));

		_magFilter = static_cast<graphics::TextureFilter>(std::clamp(
			_settings->value("Mag", static_cast<int>(DefaultMagFilter)).toInt(),
			static_cast<int>(graphics::TextureFilter::First),
			static_cast<int>(graphics::TextureFilter::Last)));

		_mipmapFilter = static_cast<graphics::MipmapFilter>(std::clamp(
			_settings->value("Mipmap", static_cast<int>(DefaultMipmapFilter)).toInt(),
			static_cast<int>(graphics::MipmapFilter::First),
			static_cast<int>(graphics::MipmapFilter::Last)));
		_settings->endGroup();

		_msaaLevel = _settings->value("MSAALevel", DefaultMSAALevel).toInt();
		TransparentScreenshots = _settings->value("TransparentScreenshots", DefaultTransparentScreenshots).toBool();
		_settings->endGroup();
	}

	void SaveSettings()
	{
		_settings->beginGroup("General");
		_settings->setValue("PauseAnimationsOnTimelineClick", PauseAnimationsOnTimelineClick);
		_settings->setValue("OneAssetAtATime", OneAssetAtATime);
		_settings->setValue("PromptExternalProgramLaunch", PromptExternalProgramLaunch);
		_settings->setValue("TickRate", _tickRate);
		_settings->setValue("GuidelinesAspectRatio", static_cast<int>(GuidelinesAspectRatio));
		_settings->endGroup();

		_settings->beginGroup("Mouse");
		_settings->setValue("InvertMouseX", _invertMouseX);
		_settings->setValue("InvertMouseY", _invertMouseY);
		_settings->setValue("MouseSensitivity", _mouseSensitivity);
		_settings->setValue("MouseWheelSpeed", _mouseWheelSpeed);
		_settings->endGroup();

		_settings->beginGroup("Audio");
		_settings->setValue("EnableAudioPlayback", _enableAudioPlayback);
		_settings->setValue("PlaySounds", PlaySounds);
		_settings->setValue("FramerateAffectsPitch", FramerateAffectsPitch);
		_settings->endGroup();

		_settings->beginGroup("Graphics");
		_settings->setValue("PowerOf2Textures", _powerOf2Textures);

		_settings->beginGroup("TextureFilters");
		_settings->setValue("Min", static_cast<int>(_minFilter));
		_settings->setValue("Mag", static_cast<int>(_magFilter));
		_settings->setValue("Mipmap", static_cast<int>(_mipmapFilter));
		_settings->endGroup();

		_settings->setValue("MSAALevel", _msaaLevel);
		_settings->setValue("TransparentScreenshots", DefaultTransparentScreenshots);
		_settings->endGroup();
	}

	bool ShouldUseSingleInstance() const
	{
		return _settings->value("Startup/UseSingleInstance", DefaultUseSingleInstance).toBool();
	}

	void SetUseSingleInstance(bool value)
	{
		_settings->setValue("Startup/UseSingleInstance", value);
	}

	bool ShouldAllowTabCloseWithMiddleClick() const
	{
		return _settings->value("General/AllowTabCloseWithMiddleClick", DefaultAllowTabCloseWithMiddleClick).toBool();
	}

	void SetAllowTabCloseWithMiddleClick(bool value)
	{
		_settings->setValue("General/AllowTabCloseWithMiddleClick", value);
	}

	int GetTickRate() const { return _tickRate; }

	void SetTickRate(int value)
	{
		if (_tickRate != value)
		{
			_tickRate = value;
			emit TickRateChanged(_tickRate);
		}
	}

	bool ShouldInvertMouseX() const { return _invertMouseX; }

	void SetInvertMouseX(bool value)
	{
		_invertMouseX = value;
	}

	bool ShouldInvertMouseY() const { return _invertMouseY; }

	void SetInvertMouseY(bool value)
	{
		_invertMouseY = value;
	}

	int GetMouseSensitivity() const { return _mouseSensitivity; }

	void SetMouseSensitivity(int value)
	{
		_mouseSensitivity = value;
	}

	float GetNormalizedMouseSensitivity() const
	{
		return static_cast<float>(_mouseSensitivity) / DefaultMouseSensitivity;
	}

	int GetMouseWheelSpeed() const { return _mouseWheelSpeed; }

	void SetMouseWheelSpeed(int value)
	{
		_mouseWheelSpeed = value;
	}

	bool ShouldEnableAudioPlayback() const { return _enableAudioPlayback; }

	void SetEnableAudioPlayback(bool value)
	{
		_enableAudioPlayback = value;
	}

	bool PlaySounds = DefaultPlaySounds;
	bool FramerateAffectsPitch = DefaultFramerateAffectsPitch;

	bool ShouldEnableVSync() const
	{
		return _settings->value("Graphics/EnableVSync", DefaultEnableVSync).toBool();
	}

	void SetEnableVSync(bool value)
	{
		_settings->setValue("Graphics/EnableVSync", value);
	}

	bool ShouldResizeTexturesToPowerOf2() const { return _powerOf2Textures; }

	void SetResizeTexturesToPowerOf2(bool value)
	{
		if (_powerOf2Textures != value)
		{
			_powerOf2Textures = value;
			emit ResizeTexturesToPowerOf2Changed(value);
		}
	}

	graphics::TextureFilter GetMinFilter() const { return _minFilter; }

	graphics::TextureFilter GetMagFilter() const { return _magFilter; }

	graphics::MipmapFilter GetMipmapFilter() const { return _mipmapFilter; }

	void SetTextureFilters(graphics::TextureFilter minFilter, graphics::TextureFilter magFilter, graphics::MipmapFilter mipmapFilter)
	{
		if (_minFilter == minFilter && _magFilter == magFilter && _mipmapFilter == mipmapFilter)
		{
			return;
		}

		_minFilter = minFilter;
		_magFilter = magFilter;
		_mipmapFilter = mipmapFilter;

		emit TextureFiltersChanged(_minFilter, _magFilter, _mipmapFilter);
	}

	int GetMSAALevel() const { return _msaaLevel; }

	void SetMSAALevel(int msaaLevel)
	{
		if (_msaaLevel != msaaLevel)
		{
			_msaaLevel = msaaLevel;
			emit MSAALevelChanged(_msaaLevel);
		}
	}

	QString GetStudiomdlCompilerFileName() const
	{
		return _settings->value(StudiomdlCompilerFileNameKey).toString();
	}

	void SetStudiomdlCompilerFileName(const QString& fileName)
	{
		_settings->setValue(StudiomdlCompilerFileNameKey, fileName);
	}

	QString GetStudiomdlDecompilerFileName() const
	{
		return _settings->value(StudiomdlDecompilerFileNameKey).toString();
	}

	void SetStudiomdlDecompilerFileName(const QString& fileName)
	{
		_settings->setValue(StudiomdlDecompilerFileNameKey, fileName);
	}

	QString GetXashModelViewerFileName() const
	{
		return _settings->value(XashModelViewerFileNameKey).toString();
	}

	void SetXashModelViewerFileName(const QString& fileName)
	{
		_settings->setValue(XashModelViewerFileNameKey, fileName);
	}

	QString GetQuake1ModelViewerFileName() const
	{
		return _settings->value(Quake1ModelViewerFileNameKey).toString();
	}

	void SetQuake1ModelViewerFileName(const QString& fileName)
	{
		_settings->setValue(Quake1ModelViewerFileNameKey, fileName);
	}

	QString GetSource1ModelViewerFileName() const
	{
		return _settings->value(Source1ModelViewerFileNameKey).toString();
	}

	void SetSource1ModelViewerFileName(const QString& fileName)
	{
		_settings->setValue(Source1ModelViewerFileNameKey, fileName);
	}

signals:
	void TickRateChanged(int value);

	void ResizeTexturesToPowerOf2Changed(bool value);

	void TextureFiltersChanged(
		graphics::TextureFilter minFilter, graphics::TextureFilter magFilter, graphics::MipmapFilter mipmapFilter);

	void MSAALevelChanged(int msaaLevel);

public:
	bool PauseAnimationsOnTimelineClick{DefaultPauseAnimationsOnTimelineClick};
	bool OneAssetAtATime{DefaultOneAssetAtATime};
	bool PromptExternalProgramLaunch{DefaultPromptExternalProgramLaunch};
	bool TransparentScreenshots{DefaultTransparentScreenshots};

	GuidelinesAspectRatio GuidelinesAspectRatio{DefaultGuidelinesAspectRatio};

private:
	QSettings* const _settings;

	int _tickRate{DefaultTickRate};

	bool _invertMouseX{false};
	bool _invertMouseY{false};

	int _mouseSensitivity{DefaultMouseSensitivity};

	int _mouseWheelSpeed{DefaultMouseWheelSpeed};

	bool _enableAudioPlayback{DefaultEnableAudioPlayback};

	bool _powerOf2Textures{DefaultPowerOf2Textures};

	graphics::TextureFilter _minFilter{DefaultMinFilter};
	graphics::TextureFilter _magFilter{DefaultMagFilter};
	graphics::MipmapFilter _mipmapFilter{DefaultMipmapFilter};

	int _msaaLevel{DefaultMSAALevel};
};
