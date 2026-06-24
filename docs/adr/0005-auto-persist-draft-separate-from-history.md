# Auto-persist the in-progress workout as a draft, separate from explicitly-saved workout history

Today the workout being edited in `ExerciseListModel` lives purely in memory and is lost when the app closes. We're adding two distinct persisted things: a single **draft** (the current in-progress workout, auto-saved continuously so killing the app never loses edits) and a **history** of workouts the user explicitly saves by name. The alternative was treating every edit as implicitly writing into history, with no separate draft concept.

Picked the split because it matches how the app already treats settings (`startDelay`/`endWarningTime` auto-persist with no user action) while keeping history as a curated list the user deliberately builds, rather than auto-polluting it with every in-progress edit, abandoned experiment, or accidental change. The draft has no name and is not itself a history entry; saving promotes (a copy of) it into history under a name the user provides.
