# History entries are deleted only explicitly; no automatic cap or pruning

Each History row gets an explicit delete action (the same `ListItem` context-menu pattern `FirstPage.qml` already uses for Sets/Exercises). There is no automatic limit on how many entries History can hold and no silent pruning of old entries. The alternative was a cap (hard limit blocking further saves, or auto-pruning the oldest entry once a limit is reached).

Picked explicit-only deletion because silently losing a saved workout the user never asked to delete would be surprising, and the cost of not pruning is negligible — each entry is a handful of Sets/Exercises, so even a long History list is a tiny amount of data. If list size ever becomes a real UX problem, that's a future search/sort feature, not a reason to delete data automatically today.
