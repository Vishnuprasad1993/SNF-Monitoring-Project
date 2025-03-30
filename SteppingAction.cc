
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4Threading.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include <sstream>

namespace {
    G4Mutex mutex = G4MUTEX_INITIALIZER;
}

namespace G4_BREMS {
    std::vector<SipmHit> gSipmHits;
    G4Mutex sipmHitsMutex = G4MUTEX_INITIALIZER;
    G4_BREMS::SteppingAction::SteppingAction(RunAction* runAction)
        : G4UserSteppingAction(),
        fRunAction(runAction),
        fSensitiveVolume(nullptr)
    {
    }

    G4_BREMS::SteppingAction::~SteppingAction()
    {
    }

    void G4_BREMS::SteppingAction::UserSteppingAction(const G4Step* step)
    {
        // Skip processing in master thread
        if (G4Threading::IsMasterThread()) return;

        // Get the track and check if it's an optical photon
        G4Track* track = step->GetTrack();
        if (!track || track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return;

        // Get position and energy information
        G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
        G4double globalTime = step->GetPreStepPoint()->GetGlobalTime();
        G4double edep = step->GetTotalEnergyDeposit();
        G4double energy = track->GetTotalEnergy();
        G4double wavelength = (1239.84193 * eV) / energy;

        // Get volume name
        G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
        if (!volume) return;
        G4LogicalVolume* logicalVolume = volume->GetLogicalVolume();
        if (!logicalVolume) return;
        G4String volumeName = logicalVolume->GetName();

        G4String creatorName = "Primary";
        const G4VProcess* creatorProcess = track->GetCreatorProcess();
        if (creatorProcess != nullptr) {
            creatorName = creatorProcess->GetProcessName();
        }

        // Get process names
        G4String processName = "Unknown";
        const G4StepPoint* postStepPoint = step->GetPostStepPoint();
        if (postStepPoint) {
            const G4VProcess* process = postStepPoint->GetProcessDefinedStep();
            if (process) {
                processName = process->GetProcessName();
            }
        }

        // Track WLS events
        if (creatorName == "OpWLS") {
            // This is a re-emitted photon
            G4double reEmitEnergy = track->GetKineticEnergy();
            G4double reEmitWavelength = (1239.84193 * eV) / reEmitEnergy;

            auto analysisManager = G4AnalysisManager::Instance();
            analysisManager->FillH1(3, reEmitEnergy / eV);    // Energy after WLS
            analysisManager->FillH1(5, reEmitWavelength);   // Wavelength after WLS
            analysisManager->FillH1(10, reEmitWavelength);
        }
        else if (processName == "OpWLS") {
            // This is a photon about to be absorbed by WLS
            G4double absorbEnergy = track->GetKineticEnergy();
            G4double absorbWavelength = (1239.84193 * eV) / absorbEnergy;

            auto analysisManager = G4AnalysisManager::Instance();
            analysisManager->FillH1(2, absorbEnergy / eV);    // Energy before WLS
            analysisManager->FillH1(4, absorbWavelength);   // Wavelength before WLS
        }

        // Update process counts
        fRunAction->AddProcessCount(volumeName, creatorName, true);
        fRunAction->AddProcessCount(volumeName, processName, false);

        if (volumeName == "Tile") {
            fRunAction->IncrementTileCount();
        }
        else if (volumeName == "FiberClad") {
            fRunAction->IncrementCladCount();
        }
        else if (volumeName == "FiberCore") {
            fRunAction->IncrementCoreCount();
        }
        else if (volumeName == "Sipm") {
            fRunAction->IncrementSipmCount();
        }
        else {
            fRunAction->IncrementOtherCount();
        }

        G4VPhysicalVolume* preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
        G4VPhysicalVolume* postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
        G4double stepNum = step->GetTrack()->GetCurrentStepNumber();

        if (preVolume && postVolume) {
            G4String preVolumeName = preVolume->GetLogicalVolume()->GetName();
            G4String postVolumeName = postVolume->GetLogicalVolume()->GetName();

            if (preVolumeName != postVolumeName) {

                if ((postVolumeName == "FiberCore" && preVolumeName == "FiberClad") ||
                    (postVolumeName == "FiberClad" && preVolumeName == "Tile") && creatorName != "OpWLS") {
                    G4double initial_energy = track->GetKineticEnergy();
                    G4double initial_vertex_energy = track->GetVertexKineticEnergy();
                    G4double initial_Wavelength = (1239.84193 * eV) / initial_energy;
                    G4ThreeVector initial_momentum = track->GetMomentum();
                    G4ThreeVector initial_direction = track->GetMomentumDirection();

                    fRunAction->IncrementPhotonsEnteredFiber();
                }

                if ((postVolumeName != "Tile" && postVolumeName != "World") &&
                    (preVolumeName == "FiberCore" || preVolumeName == "FiberClad")
                    && creatorName == "OpWLS") {

                    G4double final_energy = track->GetKineticEnergy();
                    G4double final_vertex_energy = track->GetVertexKineticEnergy();
                    G4double final_Wavelength = (1239.84193 * eV) / final_energy;
                    G4ThreeVector final_momentum = track->GetMomentum();
                    G4ThreeVector final_direction = track->GetMomentumDirection();

                    fRunAction->IncrementPhotonsAbsorbedFiber();
                }

                if ((postVolumeName == "Sipm") && (preVolumeName == "FiberCore" || preVolumeName == "FiberClad")
                    && creatorName == "OpWLS") {
                
                    G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
                    G4double hitTimeLocal = step->GetPostStepPoint()->GetLocalTime();
                    G4ThreeVector hitPosition = step->GetPreStepPoint()->GetPosition();
                    G4ThreeVector hitPositionSipm = step->GetPostStepPoint()->GetPosition();

                    G4double hitEnergy = track->GetTotalEnergy();
                    G4double hitWavelength = (1239.84193 * eV) / hitEnergy; // Wavelength in nm

                    G4VPhysicalVolume* physVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
                    G4String fullSipmName = physVolume->GetName();
                    G4int sipmID = step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber();
                    //G4cout << "Hit SiPM with name: " << fullSipmName
                        //<< ", Copy Number: " << sipmID << G4endl;
                    // Store hit information
                    SipmHit hit;
                    hit.sipmID = sipmID;
                    hit.sipmName = fullSipmName;
                    hit.time = hitTime;
                    hit.position = hitPosition;
                    hit.energy = hitEnergy;
                    hit.wavelength = hitWavelength;
                    fSipmHits.push_back(hit);

                    G4AutoLock lock(&sipmHitsMutex);
                    gSipmHits.push_back(hit);
                    lock.unlock();


                    G4cout << "Hit SiPM with name: " << fullSipmName << " " << "Hit Time: " << hitTime << " "
                        << "Hit Local Time: " << hitTimeLocal << " " << "Hit Position: "
                        << hitPosition << " " << "Hit Wavelength: " << hitWavelength << " " << "Pre Volume: " << preVolumeName
                        << " " << "Hit Position Sipm: " << hitPositionSipm 
                        //<< " " << "Energy Deposition in Sipm: "
                        //<< edepSipm 
                        << G4endl;
                    //G4cout << "Hit Time: " << hitTime << " " << "Hit Position: " << hitPosition << " " << "Hit Energy: "
                        //<< hitEnergy << " " << "Hit Wavelength: " << hitWavelength << " " << "Pre Volume: " << preVolumeName
                        //<< " Step Number in Sipm: " << stepNum << " SiPM ID: " << sipmID << G4endl;

                    //G4cout << "Adding SiPM hit to collection, current size: " << fSipmHits.size() << G4endl;
                    fSipmHits.push_back(hit);
                    //G4cout << "New collection size: " << fSipmHits.size() << G4endl;

                    auto analysisManager = G4AnalysisManager::Instance();
                    analysisManager->FillH1(11, hitTime / ns);
                    analysisManager->FillH1(12, hitWavelength);
                    analysisManager->FillH2(12, hitPositionSipm.x() / mm, hitPositionSipm.y() / mm, hitTime);
                    analysisManager->FillH2(13, hitPositionSipm.y() / mm, hitPositionSipm.z() / mm, hitTime);
                    analysisManager->FillH2(14, hitPositionSipm.x() / mm, hitPositionSipm.z() / mm, hitTime);

                }
            }
        }

        auto analysisManager = G4AnalysisManager::Instance();

        // Fill 1D histograms
        analysisManager->FillH1(0, edep / MeV);
        analysisManager->FillH1(1, globalTime / ns);

        // Fill 2D histograms with timing
        analysisManager->FillH2(0, position.x() / mm, position.y() / mm, globalTime / ns);
        analysisManager->FillH2(1, position.y() / mm, position.z() / mm, globalTime / ns);
        analysisManager->FillH2(2, position.x() / mm, position.z() / mm, globalTime / ns);

        // Fill 2D histograms with energy deposition
        analysisManager->FillH2(3, position.x() / mm, position.y() / mm, edep / MeV);
        analysisManager->FillH2(4, position.y() / mm, position.z() / mm, edep / MeV);
        analysisManager->FillH2(5, position.x() / mm, position.z() / mm, edep / MeV);

        // Fill volume-specific histograms
        if (volumeName == "FiberClad") {
            analysisManager->FillH1(6, wavelength);  // Wavelength in cladding
            analysisManager->FillH1(7, energy / eV); // Energy in cladding
            analysisManager->FillH2(6, position.x() / mm, position.y() / mm, edep / MeV);
            analysisManager->FillH2(7, position.y() / mm, position.z() / mm, edep / MeV);
            analysisManager->FillH2(8, position.x() / mm, position.z() / mm, edep / MeV);
        }
        else if (volumeName == "FiberCore") {
            analysisManager->FillH1(8, wavelength);  // Wavelength in core
            analysisManager->FillH1(9, energy / eV); // Energy in core
            analysisManager->FillH2(9, position.x() / mm, position.y() / mm, edep / MeV);
            analysisManager->FillH2(10, position.y() / mm, position.z() / mm, edep / MeV);
            analysisManager->FillH2(11, position.x() / mm, position.z() / mm, edep / MeV);
        }
    }

} // namespace G4_BREMS
