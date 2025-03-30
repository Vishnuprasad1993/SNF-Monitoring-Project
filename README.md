# SNF-Monitoring-Project

GEANT4 Simulation for the Antineutrino Detector

Studying positron interactions from the inverse beta decay events and 

1) PrimaryGeneratorAction
           The details regarding the particle type (e+), energy, direction, and source position.
2) PhysicsList
           Positron annihilation produce 2 gamma rays, which causes scintillation in the plastic scintillator tiles. Considered only optical photons for simulation.
3) Actioninit
4) DetectorConstruction
           Detector Components Used
               1) Plastic Scintillator Tiles
                         Polymer Base: Polystyrene
                         Refractive Index : 1.59
                         Dimension 200 (W) x 200 (L) x 5 (D) mm
                         Four 1.5 (W) x 2.5 (D) mm grooves for placement of optical fibres for light transmission
               2) Wavelength Shifting (WLS) Fibers
                         Type: BCF-91A
                         Square Fibers (1.5 x 1.5 x 200 mm)
                         Shift Blue light to Green
                         1) Core
                             Material : Polystyrene
                             Refractive Index : 1.60
                             Produce Wavelength shifted photons

                         2) Cladding
                             Material : PMMA
                             Refractive Index : 1.49
                             Surrounds the core
                             Prevents Optical Photon Loss




