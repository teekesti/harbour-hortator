# Draft, History, and the Exercise Template library persist as JSON files

The Draft (ADR-0005), History (ADR-0005/0006), and Exercise Template library (ADR-0008) are each persisted as JSON, written to the app's local data directory — not a database (e.g. SQLite). The alternative considered was a database, given multiple related persisted entities now exist.

Picked JSON files because all three are small, tree-shaped, and read/written wholesale (the whole Draft, the whole History list, the whole Template library) rather than queried piecemeal — there's no need for indexed lookups, joins, or partial writes that would justify a database's overhead. This mirrors the existing `QSettings`-based persistence already used for `startDelay`/`endWarningTime`, just for structured data instead of scalars.
