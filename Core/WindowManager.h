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
	static void ExecuteActionID(uint16_t actionID);
	static cContainer* GetWindowByContainerID(uint16_t containerID);
	static void CreateContainer(uint16_t containerID, cContainer* parent, wxConstraintPosition constraints);
	static void DestroyContainer(cContainer* containerWindow);
	static void SetTopContainer(cContainer* window);
private:
	static struct Container* topContainer;
	static std::map<uint16_t, cContainer*> containerWindows;

	static void LinkContainerIDWithWindow(uint16_t containerID, cContainer* containerWindow);
	static void UnlinkContainerID(uint16_t containerID);
	static void executeAction(struct Action* action);
	static void replaceContainers(cContainer* target, struct Container* replaceWith);
	static void replaceContainerElementIndexWithContent(uint16_t containerID, uint8_t index, uint16_t contentID);
};

