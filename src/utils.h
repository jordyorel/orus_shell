// utils.h

#ifndef UTILS_H
#define UTILS_H

#include "orus.h"  // Ensure "orus.h" is included if necessary for Command and other types

// Declare the cleanup functions
void cleanup_command_list(Command *commands, int num_commands);  // Declaration of cleanup_command_list
void cleanup_redirections(RedirectInfo *redirect);

#endif
