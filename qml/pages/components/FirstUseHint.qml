import QtQuick 2.0
import Sailfish.Silica 1.0

// A small blinking indicator over a control, played a few times to point
// out a non-obvious gesture (see #14, ADR-0019). Starts/stops based on
// `active` and reports natural completion via finished() - cancellation
// is just setting `active` back to false, which emits no finished().
TapInteractionHint {
    id: root

    property bool active: false
    signal finished()

    loops: 3

    // TapInteractionHint defaults to running immediately on creation;
    // force it off until something actually activates it.
    Component.onCompleted: stop()

    onActiveChanged: {
        if (active) {
            start()
        } else {
            stop()
        }
    }

    onRunningChanged: {
        if (!running && active) {
            finished()
        }
    }
}
