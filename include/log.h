#ifndef _LOG_H_
#define _LOG_H_

// ----------------------------------------------------------------------------
// PREPROCESSOR MACROS
// ----------------------------------------------------------------------------

/* -- logging macros */
// strips path from __FILE__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// structure enables logging to add endlines
struct tmpEndl {
  ~tmpEndl () {std::cerr << std::endl;}
};

// logging macro that is visible in release and debug modes
#define LOG (tmpEndl(),\
          std::cerr << "[" << __FILENAME__ << ":" << __LINE__ << "] ")

// debug macro that only logs messages in debug mode
// #ifndef DEBUG_MESSAGES
#define DEBUG_MESSAGES 0
//#endif

#if DEBUG_MESSAGES
#define DEBUG LOG<<"DEBUG | "
#else
#define DEBUG 0 && LOG << "DEBUG | "
#endif

// throws runtime error with filename and line numbers
#define throwRuntimeError(x) do { \
        std::stringstream ss;\
        ss << "[" << __FILENAME__ << ":" << __LINE__ << "] " << x;\
        throw std::runtime_error(ss.str());\
      } while(0)


#endif // _LOG_H_
