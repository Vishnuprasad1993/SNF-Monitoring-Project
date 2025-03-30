
#include "ActionInit.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"

namespace G4_BREMS {
	void ActionInit::Build() const {
		SetUserAction(new PrimaryGeneratorAction);

		SteppingAction* steppingAction = new SteppingAction(nullptr);

		// Create run action with the stepping action pointer
		RunAction* runAction = new RunAction(steppingAction);

		// Now update the stepping action with the run action pointer
		steppingAction->SetRunAction(runAction);

		// Set user actions
		SetUserAction(runAction);
		SetUserAction(steppingAction);


		//RunAction* runAction = new RunAction();
		//SetUserAction(runAction);

		//SetUserAction(new SteppingAction(runAction));
	}

	void ActionInit::BuildForMaster() const {
		//SetUserAction(new RunAction());
		SetUserAction(new RunAction(nullptr));

	}
}
