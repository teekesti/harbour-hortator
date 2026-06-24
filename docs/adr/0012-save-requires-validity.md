# Saving the draft to history requires the same validity the app already requires to play it

A workout can only be saved to history if it passes the same overall-validity check (`ExerciseTimer`'s existing `allValid`/`checkOverallValidity`) that already gates whether the draft can be started on `RunPage` today. An invalid draft cannot be saved — the Save action is disabled or rejected with the same validity feedback already shown in the editor. The alternative was allowing invalid drafts into history, deferring the validity check to whenever the entry is later played.

Picked requiring validity at save time because every history entry should be playable on demand without a separate "fix it up first" step — the whole point of history is replaying a workout later, and a saved-but-broken entry would defeat that. This also means history never needs to track or surface a "this saved workout is currently invalid" state.
