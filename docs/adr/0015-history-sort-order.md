# History list sorts by most-recently-played, falling back to created-time

Each History entry tracks both a created timestamp (when first saved) and a last-played timestamp (updated whenever it's played, whether via direct-play or after loading into the editor). The list's default sort order is most-recently-played first; an entry that has never been played sorts by its created timestamp instead, so a freshly-saved workout still surfaces near the top rather than sinking below already-played entries. The alternative was sorting by created-time only, or alphabetically by name.

Picked recency-of-play as the primary sort because History exists primarily for replay, not archival — the workout most likely wanted next is usually the one played most recently. Falling back to created-time for unplayed entries avoids the alternative failure mode of new saves disappearing to the bottom of a long list before ever being played once.
