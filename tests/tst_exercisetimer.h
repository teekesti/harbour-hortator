#ifndef TST_EXERCISETIMER_H
#define TST_EXERCISETIMER_H

#include <QObject>

class TstExerciseTimer : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void durationAggregation();
    void validityReflectsExercises();
    void startDelayBounds();
    void startDelayPersists();
    void endWarningTimeBounds();
    void endWarningTimePersists();
    void muteSoundsToggleAndPersists();
    void draftPersistsAcrossRestart();
    void draftDirtyTracksEdits();
    void saveDraftToHistoryRequiresValidity();
    void saveDraftToHistorySucceedsAndClearsDirty();
    void loadHistoryEntryReplacesDraftAndMarksDirtyClean();
    void historySortsMostRecentlyPlayedFirst();
    void namingExerciseUpsertsTemplate();
    void renamingExerciseDoesNotTouchOldTemplate();
    void addExerciseFromTemplateCopiesByValue();
    void removingTemplateDoesNotAffectExistingExercise();
    void invalidExerciseDoesNotUpsertTemplate();
    void resetDraftToDefaultClearsAndSyncs();
    void resetOnFreshTimer();
    void currentActivityOnEmptyModel();
    void startOnEmptyModelIsNoOp();
    void startThenPauseTransitionsRunning();
    void pauseDuringStartDelayFreezesAndResumesFromRemainingCount();
    void resumingMidExercisePauseDoesNotReplayStartDelay();
    void resumingMidExercisePauseDoesNotReplaySetupSideEffects();
    void resumingAfterPauseDuringEndOfExerciseWarningGoesStraightBackToRunning();
    void resumingAfterPauseDuringEndOfExerciseWarningRestartsCountdownTimer();
    void resetDuringStartDelayClearsWaitingAndPausedFlags();

    void emptySetBlocksValidity();
    void noSetsBlocksValidity();
    void durationAccountsForSetAndExerciseRounds();
    void playSequenceFlattensSetAndExerciseRounds();
    void positionPropertiesDuringPlayback();
    void outOfRangeSetIndexIsNoOp();

    void skipLastRestDefaultsToFalse();
    void skipLastRestToggleAndPersists();
    void skipLastRestSkipsWhenLastItemIsRest();
    void skipLastRestNoOpWhenLastItemIsWork();
    void skipLastRestNoOpWhenFlagOff();

    void showFirstUseHintsDefaultsFalseAndIsNotPersisted();
};

#endif // TST_EXERCISETIMER_H
