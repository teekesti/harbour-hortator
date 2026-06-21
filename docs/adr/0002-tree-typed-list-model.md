# Represent the workout as a tree of typed nodes (Set, Exercise), not a flat list with group markers

To express Sets containing Exercises, we introduce a new `ExerciseSet` type that owns an ordered list of `TimedExercise` children plus its own `rounds` count. The top-level `ExerciseListModel` becomes a list of `ExerciseSet` only. The alternative was keeping `ExerciseListModel` as today's flat `QVector<TimedExercise*>` and tagging items with a `groupId`/boundary marker to denote set membership.

Picked the typed tree because it encodes the two-level cap (see ADR-0001) structurally — a Set's children are statically `TimedExercise`, so a Set-within-a-Set isn't representable by accident — whereas the marker approach pushes that invariant into runtime checks and invents sentinel/marker items that the player and UI both have to special-case. The cost is that `ExerciseListModel` and `FirstPage.qml`'s list rendering both change shape, since the model is no longer a flat list of one type.
