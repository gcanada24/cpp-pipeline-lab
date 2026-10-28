// Capture++ — timecode: a moment in a recording.     Lesson 02: read lessons/02-take-details/LESSON.md
//
// A Timecode is a frame count at a frame rate. Shown as HH:MM:SS:FF, e.g. "01:02:03:04" at 30 fps means
// 1 hour, 2 minutes, 3 seconds and 4 frames. Mocap stages record at 30, 60 or 120 fps.
#pragma once

#include <string>

namespace capture::model {

class Timecode {
public:
    // STEP 4 — YOU declare these here (and define them in timecode.cpp):
    //   - a constructor taking (int frame_rate, int total_frames)
    //   - int frame_rate()     the frame rate it was made with         (callable on a const Timecode)
    //   - int total_frames()   frames since 00:00:00:00                (callable on a const Timecode)
    //   - == : two Timecodes are equal when both the rate and the frame count match
    //
    // STEP 5 — std::string to_string()   "HH:MM:SS:FF", every part at least 2 digits     (const)
    //
    // STEP 6 — operator+ : Timecode + int    -> a new Timecode, that many frames later     (const)
    //          operator- : Timecode - Timecode -> int, the number of frames between them    (const)
    //          (both Timecodes in a - have the same frame rate; you don't need to check it)

private:
    // STEP 4 — YOU declare the data members here: the frame rate and the total frame count.
};

}  // namespace capture::model
