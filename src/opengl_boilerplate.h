#ifndef INCL_OPENGL_BOILERPLATE_H
#define INCL_OPENGL_BOILERPLATE_H

#include "glad.h"

/// \brief Load OpenGL using Glad and set up some default settings. You need to have a valid OpenGL context before calling this function.
/// \return 0 on success, 1 for non-critical warnings, 2 otherwise
/// \param FILE_PTR A <c>FILE*</c> to use for logging, or NULL to disable logging.
int LoadOpenGL(void *FILE_PTR, GLADloadproc loader);

/// \brief Something to do with with custom debug messages from OpenGL. Copied this code from an old project of mine.
void gr_glDebugMessageCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam);

#endif // ifndef INCL_OPENGL_BOILERPLATE_H
