#include "emoji.h"

QString EmojiTime[24] = {"🕛", "🕧", "🕐", "🕜",
                        "🕑", "🕝", "🕒", "🕞",
                        "🕓", "🕟", "🕔", "🕠",
                        "🕕", "🕡", "🕖", "🕢",
                        "🕗", "🕣", "🕘", "🕤",
                        "🕙", "🕥", "🕚", "🕦"};

QString getEmojiFrameLoading(size_t _frame)
{
    return _frame >= 0 && _frame < 24 ? EmojiTime[_frame]: EmojiTime[0];
}
