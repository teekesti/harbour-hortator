# Opening a history entry replaces the draft outright, confirming only when the draft has unsaved changes

Loading a saved workout from history overwrites the current draft (ADR-0005) rather than opening it in a separate preview/comparison mode. A confirmation prompt appears only when the current draft differs from its own last-saved state in history (i.e. there are changes that would be silently lost); loading when the draft is already saved or unchanged proceeds without prompting. The alternative was a dual-pane or preview mode that lets the user inspect a history entry before committing to load it.

Picked outright replacement to keep one editor and one mental model (ADR-0005's single-draft design), rather than introducing a second view mode whose relationship to "the" draft would need its own rules. The conditional confirmation protects against accidental data loss without nagging on the common case of loading a workout when there's nothing at risk.
