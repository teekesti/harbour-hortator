# Saving the draft to history takes a snapshot; it does not clear or reset the draft

When the user saves the current draft (see ADR-0005) into workout history, the editor keeps showing and editing the same draft afterward. History receives an independent copy taken at the moment of saving — later edits to the draft do not retroactively change the saved history entry, and saving again creates a separate history entry rather than overwriting the first. The alternative was treating "Save" as "file away and start fresh," clearing the editor back to a blank workout as part of saving.

Picked the snapshot-without-clearing behavior because it's the simpler mental model — "Save" only ever adds to history, it never resets editor state as a side effect. Starting fresh is a distinct user intent and is tracked separately as an explicit "New workout" action (#9), rather than being an implicit side effect of saving.
