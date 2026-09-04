# Hortator

A Sailfish OS interval/exercise timer. The domain is the structure and playback of a workout: an ordered sequence of timed activities a user steps through.

## Language

**Exercise**:
A single timed activity — work or rest — with a duration. The leaf, playable unit; always belongs to exactly one Set.
_Avoid_: Activity, Step

**Set**:
An ordered group of Exercises that is itself repeated as a whole (see Round). A workout is an ordered list of Sets. Nesting stops here — a Set cannot contain another Set.
_Avoid_: Group, Circuit, Superset

**Round**:
The repeat count attached to an Exercise or a Set, controlling how many times that item is replayed in sequence before moving on to the next. Distinct from "reps," which counts beep notifications fired *during* a single Exercise's duration (its cadence), not how many times the Exercise itself is replayed.
_Avoid_: Reps (reserved for the existing beep-cadence count), Repeat, Cycle

**Workout**:
The ordered list of Sets being edited or played — the top-level structure described above. At any moment there is exactly one workout open in the editor (see Draft).
_Avoid_: Routine, Program

**Draft**:
The single workout currently open in the editor. Auto-persisted continuously so it survives app restarts without any explicit save action. Distinct from a History entry: the draft has no name and is not itself part of History until explicitly saved. See ADR-0005.
_Avoid_: Current workout, Session

**History**:
The persisted list of workouts the user has explicitly saved by name, each a snapshot independent of the Draft and of each other — editing the Draft or one History entry never changes another. Saving requires the workout to be valid (ADR-0012). Opening a History entry for editing replaces the Draft (ADR-0011); History entries can also be played directly without opening them, recognized via a Summary Bar and one-line text summary rather than full detail.
_Avoid_: Saved workout (use "History entry"), Library (reserved for Exercise Template)

**Summary Bar**:
A shared visual component: a horizontal bar of work/rest-colored sub-rectangles, one per Exercise occurrence, widthed proportionally to duration. Shown both in the editor (near total duration) and per-row in the History list, alongside a one-line text summary. See ADR-0013.
_Avoid_: Timeline, Progress bar (reserved for playback progress in RunPage)

**Exercise Template**:
A named, reusable starting point — `{name, default duration, default reps}` — kept in a flat library independent of any Workout. Copied **by value** into a new Exercise when picked; later edits to either side never propagate. Naming an Exercise implicitly upserts a template by that name (ADR-0009); templates can also be deleted directly from a library page. See ADR-0008, ADR-0009.
_Avoid_: Library item (use "Exercise Template"), Preset
