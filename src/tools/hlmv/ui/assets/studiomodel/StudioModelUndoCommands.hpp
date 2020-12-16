#pragma once

#include <array>
#include <cassert>

#include <QString>
#include <QUndoStack>

#include <glm/vec3.hpp>

#include "ui/assets/studiomodel/StudioModelAsset.hpp"

namespace studiomdl
{
class CStudioModel;
}

namespace ui::assets::studiomodel
{
enum class ModelChangeId
{
	RenameBone,
	ChangeBoneParent,
	ChangeBoneFlags,
	ChangeBoneProperty,

	ChangeAttachmentName,
	ChangeAttachmentType,
	ChangeAttachmentBone,
	ChangeAttachmentOrigin,

	ChangeBoneControllerBone,
	ChangeBoneControllerRange,
	ChangeBoneControllerRest,
	ChangeBoneControllerIndex,
	ChangeBoneControllerType,

	ChangeModelFlags,
};

/**
*	@brief Base class for all model change events
*	Indicates that a change has been made to a model
*/
class ModelChangeEvent
{
public:
	ModelChangeEvent(ModelChangeId id)
		: _id(id)
	{
	}

	ModelChangeEvent(const ModelChangeEvent&) = delete;
	ModelChangeEvent& operator=(const ModelChangeEvent&) = delete;

	ModelChangeId GetId() const { return _id; }

private:
	const ModelChangeId _id;
};

/**
*	@brief Base class for all model change events that involve a change that occurred in a list
*/
class ModelListChangeEvent : public ModelChangeEvent
{
public:
	ModelListChangeEvent(ModelChangeId id, int sourceIndex, int destinationIndex = -1)
		: ModelChangeEvent(id)
		, _sourceIndex(sourceIndex)
		, _destinationIndex(destinationIndex)
	{
	}

	/**
	*	@brief The list entry being changed. If this change involves the moving of a list entry, this is where the entry was moved from
	*/
	int GetSourceIndex() const { return _sourceIndex; }

	/**
	*	@brief If this change involves the moving of a list entry, this is where the entry was moved to
	*/
	int GetDestinationIndex() const { return _destinationIndex; }

private:
	const int _sourceIndex;
	const int _destinationIndex;
};

/**
*	@brief Base class for all undo commands related to Studiomodel editing
*/
class BaseModelUndoCommand : public QUndoCommand
{
protected:
	BaseModelUndoCommand(StudioModelAsset* asset, ModelChangeId id)
		: _asset(asset)
		, _id(id)
	{
		assert(_asset);
	}

public:
	int id() const override final { return static_cast<int>(_id); }

protected:
	StudioModelAsset* const _asset;

protected:
	const ModelChangeId _id;
};

template<typename T>
class ModelUndoCommand : public BaseModelUndoCommand
{
protected:
	ModelUndoCommand(StudioModelAsset* asset, ModelChangeId id, const T& oldValue, const T& newValue)
		: BaseModelUndoCommand(asset, id)
		, _oldValue(oldValue)
		, _newValue(newValue)
	{
	}

public:
	bool mergeWith(const QUndoCommand* other) override
	{
		if (id() != other->id())
		{
			return false;
		}

		_newValue = static_cast<const ModelUndoCommand*>(other)->_newValue;

		return true;
	}

	void undo() override
	{
		Apply(_newValue, _oldValue);
		_asset->EmitModelChanged(ModelChangeEvent{_id});
	}

	void redo() override
	{
		Apply(_oldValue, _newValue);
		_asset->EmitModelChanged(ModelChangeEvent{_id});
	}

protected:
	virtual void Apply(const T& oldValue, const T& newValue) = 0;

protected:
	const T _oldValue;
	T _newValue;
};

template<typename T>
class ModelListUndoCommand : public BaseModelUndoCommand
{
protected:
	ModelListUndoCommand(StudioModelAsset* asset, ModelChangeId id, int index, const T& oldValue, const T& newValue)
		: BaseModelUndoCommand(asset, id)
		, _index(index)
		, _oldValue(oldValue)
		, _newValue(newValue)
	{
	}

public:
	bool mergeWith(const QUndoCommand* other) override
	{
		if (id() != other->id())
		{
			return false;
		}

		_newValue = static_cast<const ModelListUndoCommand*>(other)->_newValue;

		return true;
	}

	void undo() override
	{
		Apply(_index, _newValue, _oldValue);
		_asset->EmitModelChanged(ModelListChangeEvent{_id, _index});
	}

	void redo() override
	{
		Apply(_index, _oldValue, _newValue);
		_asset->EmitModelChanged(ModelListChangeEvent{_id, _index});
	}

protected:
	virtual void Apply(int index, const T& oldValue, const T& newValue) = 0;

protected:
	const int _index;
	const T _oldValue;
	T _newValue;
};

class BoneRenameCommand : public ModelListUndoCommand<QString>
{
public:
	BoneRenameCommand(StudioModelAsset* asset, int boneIndex, const QString& oldName, const QString& newName)
		: ModelListUndoCommand(asset, ModelChangeId::RenameBone, boneIndex, oldName, newName)
	{
		setText("Rename bone");
	}

protected:
	void Apply(int index, const QString& oldValue, const QString& newValue) override;
};

class ChangeBoneParentCommand : public ModelListUndoCommand<int>
{
public:
	ChangeBoneParentCommand(StudioModelAsset* asset, int boneIndex, int oldParent, int newParent)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeBoneParent, boneIndex, oldParent, newParent)
	{
		setText("Change bone parent");
	}

protected:
	void Apply(int index, const int& oldValue, const int& newValue) override;
};

class ChangeBoneFlagsCommand : public ModelListUndoCommand<int>
{
public:
	ChangeBoneFlagsCommand(StudioModelAsset* asset, int boneIndex, int oldFlags, int newFlags)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeBoneParent, boneIndex, oldFlags, newFlags)
	{
		setText("Change bone flags");
	}

protected:
	void Apply(int index, const int& oldValue, const int& newValue) override;
};

struct ChangeBoneProperties
{
	std::array<glm::vec3, 2> Values;
	std::array<glm::vec3, 2> Scales;
};

class ChangeBonePropertyCommand : public ModelListUndoCommand<ChangeBoneProperties>
{
public:
	ChangeBonePropertyCommand(StudioModelAsset* asset, int boneIndex, const ChangeBoneProperties& oldProperties, const ChangeBoneProperties& newProperties)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeBoneProperty, boneIndex, oldProperties, newProperties)
	{
		setText("Change bone property");
	}

protected:
	void Apply(int index, const ChangeBoneProperties& oldValue, const ChangeBoneProperties& newValue) override;
};

class ChangeAttachmentNameCommand : public ModelListUndoCommand<QString>
{
public:
	ChangeAttachmentNameCommand(StudioModelAsset* asset, int attachmentIndex, const QString& oldName, const QString& newName)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeAttachmentName, attachmentIndex, oldName, newName)
	{
		setText("Change attachment name");
	}

protected:
	void Apply(int index, const QString& oldValue, const QString& newValue) override;
};

class ChangeAttachmentTypeCommand : public ModelListUndoCommand<int>
{
public:
	ChangeAttachmentTypeCommand(StudioModelAsset* asset, int attachmentIndex, int oldType, int newType)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeAttachmentType, attachmentIndex, oldType, newType)
	{
		setText("Change attachment type");
	}

protected:
	void Apply(int index, const int& oldValue, const int& newValue) override;
};

class ChangeAttachmentBoneCommand : public ModelListUndoCommand<int>
{
public:
	ChangeAttachmentBoneCommand(StudioModelAsset* asset, int attachmentIndex, int oldBone, int newBone)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeAttachmentBone, attachmentIndex, oldBone, newBone)
	{
		setText("Change attachment bone");
	}

protected:
	void Apply(int index, const int& oldValue, const int& newValue) override;
};

class ChangeAttachmentOriginCommand : public ModelListUndoCommand<glm::vec3>
{
public:
	ChangeAttachmentOriginCommand(StudioModelAsset* asset, int attachmentIndex, const glm::vec3& oldOrigin, const glm::vec3& newOrigin)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeAttachmentOrigin, attachmentIndex, oldOrigin, newOrigin)
	{
		setText("Change attachment origin");
	}

protected:
	void Apply(int index, const glm::vec3& oldValue, const glm::vec3& newValue) override;
};

class ChangeBoneControllerBoneCommand : public ModelListUndoCommand<int>
{
public:
	ChangeBoneControllerBoneCommand(StudioModelAsset* asset, int boneControllerIndex, int oldBone, int newBone)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeBoneControllerBone, boneControllerIndex, oldBone, newBone)
	{
		setText("Change bone controller bone");
	}

protected:
	void Apply(int index, const int& oldValue, const int& newValue) override;
};

struct ChangeBoneControllerRange
{
	float Start;
	float End;
};

class ChangeBoneControllerRangeCommand : public ModelListUndoCommand<ChangeBoneControllerRange>
{
public:
	ChangeBoneControllerRangeCommand(StudioModelAsset* asset, int boneControllerIndex,
		const ChangeBoneControllerRange& oldRange, const ChangeBoneControllerRange& newRange)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeBoneControllerRange, boneControllerIndex, oldRange, newRange)
	{
		setText("Change bone controller range");
	}

protected:
	void Apply(int index, const ChangeBoneControllerRange& oldValue, const ChangeBoneControllerRange& newValue) override;
};

class ChangeBoneControllerRestCommand : public ModelListUndoCommand<int>
{
public:
	ChangeBoneControllerRestCommand(StudioModelAsset* asset, int boneControllerIndex, int oldRest, int newRest)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeBoneControllerRest, boneControllerIndex, oldRest, newRest)
	{
		setText("Change bone controller rest");
	}

protected:
	void Apply(int index, const int& oldValue, const int& newValue) override;
};

class ChangeBoneControllerIndexCommand : public ModelListUndoCommand<int>
{
public:
	ChangeBoneControllerIndexCommand(StudioModelAsset* asset, int boneControllerIndex, int oldIndex, int newIndex)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeBoneControllerIndex, boneControllerIndex, oldIndex, newIndex)
	{
		setText("Change bone controller index");
	}

protected:
	void Apply(int index, const int& oldValue, const int& newValue) override;
};

class ChangeBoneControllerTypeCommand : public ModelListUndoCommand<int>
{
public:
	ChangeBoneControllerTypeCommand(StudioModelAsset* asset, int boneControllerIndex, int oldType, int newType)
		: ModelListUndoCommand(asset, ModelChangeId::ChangeBoneControllerType, boneControllerIndex, oldType, newType)
	{
		setText("Change bone controller type");
	}

protected:
	void Apply(int index, const int& oldValue, const int& newValue) override;
};

class ChangeModelFlagsCommand : public ModelUndoCommand<int>
{
public:
	ChangeModelFlagsCommand(StudioModelAsset* asset, int oldFlags, int newFlags)
		: ModelUndoCommand(asset, ModelChangeId::ChangeModelFlags, oldFlags, newFlags)
	{
		setText("Change model flags");
	}

protected:
	void Apply(const int& oldValue, const int& newValue) override;
};
}
