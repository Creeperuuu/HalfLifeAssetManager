#include <cstdio>
#include <cstring>

#include "application/ApplicationBuilder.hpp"
#include "assets/AssetIO.hpp"
#include "plugins/quake1/Quake1AssetManagerPlugin.hpp"

#include "ui/EditorContext.hpp"

#include "ui/assets/Assets.hpp"

#include "ui/settings/GeneralSettings.hpp"

const QString AliasModelExtension{QStringLiteral("mdl")};

constexpr char AliasModelHeaderId[] = "IDPO";

class Quake1AliasModelAssetProvider final : public AssetProvider
{
public:
	explicit Quake1AliasModelAssetProvider(GeneralSettings* generalSettings)
		: _generalSettings(generalSettings)
	{
	}

	QString GetProviderName() const override { return "Quake 1 Alias model"; }

	QStringList GetFileTypes() const override { return {AliasModelExtension}; }

	QString GetPreferredFileType() const override { return AliasModelExtension; }

	ProviderFeatures GetFeatures() const override { return ProviderFeature::AssetLoading; }

	QMenu* CreateToolMenu(EditorContext* editorContext) override { return nullptr; }

	bool CanLoad(const QString& fileName, FILE* file) const override
	{
		int id;

		if (fread(&id, sizeof(id), 1, file) == 1)
		{
			if (strncmp(reinterpret_cast<const char*>(&id), AliasModelHeaderId, 4) == 0)
			{
				return true;
			}
		}

		return false;
	}

	std::variant<std::unique_ptr<Asset>, AssetLoadInExternalProgram> Load(
		EditorContext* editorContext, const QString& fileName, FILE* file) override
	{
		const auto result = editorContext->TryLaunchExternalProgram(
			_generalSettings->GetQuake1ModelViewerFileName(),
			QStringList(fileName),
			"This is a Quake 1 Alias model which requires it to be loaded in Quake 1 Model Viewer.");

		if (result != LaunchExternalProgramResult::Failed)
		{
			return AssetLoadInExternalProgram{.Loaded = result == LaunchExternalProgramResult::Success};
		}

		throw AssetException(std::string{"File \""} + fileName.toStdString()
			+ "\" is a Quake 1 Alias model and cannot be opened by this program."
			+ "\nSet the Quake 1 Model Viewer executable setting to open the model through that program instead.");
	}

private:
	GeneralSettings* const _generalSettings;
};

bool Quake1AssetManagerPlugin::Initialize(ApplicationBuilder& builder)
{
	auto quake1AliasModelAssetProvider = std::make_unique<Quake1AliasModelAssetProvider>(builder.GeneralSettings);
	builder.AssetProviderRegistry->AddProvider(std::move(quake1AliasModelAssetProvider));
	return true;
}
