#include <stdio.h>
#include <string.h>
#include "opengl_boilerplate.h"

#define TGT_GL_VMAJ 3
#define TGT_GL_VMIN 3

int LoadOpenGL(void *file, GLADloadproc loader) {
	gladLoadGLLoader(loader);

	// print system info
    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (file)
		fprintf(file, "Using OpenGL %s%s on %s.\n",
			(const char*)glGetString(GL_VERSION),
			(flags & GL_CONTEXT_FLAG_DEBUG_BIT) ? " (debug)" : "(non-debug)",
			(const char*)glGetString(GL_RENDERER));

    // check required version
	int majVer, minVer;
	glGetIntegerv(GL_MAJOR_VERSION, &majVer), glGetIntegerv(GL_MINOR_VERSION, &minVer);
	if (majVer < TGT_GL_VMAJ && minVer < TGT_GL_VMIN && file)
		fprintf(file, "Unsupported OpenGL version: %d.%d (current) is lower than %d.%d (required)!",
				majVer, minVer, TGT_GL_VMAJ, TGT_GL_VMIN);

	// check required extensions
	char const *reqExt[] = {"GL_KHR_debug", "GL_ARB_ES2_compatibility", "GL_ARB_direct_state_access"};
	int constexpr reqExtLen = sizeof(reqExt)/sizeof(*reqExt);
	bool hasExt[reqExtLen] = { false };
	int extCnt = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extCnt);
	for(int i = 0; i < extCnt; i++) {
		char const *extName = (char*) glGetStringi(GL_EXTENSIONS, i);
		for (int j = 0; j < reqExtLen; j++)
			if (!hasExt[j] && strcmp(extName, reqExt[j]) == 0)
				hasExt[j] = true;
	}
	if (file)
		for (int i = 0; i < reqExtLen; i++)
			if (!hasExt[i])
				fprintf(file, "Required OpenGL extension %s is not supported!\n", reqExt[i]);

	// setup debug messaging
	glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gr_glDebugMessageCallback, NULL); // 2nd param is user data ptr
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // enable culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	return 0;
}

void gr_glDebugMessageCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam)
{
    /* if(id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return; */
    char *ssource, *stype, *sseverity;
    switch (source) {
        case GL_DEBUG_SOURCE_API:				ssource = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		ssource = "window system"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:	ssource = "shader compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:		ssource = "third party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:		ssource = "application"; break;
        case GL_DEBUG_SOURCE_OTHER:				ssource = "other"; break;
        default:								__builtin_unreachable();
    }
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               stype = "error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: stype = "depricated"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  stype = "undefined behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         stype = "portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         stype = "performance"; break;
        case GL_DEBUG_TYPE_MARKER:              stype = "marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          stype = "push group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           stype = "pop group"; break;
        case GL_DEBUG_TYPE_OTHER:               stype = "other"; break;
        default:								__builtin_unreachable();
    }
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         sseverity = "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       sseverity = "medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          sseverity = "low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: sseverity = "notification"; break;
        default:							 __builtin_unreachable();
    }
    switch (type) {
    	case GL_DEBUG_TYPE_ERROR:   fprintf(stderr, "OpenGL %s %u:\n\t\tSource: %s\n\t\tSeverity: %s\n\t\tMessage: %s\n\t\tUser param: %p",
                         				stype, id, ssource, sseverity, message, userParam); break;
    	default:                    fprintf(stdout, "OpenGL %s %u:\n\t\tSource: %s\n\t\tSeverity: %s\n\t\tMessage: %s\n\t\tUser param: %p",
                         				stype, id, ssource, sseverity, message, userParam); break;
    }
}
