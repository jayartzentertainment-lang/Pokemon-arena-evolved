# Native adventure integration0.3.0

The game now boots to Continue/New Adventure/Battle Lab. Character creation and saved exploration are real runtime systems surrounding the preserved portable combat controller. Story encounters construct parties and return HP/PP/status/results to the adventure. This is a condensed re-authored Sinnoh campaign, not an exact Platinum map/script port.

34 native maps and41 story objectives,28 constructed battle events,4 story2v2 setups. Actual Platinum trainer-party JSON is imported for named trainer battles; short rival/partner2v2 encounters have explicit native teams. Eight badges and a Champion flag are earned only from successful story results. The lab is separate and cannot award story progress.

All493 canonical base species have genuine directional animation data/atlases. Their presence in the lab is separate from complete move-mechanics and encounter coverage. Trainer appearance uses the supplied base/clothes/hair/hat/accessory walking pieces, not recolored Lucas. The same layered appearance is used in the world and save.

Save version1 encodes fields explicitly with a size header/CRC32, validates counts/ranges/mon data before accepting it, stages writes and retains a known-good backup. Recovery never promotes a corrupt primary over a good backup. Save errors are visible. The old0.1/0.2 projects and DS source/release are unchanged.

No full campaign gameplay completion or physical hardware performance is claimed. Scripted host progression injects battle outcomes to verify all event transitions, gates, badges and ending flags; this is labeled separately from emulator observations.
