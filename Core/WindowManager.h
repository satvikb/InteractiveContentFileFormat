#pragma once
#include "reader.h"
#include <vector>

struct wxConstraintPosition;
class cContainer;
// Keeps a reference to the top container.
// Handles execution of actions
// "static" class
class WindowManager
{
public:
	static void ExecuteActionID(uint32_t actionID);
	static cContainer* GetWindowByContainerID(uint32_t containerID);
	static void CreateContainer(uint32_t containerID, cContainer* parent, wxConstraintPosition constraints);
	static void DestroyContainer(cContainer* containerWindow);
	static void SetTopContainer(cContainer* window);
private:
	static struct Container* topContainer;
	static std::map<uint16_t, cContainer*> containerWindows;

	static void LinkContainerIDWithWindow(uint32_t containerID, cContainer* containerWindow);
	static void UnlinkContainerID(uint32_t containerID);
	static void executeAction(struct Action* action);
	static void replaceContainers(cContainer* target, struct Container* replaceWith);
	static void replaceContainerElementIndexWithContent(uint32_t containerID, uint8_t index, uint32_t contentID);
};

