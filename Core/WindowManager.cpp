#include "WindowManager.h"
#include "cContainer.h"
#include "FileManager.h"

struct Container* WindowManager::topContainer;
std::map<uint16_t, cContainer*> WindowManager::containerWindows;

void WindowManager::SetTopContainer(cContainer* window) {
	topContainer = window->getContainer();
	LinkContainerIDWithWindow(topContainer->chunkID, window);
}

void WindowManager::CreateContainer(uint16_t containerID, cContainer* parent, wxConstraintPosition constraints) {
	cContainer* newContainer = new cContainer(false, parent);

	std::pair<struct Container*, struct Layout*> data = FileManager::getContainerAndLayoutByID(containerID);
	newContainer->CreateContainerUI(data.first, data.second);
	cContainer::applyParentContraints(newContainer, parent, constraints);

	LinkContainerIDWithWindow(containerID, newContainer);
}

void WindowManager::DestroyContainer(cContainer* containerWindow) {
	UnlinkContainerID(containerWindow->getContainer()->chunkID);
	containerWindow->Show(false);
	containerWindow->Destroy();
	delete containerWindow;
}

void WindowManager::ExecuteActionID(uint16_t actionID) {
	executeAction(FileManager::getActionByID(actionID));
}

cContainer* WindowManager::GetWindowByContainerID(uint16_t containerID) {
	return containerWindows[containerID];
}

void WindowManager::executeAction(struct Action* action) {
	// TODO test nullptr
	uint8_t actionType = action->actionType;
	switch (actionType) {
	case 0x1: {
		// link -- replace top container with what is linked
		//Link& link = static_cast<Link&>(action); // downcast
		Link* link = dynamic_cast<Link*>(action);
		if (link) {
			// TODO error check for null container?
			replaceContainers(GetWindowByContainerID(topContainer->chunkID), FileManager::getContainerByID(link->containerID));
		}
		else {
			//null
		}
	}
	break;
	case 0x2: {
		// replacement
		Replacement* replacement = dynamic_cast<Replacement*>(action);

		replaceContainers(GetWindowByContainerID(replacement->replaceID), FileManager::getContainerByID(replacement->replaceWithID));
	}
	break;
	}
}

void WindowManager::LinkContainerIDWithWindow(uint16_t containerID, cContainer* containerWindow) {
	containerWindows[containerID] = containerWindow;
}

void WindowManager::UnlinkContainerID(uint16_t containerID) {
	// TODO make sure this works, especially with erase.
	containerWindows.erase(containerID);
}

void WindowManager::replaceContainers(cContainer* target, struct Container* replaceWith) {
	Layout* layout = FileManager::getLayoutByID(replaceWith->layoutID);
	target->CreateContainerUI(replaceWith, layout);
}
