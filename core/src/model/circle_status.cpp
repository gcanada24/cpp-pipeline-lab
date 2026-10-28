// Capture++ — a take's circle status (definitions).   Lesson 02: read lessons/02-take-details/LESSON.md
#include "capture/model/circle_status.h"

namespace capture::model {
namespace {  // private to this file: constants and helper functions

// STEP 3: your list of every CircleStatus goes here

}  // namespace

// STEP 2 ------------------------------------------------------------------------------------------
std::string_view to_string(CircleStatus status) {
    (void)status;  // delete this line once you use status
    return {};
}

// STEP 3 ------------------------------------------------------------------------------------------
std::optional<CircleStatus> parse_circle_status(std::string_view text) {
    (void)text;
    return std::nullopt;
}

}  // namespace capture::model
