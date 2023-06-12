#pragma once

// default value: false
// will be set to true when interrupted by an signal handler
extern bool interrupted_by_sig_handler;

// must be called to initialize all handlers
// returns:  0 if ok
//          -1 if errors
int init_sig_handlers();
