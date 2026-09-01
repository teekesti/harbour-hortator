# Tempo (harbour-exercisetimer)

A Sailfish OS interval/exercise timer app. You build
a workout as a list of **Sets**, each containing one or more timed **Exercises**
(work or rest periods), and either the Exercise or the whole Set can be repeated
for a number of **Rounds**. The app then plays the workout back with a countdown,
progress bars, and audio cues for rep cadence, round transitions, and completion.

Built with the Sailfish SDK (Qt/QML + C++ backend), packaged as an RPM.

## Domain model

See [CONTEXT.md](CONTEXT.md) for the ubiquitous language (Exercise, Set, Round)
and [docs/adr/](docs/adr/) for the architectural decisions behind the Set/Round
nesting model.

## Architecture

- **C++ layer** (`src/`) — `ExerciseTimer` is the central state machine and
  clock; `ExerciseListModel` backs the editor's list view; `ExerciseSet` groups
  exercises into a repeatable set; `TimedExercise` is a single work/rest entry;
  `SoundPlayer` handles audio cues.
- **QML layer** (`qml/`) — `FirstPage.qml` is the workout editor, `RunPage.qml`
  is the running/active view.

See [CLAUDE.md](CLAUDE.md) for a full breakdown of the class responsibilities
and the data flow from editing a workout to playing it back.

## Build & run

This is a Sailfish OS application — built and run via the **Sailfish SDK**
(SailfishOS-Build-Engine + Sailfish IDE/SFDK), not a plain desktop Qt build,
since it links against `sailfishapp` and `Sailfish.Silica`.

- Open `harbour-exercisetimer.pro` in Sailfish IDE / Qt Creator with the
  Sailfish SDK kit, or build with `sfdk build` from the SDK toolchain.
- Translations live in `translations/*.ts` (`fi`, `de`); `CONFIG +=
  sailfishapp_i18n` regenerates `.qm` files at build time.

## Tests

`tests/tests.pro` is a standalone Qt Test (`testlib`) suite covering the C++
engine (`TimedExercise`, `ExerciseSet`, `ExerciseListModel`, `ExerciseTimer`).
It's built and run via the Sailfish SDK's build engine (`sfdk`), using the
`i486` target so the binary runs natively on the host:

```
sfdk config --global target=SailfishOS-<version>-i486   # one-time
cd <repo root> && sfdk build-init                        # one-time, creates .sfdk/
sfdk build-shell bash -c "mkdir -p build-tests && cd build-tests && qmake ../tests/tests.pro && make -j$(nproc) && ./tst_engine"
```

See [CLAUDE.md](CLAUDE.md) for full test setup details, including the
`qt5-qttest-devel` package requirement and the `SoundPlayer` stub used to keep
the test suite independent of an audio backend.
