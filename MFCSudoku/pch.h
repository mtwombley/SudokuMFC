// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#define PLOG_CAPTURE_FILE
#define TRACEVARS
#include <plog/Log.h>
#include <plog/Init.h>
//#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Formatters/COutFormatter.h>
#include <plog/Appenders/DebugOutputAppender.h>

extern plog::DebugOutputAppender<plog::COutFormatter>  debugOutputAppender;

struct PLOGInitializer
{
  PLOGInitializer()
  {
    plog::init( plog::debug, &debugOutputAppender );
  }
};
#endif //PCH_H
