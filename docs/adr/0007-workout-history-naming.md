# A saved workout requires a name, pre-filled with a sensible default

When saving the draft to history (ADR-0006), the user is prompted for a name in a text field that comes pre-filled with a default (e.g. today's date, or "Workout N") so a single tap/Enter still saves successfully without typing anything. The user can overwrite the default with something meaningful (e.g. "Leg day"), and can rename a history entry later — the name is not fixed at save time. The alternative was auto-naming with no user-facing field at all (e.g. timestamp-only, rename-only-after-the-fact).

Picked a pre-filled, editable, required field because it costs the user nothing extra for the common case (just confirm) while making meaningful names a first-class, no-friction option rather than something bolted on via a separate rename step every time.
