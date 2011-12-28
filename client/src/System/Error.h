#ifndef WINDOW_ERROR_H
#define WINDOW_ERROR_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <stdarg.h>

////////////////////////////////////////////////////////////
/// Reporting error module / logger
////////////////////////////////////////////////////////////
#define ERROR_FILEPATH		"data/errors.log"

////////////////////////////////////////////////////////////
/// Show an custom msgbox error
///
/// \param Message : Text to show
/// \param Title   : Title to show
///
////////////////////////////////////////////////////////////
void MessageError(char * Title, char * Message, ...);

#endif
