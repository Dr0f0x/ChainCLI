#include "log_streambuffer.h"

namespace cli::logging
{
int LogStreamBuf::sync()
{
    if (lvl < minLevel)
        return 0; // skip

    if (auto msg = str(); !msg.empty())
    {
        (*logFuncPtr)(lvl, msg); // call the function
        str("");                 // clear the buffer
    }
    return 0;
}
} // namespace cli::logging
