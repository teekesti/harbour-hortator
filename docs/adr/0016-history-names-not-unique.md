# History entry names are not required to be unique

Each History entry is identified internally by its own id/timestamp, not by name. Two entries can share the same name (e.g. two "Leg day" snapshots saved on different days) with no validation error and no automatic disambiguation. The alternative was enforcing unique names, either by blocking a save with a duplicate name or by silently appending a suffix like "(2)".

Picked no uniqueness constraint because re-saving a tweaked version of a workout under the same name it already has is a completely ordinary action, and both alternatives (blocking the save, or inventing a suffix) add friction or surprise for no real benefit — the timestamp/Summary Bar already disambiguates same-named entries when both are visible in the list.
