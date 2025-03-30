
#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "G4LogicalVolume.hh"

class G4Step;
class G4Event;

namespace G4_BREMS {
    class RunAction;

    // Structure to store SiPM hit information
    struct SipmHit {
        G4int sipmID;
        G4String sipmName;
        G4double time;
        G4ThreeVector position;
        G4double energy;
        G4double wavelength;
    };
    extern std::vector<SipmHit> gSipmHits;

    class SteppingAction : public G4UserSteppingAction
    {
    public:
        SteppingAction(RunAction* runAction);
        virtual ~SteppingAction();

        virtual void UserSteppingAction(const G4Step*);
        void SetRunAction(RunAction* runAction) { fRunAction = runAction; }

        // Methods for SiPM hit handling
        void ClearHits() { fSipmHits.clear(); }
        const std::vector<SipmHit>& GetSipmHits() const { return fSipmHits; }

    private:
        RunAction* fRunAction;
        G4LogicalVolume* fSensitiveVolume;
        std::vector<SipmHit> fSipmHits;
    };

}

#endif

