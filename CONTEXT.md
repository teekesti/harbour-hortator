# Exercise Timer

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
