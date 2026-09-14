# Pause freezes the Start Delay and End-of-exercise Warning countdowns, not just the exercise tick loop; the two countdowns collapse onto one timer

`ExerciseTimer` exposes three independent bools — `running`, `paused`, `waitingToStart` — rather than a single enum state. Extending Pause to cover the Start Delay countdown (and, incidentally, the End-of-exercise Warning countdown) is represented as `paused` and `waitingToStart` both being `true` at once, not a fourth state or property. `RunPage`'s existing `backNavigation` guard (`!running && !paused && !waitingToStart`) already reads this correctly with no change, since it only cares whether *any* of the three is set.

This only works because the Start Delay and End-of-exercise Warning countdowns are collapsed onto the single `mCountdownTimer`/`mCountdownTracker` pair. Previously, Start Delay was *also* driven by an independent `QTimer::singleShot` racing the countdown timer to the same instant purely to flip into `playCurrentExercise()` — a duplicate mechanism with no handle to pause. Removing it (the tracker reaching zero while `waitingToStart` now triggers the transition directly) means Pause only ever has one countdown timer to stop/resume, using the same `stop()`/`start()` pattern the tick loop already used.

`start()`'s resume path had to stop assuming "resuming from pause" always means "replay the full Start Delay" — that was only true by coincidence, because until now nothing could pause *during* the delay itself. It now branches on what was paused (mid-exercise vs. mid-Start-Delay) instead of restarting the delay unconditionally, which also fixes a latent bug: resuming an ordinary mid-exercise pause was replaying the entire Start Delay countdown before continuing, which was never intended.

## Considered Options

- A separate `countdownPaused` property, keeping `paused`/`waitingToStart` mutually exclusive. Rejected: it would need its own wiring through every consumer of `paused`, for no behavioral difference from the compound-state reading.
- Keep the two Start-Delay timers separate and add elapsed-time pause tracking to the `singleShot`. Rejected: adds pause bookkeeping on top of a mechanism that was already fragile (two timers required to agree on one instant); collapsing to one timer removes the fragility instead of managing it.

## Consequences

Resuming a paused Start Delay restarts `mCountdownTimer` with a fresh 1000ms interval rather than the sub-second remainder actually left before pausing (unlike the very first tick of a fresh Start Delay, which `startCountDown()` fires immediately to avoid exactly this lag). In practice this means the frozen digit can sit on screen for up to ~1s longer than a real unpaused countdown would after each pause/resume cycle. Accepted deliberately: fixing it would mean reintroducing the elapsed-time bookkeeping this ADR just removed, for a cosmetic sub-second discrepancy in a lead-in countdown.
