
#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"
#include <map>

class G4Run;

namespace G4_BREMS {

    class SteppingAction;

    class RunAction : public G4UserRunAction
    {
    public:
        RunAction(SteppingAction* steppingAction);
        virtual ~RunAction();

        virtual void BeginOfRunAction(const G4Run*);
        virtual void EndOfRunAction(const G4Run*);

        void IncrementTileCount() { fTileCount++; fAccTileCount += 1; }
        void IncrementCladCount() { fCladCount++; fAccCladCount += 1; }
        void IncrementCoreCount() { fCoreCount++; fAccCoreCount += 1; }
        void IncrementSipmCount() { fSipmCount++; fAccSipmCount += 1; }
        void IncrementOtherCount() { fOtherCount++; fAccOtherCount += 1; }

        void IncrementPhotonsEnteredFiber() { fPhotonsEnteredFiber++; fAccPhotonsEnteredFiber += 1; }
        void IncrementPhotonsExitedFiber() { fPhotonsExitedFiber++; fAccPhotonsExitedFiber += 1; }
        void IncrementPhotonsAbsorbedFiber() { fPhotonsAbsorbedFiber++; fAccPhotonsAbsorbedFiber += 1; }

        void AddProcessCount(const G4String& volume, const G4String& processName, bool isCreationProcess);
        G4double CalculateTrappingEfficiency() const;

    private:
        G4int fTileCount;
        G4int fCladCount;
        G4int fCoreCount;
        G4int fSipmCount;
        G4int fOtherCount;
        G4int fPhotonsEnteredFiber;
        G4int fPhotonsExitedFiber;
        G4int fPhotonsAbsorbedFiber;
        G4int chargeDeposited;


        G4Accumulable<G4int> fAccTileCount;
        G4Accumulable<G4int> fAccCladCount;
        G4Accumulable<G4int> fAccCoreCount;
        G4Accumulable<G4int> fAccSipmCount;
        G4Accumulable<G4int> fAccOtherCount;
        G4Accumulable<G4int> fAccPhotonsEnteredFiber;
        G4Accumulable<G4int> fAccPhotonsExitedFiber;
        G4Accumulable<G4int> fAccPhotonsAbsorbedFiber;

        std::map<G4String, G4Accumulable<G4int>*> fAccCreationCounts;
        std::map<G4String, G4Accumulable<G4int>*> fAccInteractionCounts;

        SteppingAction* fSteppingAction;
    };

}

#endif





