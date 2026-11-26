/*
 // Copyright (c) 2026 Timothy Schoen and others.
 // For information on usage and redistribution, and for a DISCLAIMER OF ALL
 // WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */
#pragma once

extern "C" {
#include <m_pd.h>
#include <g_canvas.h>
}

#include "Pd/Instance.h"

// Muting Detective Mode for one object or one subpatch.
//
// A muted connection is left out of Detective Mode: it isn't shown and doesn't wait its turn, though its
// messages are still recorded for its tooltip. Two settings per box, kept by pd::Instance for as long as the
// box exists (not in the patch file), and carried over when a patch is reloaded from disk:
//   DetectiveMuteMessages  the messages this object sends
//   DetectiveMuteInside    every connection inside this subpatch or abstraction, and inside the subpatches it holds
//
// The functions that read the patch expect the caller to hold Pd's lock.
namespace DetectiveMute {

// The box a subpatch or abstraction canvas sits in, in its parent; nullptr for a top-level patch, and for
// the copies inside a [clone], which have an owner but aren't boxes in it
inline t_gobj* boxOf(t_canvas* canvas)
{
    if (!canvas || !canvas->gl_owner)
        return nullptr;

    auto* box = &canvas->gl_obj.te_g;
    for (auto* y = canvas->gl_owner->gl_list; y; y = y->g_next) {
        if (y == box)
            return box;
    }
    return nullptr;
}

// Whether this canvas's own box has "everything inside" muted
inline bool isOwnInsideMuted(pd::Instance* pd, t_canvas* canvas)
{
    auto* box = boxOf(canvas);
    return box && (pd->getDetectiveMute(box) & pd::Instance::DetectiveMuteInside);
}

// Whether every connection in this canvas is muted: its own box, or the box of any canvas it sits in, has
// "everything inside" muted
inline bool isInsideMuted(pd::Instance* pd, t_canvas* canvas)
{
    for (auto* c = canvas; c && c->gl_owner; c = c->gl_owner) {
        if (isOwnInsideMuted(pd, c))
            return true;
    }
    return false;
}

} // namespace DetectiveMute
