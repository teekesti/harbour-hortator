# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Sailfish OS interval/exercise timer app (`harbour-exercisetimer`), similar in purpose to GymBoss. Built with the Sailfish SDK (Qt/QML + C++ backend), packaged as RPM.

## Build & run

This is a Sailfish OS application — it's built and run via the **Sailfish SDK** (SailfishOS-Build-Engine + Sailfish IDE/SFDK), not a plain desktop Qt build, since it links against `sailfishapp` and `Sailfish.Silica`.

- Open `harbour-exercisetimer.pro` in Sailfish IDE / Qt Creator with the Sailfish SDK kit, or build with `sfdk build` from the SDK toolchain.
- Debug tracing is controlled via `.pro` file: `DEFINES += ENABLE_FUNC_TRACE ENABLE_TRACE` (already on) — see `src/eoqttrace.h` for the `TRACE`/`FUTR` macro family used throughout the C++ for qDebug output.
- Translations live in `translations/*.ts` (`fi`, `de`); `CONFIG += sailfishapp_i18n` regenerates `.qm` files at build time. New translatable strings need an entry added to `TRANSLATIONS` in the `.pro` file if a new language file is introduced.

## Tests

`tests/tests.pro` is a standalone Qt Test (`testlib`) suite covering the C++ engine (`TimedExercise`, `ExerciseListModel`, light coverage of `ExerciseTimer` — not its wall-clock tick loop). It compiles the engine sources directly and is independent of `sailfishapp`/Silica, so the main app's `.pro`/packaging is untouched.

Build and run via the Sailfish SDK's build engine (`sfdk`), using the `i486` target so the binary runs natively on the host. Use a shadow build (`build-tests/`, gitignored) so generated `Makefile`/`moc_*.cpp`/`.o` files never land in `tests/`:

```
sfdk config --global target=SailfishOS-<version>-i486   # one-time
cd <repo root> && sfdk build-init                        # one-time, creates .sfdk/
sfdk build-shell bash -c "mkdir -p build-tests && cd build-tests && qmake ../tests/tests.pro && make -j$(nproc) && ./tst_engine"
```

Re-run just `make -j$(nproc) && ./tst_engine` inside `build-tests/` for subsequent iterations — only re-run `qmake` when `tests.pro` changes.

`qt5-qttest-devel` must be installed in the target (`sfdk build-shell --maintain zypper -n install qt5-qttest-devel`) — it's not installed by default.

`ExerciseTimer` has a constructor flag `ExerciseTimer(QObject *parent = 0, bool enableSound = true)` — tests pass `enableSound=false` to skip constructing a real `SoundPlayer` (which needs an audio backend); `tests/soundplayer_stub.cpp` provides link-only stub implementations of `SoundPlayer`'s methods, since the real `src/soundplayer.cpp` depends on `sailfishapp.h`.

## Architecture

The app splits cleanly into a C++ business-logic/model layer (registered as QML types under the `com.appiukko.exercisetimer` import) and a QML UI layer.

### C++ layer (`src/`)

- **`ExerciseTimer`** (`exercisetimer.h/cpp`) — the central state machine and clock. Owns an `ExerciseListModel`, drives a `QTimer`-based tick loop (`timerEvent`, 100ms interval) that advances `currentRunningTime`/`totalRunningTime`, computes per-activity and overall progress, fires rep notifications (sound) at computed intervals, and advances to the next list item when an activity's duration is reached. Also handles the pre-start countdown (`startDelay`) and an end-of-activity warning countdown (`endWarningTime`), both driven through a shared `mCountdownTimer` + `countDown(int)` signal consumed by `SoundPlayer`. Settings (`startDelay`, `endWarningTime`) persist via `QSettings`. Exposed to QML as the `exerciseTimer` context property (set up in `harbour-exercisetimer.cpp`).
- **`ExerciseListModel`** (`exerciselistmodel.h/cpp`) — `QAbstractListModel` wrapping `QVector<TimedExercise*>`; backs the `SilicaListView` in `FirstPage.qml`. Exposed as the `exerciseListModel` context property. Supports reordering/duplication (`moveItems`, `copyItems`) for drag-reorder UI, and tracks aggregate duration via `totalDurationChanged(int)`.
- **`TimedExercise`** (`timedexercise.h/cpp`) — a single work or rest entry (`activityType` is `"work"` or `"rest"`), with `mins`/`secs` duration, `reps`, derived `rpm`/`repSeparation`, and `isValid` (used to gate whether the overall exercise list can be started — see `ExerciseTimer::checkOverallValidity`/`allValid`).
- **`SoundPlayer`** (`soundplayer.h/cpp`) — wraps `QSoundEffect`s for countdown numbers (`sounds/*.wav`, e.g. `three.wav`, `two.wav`, `one.wav`, `zero.wav`) plus round-start/round-end/rep/all-done sounds. Driven entirely by signals from `ExerciseTimer` (`countDown(int)`, direct `playSound()` calls) — it has no knowledge of timer internals.
- QML types are registered once in `main()` (`harbour-exercisetimer.cpp`); `exerciseTimer` and `exerciseListModel` are injected as QML context properties rather than instantiated in QML.

### QML layer (`qml/`)

- `harbour-exercisetimer.qml` — `ApplicationWindow` entry point; `initialPage` is `FirstPage`.
- `pages/FirstPage.qml` — exercise list editor: a `SilicaListView` bound to `exerciseListModel`, with per-row controls (`components/ValueAdjustmentHorizontal.qml`, `components/AcceleratingIconButton.qml`) for editing mins/secs/reps and a `ListItem` context menu for delete/duplicate/etc. Drives the model purely through `exerciseTimer`'s invokable slots (`addExercise`, `removeExercise`, `modifyExercise`, ...) — it does not mutate `TimedExercise` objects out-of-band.
- `pages/RunPage.qml` — the running/active view: countdown overlay, current/total `ProgressBar`s bound to `exerciseTimer.currentProgress`/`totalProgress`, and play/pause/reset `IconButton`s calling `exerciseTimer.start()/pause()/reset()`. Uses `Nemo.KeepAlive` (`DisplayBlanking`, `KeepAlive`) to keep the screen/CPU awake only while a workout is actively running.
- `qml/cover/CoverPage.qml` — minimal cover page shown when the app is backgrounded.
- `qml/pages/components/` — reusable Silica-based UI controls (declared via local `qmldir`), imported with `import "components"`.

### Key flow to understand before changing timer behavior

`FirstPage` (edit list) → user taps run → `RunPage` calls `exerciseTimer.start()` → optional `startDelay` countdown (`startCountDown`/`onCountDown`/`countDown` signal → `SoundPlayer`) → `playCurrentExercise()` starts the `timerEvent` tick loop for the current `TimedExercise` → on duration reached, `onCurrentExerciseFinished()` advances `mCurrentExerciseIndex` and either starts the next activity or emits `allExercisesFinished()`. Progress/time QML bindings (`currentProgress`, `totalProgress`, `currentRunningTime`, etc.) are all derived inside `ExerciseTimer` from this single tick loop — there's no separate UI-side timer.

## Notes

- `harbour-exercisetimer.pro.user` is a local Qt Creator/SDK user-environment file, not meant to be committed.

## Agent skills

### Issue tracker

Issues live in GitHub Issues (`teekesti/harbour-exercisetimer`), managed via the `gh` CLI. External PRs are not a triage surface. See `docs/agents/issue-tracker.md`.

### Triage labels

Default label vocabulary (`needs-triage`, `needs-info`, `ready-for-agent`, `ready-for-human`, `wontfix`) — no repo-specific overrides. See `docs/agents/triage-labels.md`.

### Domain docs

Single-context layout: `CONTEXT.md` + `docs/adr/` at the repo root. See `docs/agents/domain.md`.
