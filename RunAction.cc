
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include "G4AnalysisManager.hh"
#include "G4AccumulableManager.hh"
#include <iomanip>
#include <string>
#include <fstream>

namespace G4_BREMS {

    G4_BREMS::RunAction::RunAction(SteppingAction* steppingAction)
        : G4UserRunAction(),
        fTileCount(0), fCladCount(0), fCoreCount(0), fSipmCount(0), fOtherCount(0),
        fPhotonsEnteredFiber(0), fPhotonsExitedFiber(0), fPhotonsAbsorbedFiber(0),
        fAccTileCount("TileCount", 0),
        fAccCladCount("CladCount", 0),
        fAccCoreCount("CoreCount", 0),
        fAccSipmCount("SipmCount", 0),
        fAccOtherCount("OtherCount", 0),
        fAccPhotonsEnteredFiber("PhotonsEnteredFiber", 0),
        fAccPhotonsExitedFiber("PhotonsExitedFiber", 0),
        fAccPhotonsAbsorbedFiber("PhotonsAbsorbedFiber", 0),
        fSteppingAction(steppingAction)
    {
        std::vector<G4String> volumes = { "Tile", "FiberCore", "FiberClad", "Sipm" };
        for (const auto& volume : volumes) {
            // Creation process accumulables
            auto name = volume + "_Creation_Cerenkov";
            fAccCreationCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Creation_Scintillation";
            fAccCreationCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Creation_OpWLS";
            fAccCreationCounts[name] = new G4Accumulable<G4int>(name, 0);

            // Interaction process accumulables
            name = volume + "_Interaction_OpAbsorption";
            fAccInteractionCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Interaction_OpWLS";
            fAccInteractionCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Interaction_Transportation";
            fAccInteractionCounts[name] = new G4Accumulable<G4int>(name, 0);
        }

        // Register all accumulables
        auto accumulableManager = G4AccumulableManager::Instance();
        accumulableManager->RegisterAccumulable(fAccTileCount);
        accumulableManager->RegisterAccumulable(fAccCladCount);
        accumulableManager->RegisterAccumulable(fAccCoreCount);
        accumulableManager->RegisterAccumulable(fAccSipmCount);
        accumulableManager->RegisterAccumulable(fAccOtherCount);
        accumulableManager->RegisterAccumulable(fAccPhotonsEnteredFiber);
        accumulableManager->RegisterAccumulable(fAccPhotonsExitedFiber);
        accumulableManager->RegisterAccumulable(fAccPhotonsAbsorbedFiber);


        // Register process accumulables
        for (auto& pair : fAccCreationCounts) {
            accumulableManager->RegisterAccumulable(*pair.second);
        }
        for (auto& pair : fAccInteractionCounts) {
            accumulableManager->RegisterAccumulable(*pair.second);
        }

        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->SetVerboseLevel(1);
        analysisManager->SetFileName("OpticalPhotonAnalysis1");
        analysisManager->SetDefaultFileType("root");
        analysisManager->SetNtupleMerging(true);

        // Create histograms
        analysisManager->CreateH1("edep", "Energy Deposition Distribution",
            100, 0., 2.0E-5 * CLHEP::MeV);
        analysisManager->SetH1XAxisTitle(0, "Energy Deposition [MeV]");
        analysisManager->SetH1YAxisTitle(0, "Counts");

        analysisManager->CreateH1("time", "Time Distribution",
            100, 0., 3.0);
        analysisManager->SetH1XAxisTitle(1, "Photon Time [ns]");
        analysisManager->SetH1YAxisTitle(1, "Counts");

        // Create 2D histograms
        analysisManager->CreateH2("timing_xy", "XY Timing",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(0, "x [mm]");
        analysisManager->SetH2YAxisTitle(0, "y [mm]");
        analysisManager->SetH2ZAxisTitle(0, "Time [ns]");

        analysisManager->CreateH2("timing_yz", "YZ Timing",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(1, "y [mm]");
        analysisManager->SetH2YAxisTitle(1, "z [mm]");
        analysisManager->SetH2ZAxisTitle(1, "Time [ns]");

        analysisManager->CreateH2("timing_xz", "XZ Timing",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(2, "x [mm]");
        analysisManager->SetH2YAxisTitle(2, "z [mm]");
        analysisManager->SetH2ZAxisTitle(2, "Time [ns]");

        analysisManager->CreateH2("edep_xy", "XY Energy Deposition",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(3, "x [mm]");
        analysisManager->SetH2YAxisTitle(3, "y [mm]");
        analysisManager->SetH2ZAxisTitle(3, "Energy [MeV]");

        analysisManager->CreateH2("edep_yz", "YZ Energy Deposition",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(4, "y [mm]");
        analysisManager->SetH2YAxisTitle(4, "z [mm]");
        analysisManager->SetH2ZAxisTitle(4, "Energy [MeV]");

        analysisManager->CreateH2("edep_xz", "XZ Energy Deposition",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(5, "x [mm]");
        analysisManager->SetH2YAxisTitle(5, "z [mm]");
        analysisManager->SetH2ZAxisTitle(5, "Energy [MeV]");

        analysisManager->CreateH2("clad_xy", "Cladding XY",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(6, "x [mm]");
        analysisManager->SetH2YAxisTitle(6, "y [mm]");
        analysisManager->SetH2ZAxisTitle(6, "Energy [MeV]");

        analysisManager->CreateH2("clad_yz", "Cladding YZ",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(7, "y [mm]");
        analysisManager->SetH2YAxisTitle(7, "z [mm]");
        analysisManager->SetH2ZAxisTitle(7, "Energy [MeV]");

        analysisManager->CreateH2("clad_xz", "Cladding XZ",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(8, "x [mm]");
        analysisManager->SetH2YAxisTitle(8, "z [mm]");
        analysisManager->SetH2ZAxisTitle(8, "Energy [MeV]");

        analysisManager->CreateH2("core_xy", "Core XY",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(9, "x [mm]");
        analysisManager->SetH2YAxisTitle(9, "y [mm]");
        analysisManager->SetH2ZAxisTitle(9, "Energy [MeV]");

        analysisManager->CreateH2("core_yz", "Core YZ",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(10, "y [mm]");
        analysisManager->SetH2YAxisTitle(10, "z [mm]");
        analysisManager->SetH2ZAxisTitle(10, "Energy [MeV]");

        analysisManager->CreateH2("core_xz", "Core XZ",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(11, "x [mm]");
        analysisManager->SetH2YAxisTitle(11, "z [mm]");
        analysisManager->SetH2ZAxisTitle(11, "Energy [MeV]");

        analysisManager->CreateH1("PhotonEnergyBeforeWLS", "Photon Energy Before WLS",
            100, 2.0, 3.5);
        analysisManager->SetH1XAxisTitle(2, "Energy [eV]");
        analysisManager->SetH1YAxisTitle(2, "Counts");

        analysisManager->CreateH1("PhotonEnergyAfterWLS", "Photon Energy After WLS",
            100, 2.0, 3.5);
        analysisManager->SetH1XAxisTitle(3, "Energy [eV]");
        analysisManager->SetH1YAxisTitle(3, "Counts");

        analysisManager->CreateH1("PhotonWavelengthBeforeWLS", "Photon Wavelength Before WLS",
            100, 350., 600.);
        analysisManager->SetH1XAxisTitle(4, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(4, "Counts");

        analysisManager->CreateH1("PhotonWavelengthAfterWLS", "Photon Wavelength After WLS",
            100, 350., 600.);
        analysisManager->SetH1XAxisTitle(5, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(5, "Counts");

        analysisManager->CreateH1("CladdingWavelength", "Photon Wavelength in Cladding",
            100, 300., 600.);
        analysisManager->SetH1XAxisTitle(6, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(6, "Counts");

        analysisManager->CreateH1("CladdingEnergy", "Photon Energy in Cladding",
            100, 1.5, 4.1);
        analysisManager->SetH1XAxisTitle(7, "Energy [eV]");
        analysisManager->SetH1YAxisTitle(7, "Counts");

        analysisManager->CreateH1("CoreWavelength", "Photon Wavelength in Core",
            100, 300., 600.);
        analysisManager->SetH1XAxisTitle(8, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(8, "Counts");

        analysisManager->CreateH1("CoreEnergy", "Photon Energy in Core",
            100, 1.5, 4.1);
        analysisManager->SetH1XAxisTitle(9, "Energy [eV]");
        analysisManager->SetH1YAxisTitle(9, "Counts");

        analysisManager->CreateH1("WLSEmissionSpectrum", "WLS Emission Spectrum",
            200, 300., 600.);
        analysisManager->SetH1XAxisTitle(10, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(10, "Counts");

        analysisManager->CreateH1("SipmTimeSpectrum", "Sipm Time Spectrum",
            100, 0., 300.0);
        analysisManager->SetH1XAxisTitle(11, "Time [ns]");
        analysisManager->SetH1YAxisTitle(11, "Counts");

        analysisManager->CreateH1("SipmWavelength", "Photon Wavelenght in Sipm",
            200, 300, 600);
        analysisManager->SetH1XAxisTitle(12, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(12, "Counts");

        analysisManager->CreateH2("Sipm_Timing_xy", "Sipm XY Timing",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(12, "x [mm]");
        analysisManager->SetH2YAxisTitle(12, "y [mm]");
        analysisManager->SetH2ZAxisTitle(12, "Time [ns]");

        analysisManager->CreateH2("Sipm_Timing_yz", "Sipm YZ Timing",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(13, "y [mm]");
        analysisManager->SetH2YAxisTitle(13, "z [mm]");
        analysisManager->SetH2ZAxisTitle(13, "Time [ns]");

        analysisManager->CreateH2("Sipm_Timing_xz", "Sipm XZ Timing",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(14, "x [mm]");
        analysisManager->SetH2YAxisTitle(14, "z [mm]");
        analysisManager->SetH2ZAxisTitle(14, "Time [ns]");

    }

    G4_BREMS::RunAction::~RunAction()
    {
        // Clean up accumulables
        for (auto& pair : fAccCreationCounts) {
            delete pair.second;
        }
        for (auto& pair : fAccInteractionCounts) {
            delete pair.second;
        }
    }

    void G4_BREMS::RunAction::BeginOfRunAction(const G4Run*)
    {
        // Reset accumulables
        G4AccumulableManager::Instance()->Reset();

        // Clear SiPM hits from previous run
        if (fSteppingAction) {
            fSteppingAction->ClearHits();
        }

        if (G4Threading::IsMasterThread()) {
            gSipmHits.clear();
        }


        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->Reset();

        if (!analysisManager->OpenFile()) {
            G4ExceptionDescription msg;
            msg << "Failed to open file " << analysisManager->GetFileName();
            G4Exception("RunAction::BeginOfRunAction()",
                "Analysis_W001", JustWarning, msg);
        }

        //auto analysisManager = G4AnalysisManager::Instance();
        G4String fileName = "Sipm_Hits.root";
        
        analysisManager->OpenFile(fileName);

    }


    void G4_BREMS::RunAction::AddProcessCount(const G4String& volume,
        const G4String& processName,
        bool isCreationProcess)
    {
        G4String key = volume + (isCreationProcess ? "_Creation_" : "_Interaction_") + processName;

        if (isCreationProcess) {
            auto it = fAccCreationCounts.find(key);
            if (it != fAccCreationCounts.end()) {
                *(it->second) += 1;
            }
        }
        else {
            auto it = fAccInteractionCounts.find(key);
            if (it != fAccInteractionCounts.end()) {
                *(it->second) += 1;
            }
        }
    }

    G4double G4_BREMS::RunAction::CalculateTrappingEfficiency() const {
        G4double photonsEntered = fAccPhotonsEnteredFiber.GetValue();
        G4double photonsExited = fAccPhotonsExitedFiber.GetValue();
        G4double photonsAbsorbed = fAccPhotonsAbsorbedFiber.GetValue();

        G4cout << "Photons Entered: " << photonsEntered << " " << "Photon Exited: " << photonsExited << " " << "Photons Absorbed: "
            << photonsAbsorbed << G4endl;
        if (photonsEntered == 0) return 0.0; // Avoid division by zero
        return photonsAbsorbed / photonsEntered;
    }

    void G4_BREMS::RunAction::EndOfRunAction(const G4Run* run)
    {
        auto analysisManager = G4AnalysisManager::Instance();

        // Merge all accumulables
        G4AccumulableManager::Instance()->Merge();

        if (G4Threading::IsMasterThread()) {
            // Update volume counts from accumulables
            fTileCount = fAccTileCount.GetValue();
            fCladCount = fAccCladCount.GetValue();
            fCoreCount = fAccCoreCount.GetValue();
            fOtherCount = fAccOtherCount.GetValue();
            fSipmCount = fAccSipmCount.GetValue();
            fPhotonsEnteredFiber = fAccPhotonsEnteredFiber.GetValue();
            fPhotonsExitedFiber = fAccPhotonsExitedFiber.GetValue();
            fPhotonsAbsorbedFiber = fAccPhotonsAbsorbedFiber.GetValue();

            // Print summary
            G4cout << "\n=== Final Process Counts Summary ===" << G4endl;
            G4cout << "\nVolume Hit Counts:" << G4endl;
            G4cout << "Tile Count: " << fTileCount << G4endl;
            G4cout << "Cladding Count: " << fCladCount << G4endl;
            G4cout << "Core Count: " << fCoreCount << G4endl;
            G4cout << "Sipm Count: " << fSipmCount << G4endl;
            G4cout << "Other Count: " << fOtherCount << G4endl;

            // Print trapping efficiency
            G4double trappingEfficiency = CalculateTrappingEfficiency();
            G4cout << "\nTrapping Efficiency: " << trappingEfficiency * 100.0 << "%" << G4endl;

            // Print process counts for each volume
            std::vector<G4String> volumes = { "Tile", "FiberCore", "FiberClad", "Sipm" };
            for (const auto& volume : volumes) {
                G4cout << "\nVolume: " << volume << G4endl;

                // Creation processes
                G4cout << "Creation Processes:" << G4endl;
                std::vector<G4String> creationProcesses = { "Cerenkov", "Scintillation", "OpWLS" };
                for (const auto& process : creationProcesses) {
                    G4String key = volume + "_Creation_" + process;
                    auto it = fAccCreationCounts.find(key);
                    if (it != fAccCreationCounts.end()) {
                        G4cout << std::setw(15) << process << ": "
                            << it->second->GetValue() << G4endl;
                    }
                }

                // Interaction processes
                G4cout << "Interaction Processes:" << G4endl;
                std::vector<G4String> interactionProcesses = { "OpAbsorption", "OpWLS", "Transportation" };
                for (const auto& process : interactionProcesses) {
                    G4String key = volume + "_Interaction_" + process;
                    auto it = fAccInteractionCounts.find(key);
                    if (it != fAccInteractionCounts.end()) {
                        G4cout << std::setw(15) << process << ": "
                            << it->second->GetValue() << G4endl;
                    }
                }
            }
            G4cout << "\nAttempting to write " << gSipmHits.size() << " SiPM hits to CSV file." << G4endl;
            //G4cout << "\nAttempting to write " << gSipmHits.size() << " SiPM hits to ROOT file." << G4endl;

            /*
           
            if (!gSipmHits.empty()) {
                // Generate filename with run number
                std::string filename = "sipm_hits_run" + std::to_string(run->GetRunID()) + ".csv";
                //std::string filename = "sipm_hits_run" + std::to_string(run->GetRunID()) + ".root";
                G4cout << "Creating file: " << filename << G4endl;

                std::ofstream outFile(filename);
                if (outFile.is_open()) {
                    // Write header
                    //outFile << "SipmID,SipmName,Time(ns),X(mm),Y(mm),Z(mm),Energy(eV),Wavelength(nm)" << std::endl;
                    //outFile << "SipmName,Time(ns),X(mm),Y(mm),Z(mm),Energy(eV),Wavelength(nm)" << std::endl;
                    outFile << "SipmName,Time(ns),X(mm),Y(mm),Z(mm),Energy(eV),Wavelength(nm), TimeWindow(ns),ChargeDepositionCounts" 
                            << std::endl;

                    // First, sort all hits by SiPM ID and then by time
                    std::map<G4String, std::vector<SipmHit>> hitsBySipm;
                    for (const auto& hit : gSipmHits) {
                        hitsBySipm[hit.sipmName].push_back(hit);
                    }

                    // Process each SiPM's hits
                    for (auto& sipmPair : hitsBySipm) {
                        auto& sipmName = sipmPair.first;
                        auto& hits = sipmPair.second;

                        // Sort hits by time
                        std::sort(hits.begin(), hits.end(),
                            [](const SipmHit& a, const SipmHit& b) {
                                return a.time < b.time;
                            });
                        const G4double timeWindow = 100.0 * ns; // 100 ns time window
                        //const G4double photonToCharge = 1.602e-7; // pC per photon (adjust based on your SiPM gain)

                        // For each hit, determine its time window and charge contribution
                        for (size_t i = 0; i < hits.size(); i++) {
                           const auto& hit = hits[i];

                           // Find the start of the time window for this hit
                           G4double windowStart = hit.time;
                           G4double windowEnd = windowStart + timeWindow;

                           // Count photons in this window
                           G4int photonsInWindow = 0;
                           for (size_t j = i; j < hits.size(); j++) {
                                if (hits[j].time >= windowStart && hits[j].time <= windowEnd) {
                                    photonsInWindow++;
                                }
                                if (hits[j].time > windowEnd) break;
                           }

                           // Calculate charge for this hit
                           //G4double charge = photonToCharge * photonsInWindow;
                    
                           // Write hit data with charge information
                           outFile << hit.sipmName << ","
                               << hit.time / ns << ","
                               << hit.position.x() / mm << ","
                               << hit.position.y() / mm << ","
                               << hit.position.z() / mm << ","
                               << hit.energy / eV << ","
                               << hit.wavelength << ","
                               << windowStart / ns << "-" << windowEnd / ns << ","
                               //<< charge << std::endl;
                               << photonsInWindow << std::endl;
                        }
                    }
                    outFile.close();
                    G4cout << "Successfully wrote " << gSipmHits.size() << " SiPM hits with charge information to "
                        << filename << G4endl;
                }
                else {
                    G4cerr << "Error: Could not open " << filename << " for writing" << G4endl;
                }
            }
            */
            if (!gSipmHits.empty()) {
                // Generate filename with run number
                std::string filename = "sipm_hits_run" + std::to_string(run->GetRunID()) + ".csv";
                G4cout << "Creating file: " << filename << G4endl;

                std::ofstream outFile(filename);
                if (outFile.is_open()) {
                    // Write header
                    outFile << "SipmName,Time(ns),X(mm),Y(mm),Z(mm),Energy(eV),Wavelength(nm),TimeBin(ns),PhotonsInBin"
                        << std::endl;

                    // Group hits by SiPM
                    std::map<G4String, std::vector<SipmHit>> hitsBySipm;
                    for (const auto& hit : gSipmHits) {
                        hitsBySipm[hit.sipmName].push_back(hit);
                    }

                    // Process each SiPM's hits
                    for (auto& sipmPair : hitsBySipm) {
                        auto& sipmName = sipmPair.first;
                        auto& hits = sipmPair.second;

                        // Sort hits by time
                        std::sort(hits.begin(), hits.end(),
                            [](const SipmHit& a, const SipmHit& b) {
                                return a.time < b.time;
                            });

                        // Define bin size
                        const G4double binSize = 100.0 * ns; // 100 ns non-overlapping bins

                        // Calculate bins for this SiPM
                        std::map<G4int, G4int> binCounts; // binIndex -> count
                        for (const auto& hit : hits) {
                            G4int binIndex = static_cast<G4int>(hit.time / binSize);
                            binCounts[binIndex]++;
                        }

                        // Write hit data with bin information
                        for (const auto& hit : hits) {
                            G4int binIndex = static_cast<G4int>(hit.time / binSize);
                            G4double binStart = binIndex * binSize;
                            G4double binEnd = (binIndex + 1) * binSize;
                            G4int photonsInBin = binCounts[binIndex];

                            outFile << hit.sipmName << ","
                                << hit.time / ns << ","
                                << hit.position.x() / mm << ","
                                << hit.position.y() / mm << ","
                                << hit.position.z() / mm << ","
                                << hit.energy / eV << ","
                                << hit.wavelength << ","
                                << binStart / ns << "-" << binEnd / ns << ","
                                << photonsInBin << std::endl;
                        }
                    }
                    outFile.close();
                    G4cout << "Successfully wrote " << gSipmHits.size() << " SiPM hits with binned charge information to "
                        << filename << G4endl;
                }
                else {
                    G4cerr << "Error: Could not open " << filename << " for writing" << G4endl;
                }
            }



            else {
                G4cout << "No SiPM hits recorded in this run" << G4endl;
            }

            G4cout << "\n=================================" << G4endl;
        }

        // Write and close file
        analysisManager->Write();
        analysisManager->CloseFile(false);
    }
} // namespace G4_BREMS


